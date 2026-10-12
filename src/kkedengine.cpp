#include "kkedengine.h"

KKEDEngine::KKEDEngine()
{
  KDevLogger::getInstance().logEvent("KKEDEngine object constructed");

  const parameter_data::KParameterData& constants =
        KFileManager::getInstance().m_workingParameterSet;

  const double A1 = constants.m_KEDA1;
  const double A2 = constants.m_KEDA2;
  const double A3 = constants.m_KEDA3;

  //First, do the empircal value adjustment
  m_muULorentz = KMath::empirical_mu_adjust(KAtomicData::m_muU, 
                                            KAtomicData::m_l0U,
                                            KAtomicData::m_l1U,
                                            KAtomicData::m_l2U,
                                            KAtomicData::m_l3U,
                                            KAtomicData::m_h1U,
                                            KAtomicData::m_h2U,
                                            KAtomicData::m_h3U,  
                                            KAtomicData::m_kEdgeU);
  m_muPuLorentz = KMath::empirical_mu_adjust(KAtomicData::m_muPu, 
                                             KAtomicData::m_l0Pu,
                                             KAtomicData::m_l1Pu,
                                             KAtomicData::m_l2Pu,
                                             KAtomicData::m_l3Pu,
                                             KAtomicData::m_h1Pu,
                                             KAtomicData::m_h2Pu,
                                             KAtomicData::m_h3Pu,  
                                             KAtomicData::m_kEdgePu);

  //Next, Gauss broaden the mu table from XCOM. This is faster than doing
  // a gauss-hermite quadrature.
  m_muULorentz = KMath::gaussian_broaden_table(m_muULorentz, A1, A2, A3);
  m_muPuLorentz = KMath::gaussian_broaden_table(m_muPuLorentz, A1, A2, A3);
  //Finally, Lorentz broaden the K-edges.
  m_muULorentz =  KMath::lorentz_broadening_step(m_muULorentz, KAtomicData::m_kEdgeU, 0.6);
  m_muPuLorentz = KMath::lorentz_broadening_step(m_muPuLorentz, KAtomicData::m_kEdgePu, 0.6);
}

KKEDEngine::~KKEDEngine()
{
  KDevLogger::getInstance().logEvent("KKEDEngine object destructed");
}

void KKEDEngine::send(int a_message)
{
}

void KKEDEngine::receive(int a_message)
{
}


bool KKEDEngine::fitting_analysis(const double a_sampTemp)
{
  // --------------------------------------------------------------
  // Load the data
  // --------------------------------------------------------------
  const std::vector<double>& energies = KFileManager::getInstance().m_kedSampData.m_energies;
  std::vector<double>& counts   = KFileManager::getInstance().m_kedSampData.m_originalCounts;
  const std::size_t nObs = energies.size();

  const parameter_data::KParameterData& constants =
        KFileManager::getInstance().m_workingParameterSet;

  // --------------------------------------------------------------
  // Calculate random coincidence summing
  // --------------------------------------------------------------
  std::vector<double> coincCounts = compute_coinc_summing_tsn(counts);

  // --------------------------------------------------------------
  // Subtract the coincidence from the data prior to fitting
  // --------------------------------------------------------------
//  for (int i = 0; i < nObs; ++i) 
//  {
//    counts[i] = counts[i] - coincCounts[i];
//  }

  // --------------------------------------------------------------
  // Calculate the step continuum
  // --------------------------------------------------------------
  std::vector<double> continuumCounts = compute_continuum(counts);


  // --------------------------------------------------------------
  // Initial guesses
  // --------------------------------------------------------------
  double params[17] = {
                      1.0,   // [0]  C
                      0.88,  // [1]  chi
                      0.1,   // [2]  rhoU, g/ml
                      0.1,   // [3]  rhoPu, g/ml
                      1.4,   // [4]  rho matrix material, g/ml
                      40000, // [5]  Cd peak height
                      0.25,  // [6]  Cd peak sigma, keV
                      0.1,   // [7]  Cd filter thickness
                      1.0,   // [8]  Fe filter thickness
                      250,   // [9]  Pb xray 84.4 Ht    
                      0.25,  // [10] Pb xray 84.4 Sigma 
                      1000,  // [11] Pb xray 84.9 Ht    
                      0.25,  // [12] Pb xray 84.9 Sigma 
                      1200,  // [13] Pb xray 87.3 Ht    
                      0.25,   // [14] Pb xray 87.3 Sigma 
                      0.0,   // [15] delta_E0, keV
                      0.0    // [16] delta_gain, dimensionless
                    };

  ceres::Problem problem;

  // --------------------------------------------------------------
  // Build residual blocks
  // --------------------------------------------------------------
  for (int i = 0; i < nObs; ++i) 
  {
    const double E = energies[i];
    const double Y = counts[i];
    const double C = continuumCounts[i];

    if (E <= constants.m_KEDFitLowerBound || 
        E >= constants.m_KEDFitUpperBound) continue;

  /* By default AutoDiffCostFunction will take ownership of the cost 
   * functor pointer passed to it, ie. will call delete on the cost functor
   * when the AutoDiffCostFunction itself is deleted. However, 
   * this may be undesirable in certain cases, therefore it is also possible 
   * to specify DO_NOT_TAKE_OWNERSHIP as a second argument in the constructor, 
   * while passing a pointer to a cost functor which does not need to be 
   * deleted by the AutoDiffCostFunction.
   */
    auto* term = new KKEDSourceTerm(E, Y, C);
    term->m_muPuLorentz = m_muPuLorentz;
    term->m_muULorentz  = m_muULorentz;
    /* Do not need to manually delete the AutoDiffCostFunction pointer 
     * as it is passed to AddResidualBlock. Ceres handles this automatically 
     */
    //ceres::CostFunction* cost = new ceres::AutoDiffCostFunction<KKEDSourceTerm, 1, 10>(term);
    ceres::CostFunction* cost =
        new ceres::NumericDiffCostFunction<
            KKEDSourceTerm,
            ceres::CENTRAL,
            1, 17>(term);

    problem.AddResidualBlock(cost, new ceres::HuberLoss(1.0), params);// optional robust loss
  }

  // --------------------------------------------------------------
  // Add one calibration prior residual block
  // --------------------------------------------------------------
  const double offsetSigma = 0.05;
  const double slopeSigma  = 5e-4;

  ceres::CostFunction* prior_cost =
        new ceres::AutoDiffCostFunction<KEnergyCalibrationPrior, 2, 17>(
        new KEnergyCalibrationPrior(offsetSigma, slopeSigma));
  
  problem.AddResidualBlock(prior_cost, nullptr, params);

  // --------------------------------------------------------------
  // Solver options
  // --------------------------------------------------------------
  ceres::Solver::Options opts;
  opts.max_num_iterations = 50;
  opts.linear_solver_type = ceres::DENSE_QR;
  opts.minimizer_progress_to_stdout = false;
  opts.function_tolerance = 1e-6; //default 1e-6
  opts.gradient_tolerance = 1e-8; //default 1e-10
  //set some parameter bounds
  //problem.SetParameterLowerBound(params, 2, 0.0); // rhoU
  //problem.SetParameterLowerBound(params, 3, 0.0); // rhoPu
  //problem.SetParameterLowerBound(params, 4, 1.0); // rhoMatrix
  //problem.SetParameterLowerBound(params, 5, 0.0); // Cd 109 pk ht
  //problem.SetParameterLowerBound(params, 6, 0.0); // Cd 109 peak sigma
  //problem.SetParameterLowerBound(params, 7, 0.0); // Cd attenuator dx
  //problem.SetParameterLowerBound(params, 8, 2.4); // Fe attenuator dx
  // --------------------------------------------------------------
  // Run the optimizer
  // --------------------------------------------------------------
  ceres::Solver::Summary summary;
  ceres::Solve(opts, &problem, &summary);
  //std::cout << summary.FullReport() << "\n";

  if (summary.IsSolutionUsable()) 
  {
     std::cout << "Optimization successful! Proceeding with usable solution.\n";
  } 
  else 
  {
      std::cerr << "Ceres failed to find a usable solution: " << summary.message << "\n";
      return false;
  }

  // ---------------------------------------------------------------------------
  // Compute the covariance 
  // ---------------------------------------------------------------------------
  ceres::Covariance::Options covOptions;
  covOptions.algorithm_type = ceres::SPARSE_QR; // Or DENSE_SVD for ill-conditioned matrices
  ceres::Covariance covariance(covOptions);

  double final_cost = summary.final_cost; 
  int num_residuals = summary.num_residuals;
  int num_parameters = summary.num_effective_parameters;
  int dof = num_residuals - num_parameters;
  double chiSquared = final_cost * 2.0;
  double reducedChiSquared = chiSquared / dof;
  double scaledCov_U_U;
  double scaledUncertaintyU;
  double scaledCov_Pu_Pu;
  double scaledUncertaintyPu;

  /*
  Ceres does not automatically scale the covariance by the reduced chi-squared 
  statistic. 
  If your data points have arbitrary weighting, you must manually multiply 
  the returned covariance values by final_cost*2.0/dof
  */
  // Ceres stores 'cost' as 0.5 * sum(residual^2). We multiply by 2 to get the actual Chi-Squared value.

  std::vector<std::pair<const double*, const double*>> covarianceBlocks;
  covarianceBlocks.push_back(std::make_pair(params, params));
  double cov[289];

  if (covariance.Compute(covarianceBlocks, &problem))
  {
    covariance.GetCovarianceBlock(params, params, cov);

    double unscaledCov_U_U   = cov[2 * 17 + 2]; //cov[param_index * #_params + param_index]
    double unscaledCov_Pu_Pu = cov[3 * 17 + 3];

    scaledCov_U_U = unscaledCov_U_U * reducedChiSquared;
    scaledUncertaintyU = std::sqrt(scaledCov_U_U);

    scaledCov_Pu_Pu = unscaledCov_Pu_Pu * reducedChiSquared;
    scaledUncertaintyPu = std::sqrt(scaledCov_Pu_Pu);
  }
  else 
  {
    std::cerr << "Covariance computation failed!" << std::endl;
    return false;
  }

  double C            = params[0];
  double chi          = params[1];
  double rhoU         = params[2];
  double rhoPu        = params[3];
  double rhoMatrixMat = params[4];
  double cdHt         = params[5];
  double cdSigma      = params[6];
  double cdFilterDx   = params[7];
  double feFilterDx   = params[8];
  double Pb844Ht      = params[9];
  double Pb844Sigma   = params[10];
  double Pb849Ht      = params[11];
  double Pb849Sigma   = params[12];
  double Pb873Ht      = params[13];
  double Pb873Sigma   = params[14];
  double deltaOffset  = params[15];
  double deltaSlope   = params[16];

  //correct for temp
  rhoU  = temperature_correction(rhoU, a_sampTemp, constants.m_KEDTempCorr);
  rhoPu = temperature_correction(rhoPu, a_sampTemp, constants.m_KEDTempCorr);

  // --------------------------------------------------------------
  // Store results and emit signals
  // --------------------------------------------------------------
  m_kedResults.m_ceresResults.m_terminationType = ceres::TerminationTypeToString(summary.termination_type);
  m_kedResults.m_ceresResults.m_message = summary.message;
  m_kedResults.m_ceresResults.m_initialCost = summary.initial_cost;
  m_kedResults.m_ceresResults.m_finalCost = summary.final_cost;
  m_kedResults.m_ceresResults.m_costDrop = summary.initial_cost - summary.final_cost;
  m_kedResults.m_ceresResults.m_numResidualEvals = summary.num_residual_evaluations;
  m_kedResults.m_ceresResults.m_numJacobianEvals = summary.num_jacobian_evaluations;
  m_kedResults.m_ceresResults.m_numSuccessfulSteps = summary.num_successful_steps;
  m_kedResults.m_ceresResults.m_numUnsuccessfulSteps = summary.num_unsuccessful_steps;
  m_kedResults.m_ceresResults.m_totalIterations = summary.num_successful_steps + summary.num_unsuccessful_steps;
  m_kedResults.m_ceresResults.m_totalTime = summary.total_time_in_seconds;
  m_kedResults.m_ceresResults.m_jacobianTime = summary.jacobian_evaluation_time_in_seconds;
  m_kedResults.m_ceresResults.m_linearSolverTime = summary.linear_solver_time_in_seconds;
  m_kedResults.m_ceresResults.m_linearSolverType = ceres::LinearSolverTypeToString(summary.linear_solver_type_given);

  m_kedResults.m_initialCost = summary.initial_cost;
  m_kedResults.m_finalCost = summary.final_cost;
  m_kedResults.m_numResiduals = summary.num_residuals;
  m_kedResults.m_numParams = 5;
  m_kedResults.m_converged = summary.IsSolutionUsable();

  m_kedResults.m_uConcentration = rhoU*1000.;//*1000 to get in g/L
  m_kedResults.m_uSigma = scaledUncertaintyU*1000.;
  m_kedResults.m_uRSD = m_kedResults.m_uSigma/m_kedResults.m_uConcentration*100.;//*100 to get in %
  m_kedResults.m_puConcentration = rhoPu*1000.;
  m_kedResults.m_puSigma = scaledUncertaintyPu*1000.;
  m_kedResults.m_puRSD = m_kedResults.m_puSigma/m_kedResults.m_puConcentration*100.;//*100 to get in %
  m_kedResults.m_npConcentration = 0.*1000.;
  m_kedResults.m_npSigma = 0.*1000.;
  m_kedResults.m_npRSD = 0.;
  m_kedResults.m_amConcentration = 0.*1000.;
  m_kedResults.m_amSigma = 0.*1000.;
  m_kedResults.m_amRSD = 0.;
  m_kedResults.m_cmConcentration = 0.*1000.;
  m_kedResults.m_cmSigma = 0.*1000.;
  m_kedResults.m_cmRSD = 0.;
  std::cout << "cd " << cdFilterDx <<std::endl;
  std::cout << "fe " << feFilterDx <<std::endl;
  std::cout << "HNO3 " << rhoMatrixMat <<std::endl;
  std::cout << "cdHt " << cdHt<<std::endl;
  std::cout << "cdSigma " << cdSigma<<std::endl;
  std::cout << "Pb844Ht " << Pb844Ht<<std::endl;
  std::cout << "Pb844Sigma " << Pb844Sigma<<std::endl;
  std::cout << "Pb849Ht " << Pb849Ht<<std::endl;
  std::cout << "Pb849Sigma " << Pb849Sigma<<std::endl;
  std::cout << "Pb873Ht " << Pb873Ht<<std::endl;
  std::cout << "Pb873Sigma " << Pb873Sigma<<std::endl;
  std::cout << "deltaOffset " << deltaOffset<<std::endl;
  std::cout << "deltaSlope " << deltaSlope<<std::endl;
  std::cout << "red chi sq " << reducedChiSquared <<std::endl; 
  std::vector<double> fittedEs, fittedYs, fitROIData, fittedContinuums, fittedCoinces;
  for (size_t i = 0; i < energies.size(); ++i) 
  {
    if (energies.at(i) > constants.m_KEDFitLowerBound && 
        energies.at(i) < constants.m_KEDFitUpperBound)
    {
      fittedEs.push_back(energies.at(i));
      fitROIData.push_back(counts.at(i));
      //pass a step background of zero so we don't add it twice
      fittedYs.push_back(KKEDSourceTerm::evaluate(energies.at(i), params, continuumCounts.at(i)));
      fittedContinuums.push_back(continuumCounts.at(i));
      fittedCoinces.push_back(coincCounts.at(i));
    }
  }
  //save the fit to the results object
  m_kedResults.m_fitROIEnergies = fittedEs;
  m_kedResults.m_fitROIDataCounts = fitROIData;
  m_kedResults.m_fitROIModelCounts = fittedYs;
  m_kedResults.m_continuumCounts = fittedContinuums;
  m_kedResults.m_coincCounts = fittedCoinces;

  //send the results to the analysisworker
  emit send_KED_results(m_kedResults);

  return true;
}

bool KKEDEngine::two_roi_analysis()
{
  return true;
}

double KKEDEngine::temperature_correction(const double a_value, 
                                          const double a_temp,
                                          const double a_corrFactor)
{
  return a_value*(1. + a_corrFactor*(a_temp - 25.));
}

 std::vector<double> KKEDEngine::compute_continuum(const std::vector<double>& a_counts)
{
  std::vector<double> continuumCounts;
  const std::size_t numChannels = a_counts.size();
  const parameter_data::KParameterData& constants =
        KFileManager::getInstance().m_workingParameterSet;

  //using SpecUtils method to integrate low E bkgd region
  const double lowECountSum = KFileManager::getInstance().m_kedSampData.m_measurement->gamma_integral(constants.m_KEDBkgdLowLowBound, constants.m_KEDBkgdLowHighBound);
  //FRY: needs to be updated to account for fractional bins FRY//
  const int lowEChannelSum = 
            KFileManager::getInstance().m_kedSampData.m_measurement->find_gamma_channel(constants.m_KEDBkgdLowHighBound) -
            KFileManager::getInstance().m_kedSampData.m_measurement->find_gamma_channel(constants.m_KEDBkgdLowLowBound);
  
  double lowECountAvg, highECountAvg;
  //make sure num channels is not zero or negative. If it is, use the bin value
  //at right side of bkgd region
  if(lowEChannelSum > 0)
  {
    lowECountAvg = lowECountSum/lowEChannelSum;
  }
  else
  {
    int bin = KFileManager::getInstance().m_kedSampData.m_measurement->find_gamma_channel(constants.m_KEDBkgdLowHighBound);
    lowECountAvg = KFileManager::getInstance().m_kedSampData.m_originalCounts.at(bin);
  }

  //using SpecUtils method to integrate high E bkgd region
  const double highECountSum = KFileManager::getInstance().m_kedSampData.m_measurement->gamma_integral(constants.m_KEDBkgdHighLowBound, constants.m_KEDBkgdHighHighBound);

  //FRY: needs to be updated to account for fractional bins FRY//
  const int highEChannelSum = 
            KFileManager::getInstance().m_kedSampData.m_measurement->find_gamma_channel(constants.m_KEDBkgdHighHighBound) -
            KFileManager::getInstance().m_kedSampData.m_measurement->find_gamma_channel(constants.m_KEDBkgdHighLowBound);
  
  //make sure num channels is not zero or negative. If it is, use the bin value
  //at right side of bkgd region
  if(highEChannelSum > 0)
  {
    highECountAvg = highECountSum/highEChannelSum;
  }
  else
  {
    int bin = KFileManager::getInstance().m_kedSampData.m_measurement->find_gamma_channel(constants.m_KEDBkgdHighHighBound);
    highECountAvg = KFileManager::getInstance().m_kedSampData.m_originalCounts.at(bin);
  }

  //using SpecUtils method to integrate ROI denominator region
  const double roiCountSum = KFileManager::getInstance().m_kedSampData.m_measurement->gamma_integral(constants.m_KEDBkgdLowHighBound, constants.m_KEDBkgdHighLowBound);

  //loop over data and calculate the continuum spectrum
  for (int bin = 0; bin < numChannels; ++bin) 
  {
    double numerator = 0;
    for(int i = bin; i<numChannels; ++i)
    {
      numerator += a_counts.at(i);
    }
    double continuumAti = highECountAvg + (numerator/roiCountSum)*(lowECountAvg - highECountAvg);
    continuumCounts.push_back(continuumAti);
  }
  return continuumCounts;
}

std::vector<double> KKEDEngine::compute_coinc_summing(const std::vector<double>& a_counts)
{
  std::vector<double> coincCounts;
  const double liveTime = KFileManager::getInstance().m_kedSampData.m_measurement->live_time();
  //FRY: This constant needs to be added to paramter set //FRY
  const double coincTime = 0.00000085; 
  const int numChan = a_counts.size();
  
  for (int j=0; j < a_counts.size(); ++j) 
  {
    double coincAtj = 0.0;
    for (int i=1; i < j-1; ++i) 
    {
      coincAtj += a_counts.at(i)*a_counts.at(j-i);
    }
    coincAtj = coincAtj*(coincTime/liveTime);
    coincCounts.push_back(coincAtj);
  }
  return coincCounts;
}

std::vector<double> KKEDEngine::compute_coinc_summing_tsn(const std::vector<double>& a_counts)
{
  std::vector<double> coincCounts;
  const double liveTime = KFileManager::getInstance().m_kedSampData.m_measurement->live_time();
  //FRY: This constant needs to be added to paramter set //FRY
  const double coincTime = 0.00000085; 
  const int numChan = a_counts.size();
  
  for (int j = 0; j < a_counts.size(); ++j) 
  {
    double coincAtj = 0.0;
    for (int i=1; i < j-1; ++i) 
    {
      coincAtj += (a_counts.at(i) - coincCounts.at(i))*(a_counts.at(j-i) - coincCounts.at(j-i));
    }
    coincAtj = coincAtj*(coincTime/liveTime);
    coincCounts.push_back(coincAtj);
  }
  return coincCounts;
}