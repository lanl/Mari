#include "kcalibrateengine.h"

KCalibrateEngine::KCalibrateEngine()
{
  KDevLogger::getInstance().logEvent("KCalibrateEngine object constructed");
}

KCalibrateEngine::~KCalibrateEngine()
{
  KDevLogger::getInstance().logEvent("KCalibrateEngine object destructed");
}

void KCalibrateEngine::send(int a_message)
{
}

void KCalibrateEngine::receive(int a_message)
{
}


bool KCalibrateEngine::calibrate_XRF()
{
  // --------------------------------------------------------------
  // Load the data
  // --------------------------------------------------------------
  const std::vector<double>& energies = KFileManager::getInstance().m_xrfRefData.m_energies;
  std::vector<double>& counts   = KFileManager::getInstance().m_xrfRefData.m_originalCounts;
  const std::size_t nObs = energies.size();

  const parameter_data::KParameterData& constants =
        KFileManager::getInstance().m_workingParameterSet;

  // --------------------------------------------------------------
  // Calculate random coincidence summing
  // --------------------------------------------------------------
  //std::vector<double> coincCounts = compute_coinc_summing_tsn(counts);

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
  //std::vector<double> continuumCounts = compute_continuum(counts);

  // --------------------------------------------------------------
  // Initial guesses
  // --------------------------------------------------------------
  double params[13] = {constants.m_backScattC,
                       constants.m_backScattb3,
                       constants.m_backScattb4,
                       constants.m_backScattb5,
                       constants.m_backScattb6,
                       constants.m_backScattb7,
                       constants.m_backScattb8,
                       constants.m_backScattb9,
                       constants.m_backScattb10,
                       constants.m_backScattb11,
                       constants.m_backScattb12,
                       constants.m_backScattb13,
                       constants.m_backScattb14
                      };

  ceres::Problem problem;

  // --------------------------------------------------------------
  // Build residual blocks
  // --------------------------------------------------------------
  for (int i = 0; i < nObs; ++i) 
  {
    const double E = energies[i];
    const double Y = counts[i];
    //const double C = continuumCounts[i];

    double fitLowerBound = 85;//FRY unhardcode
    double fitUpperBound = 110;
    if (E <= fitLowerBound || 
        E >= fitUpperBound) continue;

  /* By default AutoDiffCostFunction will take ownership of the cost 
   * functor pointer passed to it, ie. will call delete on the cost functor
   * when the AutoDiffCostFunction itself is deleted. However, 
   * this may be undesirable in certain cases, therefore it is also possible 
   * to specify DO_NOT_TAKE_OWNERSHIP as a second argument in the constructor, 
   * while passing a pointer to a cost functor which does not need to be 
   * deleted by the AutoDiffCostFunction.
   */
    auto* term = new KCalXRFSourceTerm(E, Y);
    /* Do not need to manually delete the AutoDiffCostFunction pointer 
     * as it is passed to AddResidualBlock. Ceres handles this automatically 
     */
    //ceres::CostFunction* cost = new ceres::AutoDiffCostFunction<KKEDSourceTerm, 1, 10>(term);
    ceres::CostFunction* cost =
        new ceres::NumericDiffCostFunction<
            KCalXRFSourceTerm,
            ceres::CENTRAL,
            1, 13>(term);

    problem.AddResidualBlock(cost, new ceres::HuberLoss(1.0), params);// optional robust loss
  }

//  // --------------------------------------------------------------
//  // Add one calibration prior residual block
//  // --------------------------------------------------------------
//  const double offsetSigma = 0.05;
//  const double slopeSigma  = 5e-4;
//
//  ceres::CostFunction* prior_cost =
//        new ceres::AutoDiffCostFunction<KEnergyCalibrationPrior, 2, 13>(
//        new KEnergyCalibrationPrior(offsetSigma, slopeSigma));
//  
//  problem.AddResidualBlock(prior_cost, nullptr, params);

  // --------------------------------------------------------------
  // Solver options
  // --------------------------------------------------------------
  ceres::Solver::Options opts;
  opts.max_num_iterations = 100;
  opts.linear_solver_type = ceres::DENSE_QR;
  opts.minimizer_progress_to_stdout = false;
  opts.function_tolerance = 1e-6; //default 1e-6
  opts.gradient_tolerance = 1e-8; //default 1e-10

  // --------------------------------------------------------------
  // Run the optimizer
  // --------------------------------------------------------------
  ceres::Solver::Summary summary;
  ceres::Solve(opts, &problem, &summary);
  //std::cout << summary.FullReport() << "\n";

  if (summary.IsSolutionUsable()) 
  {
    KDevLogger::getInstance().logEvent("Optimization successful! Proceeding with usable solution.");
  } 
  else 
  {
    KDevLogger::getInstance().logEvent("Ceres failed to find a usable solution: " + summary.message );
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
  //double scaledCov_U_U;
  //double scaledUncertaintyU;
  //double scaledCov_Pu_Pu;
  //double scaledUncertaintyPu;

  /*
  Ceres does not automatically scale the covariance by the reduced chi-squared 
  statistic. 
  If your data points have arbitrary weighting, you must manually multiply 
  the returned covariance values by final_cost*2.0/dof
  */
  // Ceres stores 'cost' as 0.5 * sum(residual^2). We multiply by 2 to get the actual Chi-Squared value.

  std::vector<std::pair<const double*, const double*>> covarianceBlocks;
  covarianceBlocks.push_back(std::make_pair(params, params));
  double cov[169];

  if (covariance.Compute(covarianceBlocks, &problem))
  {
    //covariance.GetCovarianceBlock(params, params, cov);
    //double unscaledCov_U_U   = cov[2 * 13 + 2]; //cov[param_index * #_params + param_index]
    //double unscaledCov_Pu_Pu = cov[3 * 13 + 3];
    //scaledCov_U_U = unscaledCov_U_U * reducedChiSquared;
    //scaledUncertaintyU = std::sqrt(scaledCov_U_U);
    //scaledCov_Pu_Pu = unscaledCov_Pu_Pu * reducedChiSquared;
    //scaledUncertaintyPu = std::sqrt(scaledCov_Pu_Pu);
  }
  else 
  {
    std::cerr << "Covariance computation failed!" << std::endl;
    return false;
  }

  double C   = params[0];
  double b3  = params[1];
  double b4  = params[2];
  double b5  = params[3];
  double b6  = params[4];
  double b7  = params[5];
  double b8  = params[6];
  double b9  = params[7];
  double b10 = params[8];
  double b11 = params[9];
  double b12 = params[10];
  double b13 = params[11];
  double b14 = params[12];

  // --------------------------------------------------------------
  // Store results and emit signals
  // --------------------------------------------------------------
  std::vector<double> fittedEs, fittedYs, fitROIData, fittedContinuums, fittedCoinces;
  for (size_t i = 0; i < energies.size(); ++i) 
  {
    if (energies.at(i) >= 85 && 
        energies.at(i) <= 110)
    {
      fittedEs.push_back(energies.at(i));
      fitROIData.push_back(counts.at(i));
      //pass a step background of zero so we don't add it twice
      fittedYs.push_back(KCalXRFSourceTerm::evaluate(energies.at(i), params));
      //fittedContinuums.push_back(continuumCounts.at(i));
      //fittedCoinces.push_back(coincCounts.at(i));
    }
  }

  //save the fit to the results object
  m_calXRFResults.m_ceresResults.m_linearSolverType = ceres::LinearSolverTypeToString(summary.linear_solver_type_given);
  m_calXRFResults.m_ceresResults.m_terminationType = ceres::TerminationTypeToString(summary.termination_type);
  m_calXRFResults.m_ceresResults.m_message = summary.message;
  m_calXRFResults.m_ceresResults.m_initialCost = summary.initial_cost;
  m_calXRFResults.m_ceresResults.m_finalCost = summary.final_cost;
  m_calXRFResults.m_ceresResults.m_reducedChiSquared = reducedChiSquared;

  const auto& last = summary.iterations.back();
  m_calXRFResults.m_ceresResults.m_gradMaxNorm = last.gradient_max_norm;
  m_calXRFResults.m_ceresResults.m_relDecrease = last.relative_decrease;
  m_calXRFResults.m_ceresResults.m_stepNorm    = last.step_norm; 
  m_calXRFResults.m_ceresResults.m_stepSucess  = last.step_is_successful;
  m_calXRFResults.m_C   = C;
  m_calXRFResults.m_b3  = b3;
  m_calXRFResults.m_b4  = b4;
  m_calXRFResults.m_b5  = b5;
  m_calXRFResults.m_b6  = b6;
  m_calXRFResults.m_b7  = b7;
  m_calXRFResults.m_b8  = b8;
  m_calXRFResults.m_b9  = b9;
  m_calXRFResults.m_b10 = b10;
  m_calXRFResults.m_b11 = b11;
  m_calXRFResults.m_b12 = b12;
  m_calXRFResults.m_b13 = b13;
  m_calXRFResults.m_b14 = b14;
  m_calXRFResults.m_fitROIEnergies = fittedEs;
  m_calXRFResults.m_fitROIDataCounts = fitROIData;
  m_calXRFResults.m_fitROIModelCounts = fittedYs;
  //m_calXRFResults.m_continuumCounts = fittedContinuums;
  //m_calXRFResults.m_coincCounts = fittedCoinces;

  //send the results to the calibration worker
  emit send_XRF_cal_results(m_calXRFResults);

  return true;
}

bool KCalibrateEngine::calibrate_KED()
{
  return true;
}

 std::vector<double> KCalibrateEngine::compute_continuum(const std::vector<double>& a_counts)
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

std::vector<double> KCalibrateEngine::compute_coinc_summing(const std::vector<double>& a_counts)
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

std::vector<double> KCalibrateEngine::compute_coinc_summing_tsn(const std::vector<double>& a_counts)
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