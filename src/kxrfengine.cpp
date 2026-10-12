#include "kxrfengine.h"
#include <thread>

KXRFEngine::KXRFEngine()
{
  KDevLogger::getInstance().logEvent("KXRFEngine object constructed");
}

KXRFEngine::~KXRFEngine()
{
  KDevLogger::getInstance().logEvent("KXRFEngine object destructed");
}

void KXRFEngine::send(int a_message)
{
}

void KXRFEngine::receive(int a_message)
{
}
/*
bool KXRFEngine::fitting_analysis()
{
  // --------------------------------------------------------------
  // Load the data
  // --------------------------------------------------------------
  const std::vector<double>& energies = KFileManager::getInstance().m_xrfSampData.m_energies;
  std::vector<double>& counts   = KFileManager::getInstance().m_xrfSampData.m_originalCounts;
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
  //std::vector<double> continuumCounts = compute_continuum(counts);


  // --------------------------------------------------------------
  // Initial guesses
  // --------------------------------------------------------------
  double params[53] = {
                        100000,   // [0]  U xray 84.4 Ht
                        100000,   // [1]  U xray 84.4 Ht
                        100000,   // [2]  U xray 84.4 Ht
                        100000,   // [3]  U xray 84.4 Ht
                        100000,   // [4]  U xray 84.4 Ht
                        100000,   // [10] U xray 84.4 Ht
                        100000,   // [12] Pu xray 84.4 Ht
                        100000,   // [14] Pu xray 84.4 Ht
                        10000,   // [16] Pu xray 84.4 Ht
                        10000,   // [18] Pu xray 84.4 Ht
                        10000,   // [20] Pu xray 84.4 Ht
                        10000,   // [22] Pu xray 84.4 Ht
                        10000,   // [16] Pu xray 84.4 Ht
                        10000,   // [18] Pu xray 84.4 Ht
                        10000,   // [20] Pu xray 84.4 Ht
                        10000,   // [22] Pu xray 84.4 Ht
                        10000,   // [16] Pu xray 84.4 Ht
                        10000,   // [18] Pu xray 84.4 Ht
                        10000,   // [20] Pu xray 84.4 Ht
                        10000,   // [22] Pu xray 84.4 Ht
                        100,   // [0]  U xray 84.4 Ht
                        100,   // [1]  U xray 84.4 Ht
                        100,   // [2]  U xray 84.4 Ht
                        100,   // [3]  U xray 84.4 Ht
                        100,   // [4]  U xray 84.4 Ht
                        100,   // [10] U xray 84.4 Ht
                        100,   // [12] Pu xray 84.4 Ht
                        100,   // [14] Pu xray 84.4 Ht
                        100,   // [16] Pu xray 84.4 Ht
                        100,   // [18] Pu xray 84.4 Ht
                        100,   // [20] Pu xray 84.4 Ht
                        100,   // [22] Pu xray 84.4 Ht
                        100,   // [16] Pu xray 84.4 Ht
                        100,   // [18] Pu xray 84.4 Ht
                        100,   // [20] Pu xray 84.4 Ht
                        100,   // [22] Pu xray 84.4 Ht
                        100,   // [16] Pu xray 84.4 Ht
                        100,   // [18] Pu xray 84.4 Ht
                        100,   // [20] Pu xray 84.4 Ht
                        100,   // [22] Pu xray 84.4 Ht
                        100,   // [20] Pu xray 84.4 Ht
                        100,   // [22] Pu xray 84.4 Ht
                        100,   // [16] Pu xray 84.4 Ht
                        100,   // [18] Pu xray 84.4 Ht
                        100,   // [20] Pu xray 84.4 Ht
                        100,   // [22] Pu xray 84.4 Ht
                        100,   // [16] Pu xray 84.4 Ht
                        100,   // [18] Pu xray 84.4 Ht
                        100,   // [20] Pu xray 84.4 Ht
                        100,   // [22] Pu xray 84.4 Ht
                        100,      // [15] quad 0
                       -8700,       // [16] quad 1
                        0.0     // quad 2
                      };

  ceres::Problem problem;

  // --------------------------------------------------------------
  // Build residual blocks
  // --------------------------------------------------------------
  for (int i = 0; i < nObs; ++i) 
  {
    const double E = energies[i];
    const double Y = counts[i];
   // const double C = continuumCounts[i];

    if (E <= constants.m_XRFFitLowerBound || 
        E >= constants.m_XRFFitUpperBound) continue;

  //By default AutoDiffCostFunction will take ownership of the cost 
  //functor pointer passed to it, ie. will call delete on the cost functor
  //when the AutoDiffCostFunction itself is deleted. However, 
  //this may be undesirable in certain cases, therefore it is also possible 
  //to specify DO_NOT_TAKE_OWNERSHIP as a second argument in the constructor, 
  //while passing a pointer to a cost functor which does not need to be 
  //deleted by the AutoDiffCostFunction.
  //
    auto* term = new KXRFSourceTerm(E, Y);

    //Do not need to manually delete the AutoDiffCostFunction pointer 
    //as it is passed to AddResidualBlock. Ceres handles this automatically 
    //
    //ceres::CostFunction* cost = new ceres::AutoDiffCostFunction<KXRFSourceTerm, 1, 10>(term);
    ceres::CostFunction* cost =
        new ceres::NumericDiffCostFunction<
            KXRFSourceTerm,
            ceres::CENTRAL,
            1, 53>(term);

    problem.AddResidualBlock(cost, new ceres::HuberLoss(1.0), params);// optional robust loss
  }

//  // --------------------------------------------------------------
//  // Add one calibration prior residual block
//  // --------------------------------------------------------------
//  const double offsetSigma = 0.05;
//  const double slopeSigma  = 5e-4;
//
//  ceres::CostFunction* prior_cost =
//        new ceres::AutoDiffCostFunction<KEnergyCalibrationPrior, 2, 14>(
//        new KEnergyCalibrationPrior(offsetSigma, slopeSigma));
//  
//  problem.AddResidualBlock(prior_cost, nullptr, params);

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
  std::cout << summary.FullReport() << "\n";

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


  //
  //Ceres does not automatically scale the covariance by the reduced chi-squared 
  //statistic. 
  //If your data points have arbitrary weighting, you must manually multiply 
  //the returned covariance values by final_cost*2.0/dof
  //
  // Ceres stores 'cost' as 0.5 * sum(residual^2). We multiply by 2 to get the actual Chi-Squared value.

  std::vector<std::pair<const double*, const double*>> covarianceBlocks;
  covarianceBlocks.push_back(std::make_pair(params, params));
  double cov[2809];

  if (covariance.Compute(covarianceBlocks, &problem))
  {
    covariance.GetCovarianceBlock(params, params, cov);

    double unscaledCov_U_U   = cov[2 * 53 + 2];//cov[param_index * #_params + param_index]
    double unscaledCov_Pu_Pu = cov[3 * 53 + 3];

  }
  else 
  {
    std::cerr << "Covariance computation failed!" << std::endl;
    return false;
  }

  //double C            = params[0];
  //double chi          = params[1];


  // --------------------------------------------------------------
  // Store results and emit signals
  // --------------------------------------------------------------
  m_xrfResults.m_ceresResults.m_terminationType = ceres::TerminationTypeToString(summary.termination_type);
  m_xrfResults.m_ceresResults.m_message = summary.message;
  m_xrfResults.m_ceresResults.m_initialCost = summary.initial_cost;
  m_xrfResults.m_ceresResults.m_finalCost = summary.final_cost;
  m_xrfResults.m_ceresResults.m_costDrop = summary.initial_cost - summary.final_cost;
  m_xrfResults.m_ceresResults.m_numResidualEvals = summary.num_residual_evaluations;
  m_xrfResults.m_ceresResults.m_numJacobianEvals = summary.num_jacobian_evaluations;
  m_xrfResults.m_ceresResults.m_numSuccessfulSteps = summary.num_successful_steps;
  m_xrfResults.m_ceresResults.m_numUnsuccessfulSteps = summary.num_unsuccessful_steps;
  m_xrfResults.m_ceresResults.m_totalIterations = summary.num_successful_steps + summary.num_unsuccessful_steps;
  m_xrfResults.m_ceresResults.m_totalTime = summary.total_time_in_seconds;
  m_xrfResults.m_ceresResults.m_jacobianTime = summary.jacobian_evaluation_time_in_seconds;
  m_xrfResults.m_ceresResults.m_linearSolverTime = summary.linear_solver_time_in_seconds;
  m_xrfResults.m_ceresResults.m_linearSolverType = ceres::LinearSolverTypeToString(summary.linear_solver_type_given);

  m_xrfResults.m_initialCost = summary.initial_cost;
  m_xrfResults.m_finalCost = summary.final_cost;
  m_xrfResults.m_numResiduals = summary.num_residuals;
  m_xrfResults.m_numParams = 5;
  m_xrfResults.m_converged = summary.IsSolutionUsable();


  //std::cout << "cd " << cdFilterDx <<std::endl;
  //std::cout << "red chi sq " << reducedChiSquared <<std::endl; 
  std::vector<double> fittedEs, fittedYs, fitROIData, fittedContinuums, fittedCoinces;
  for (size_t i = 0; i < energies.size(); ++i) 
  {
    if (energies.at(i) > constants.m_XRFFitLowerBound && 
        energies.at(i) < constants.m_XRFFitUpperBound)
    {
      fittedEs.push_back(energies.at(i));
      fitROIData.push_back(counts.at(i));
      //pass a step background of zero so we don't add it twice
      fittedYs.push_back(KXRFSourceTerm::evaluate(energies.at(i), params));
      //fittedContinuums.push_back(continuumCounts.at(i));
      fittedCoinces.push_back(coincCounts.at(i));
    }
  }
 
  //save the fit to the results object
  m_xrfResults.m_fitROIEnergies = fittedEs;
  m_xrfResults.m_fitROIDataCounts = fitROIData;
  m_xrfResults.m_fitROIModelCounts = fittedYs;
  m_xrfResults.m_continuumCounts = fittedContinuums;
  m_xrfResults.m_coincCounts = fittedCoinces;

  //send the results to the analysisworker
  emit send_XRF_results(m_xrfResults);

  return true;
}
*/


bool KXRFEngine::fitting_analysis()
{
  const std::vector<double>& energies = KFileManager::getInstance().m_xrfSampData.m_energies;
  const std::vector<double>& counts = KFileManager::getInstance().m_xrfSampData.m_originalCounts;
  const parameter_data::KParameterData& constants = KFileManager::getInstance().m_workingParameterSet;

  if (energies.size() != counts.size() || energies.empty())
  {
    std::cerr << "XRF fit failed: invalid spectrum data.\n";
    return false;
  }

  // p[0..9]   U
  // p[10..19] Pu
  // p[20..29] Np (optional)
  // p[30..39] Am
  // p[40..49] Cm (optional)
  // p[50..51] continuum
  double params[m_numParams] = {};

  std::vector<std::size_t> roiIndices;
  roiIndices.reserve(energies.size());
  for (std::size_t i = 0; i < energies.size(); ++i)
  {
    if (energies[i] > constants.m_XRFFitLowerBound &&
        energies[i] < constants.m_XRFFitUpperBound)
    {
      roiIndices.push_back(i);
    }
  }

  if (roiIndices.size() < 10)
  {
    std::cerr << "XRF fit failed: too few channels in the fit ROI.\n";
    return false;
  }



  params[50] = 2;
  //params[50] = 200000; //A1
  //params[51] = 0.20;   //lambda1
  //params[52] = 70000;  //A1
  //params[53] = 0.05;   //lambda2
  //params[54] = 1200;   //C

  // Initialize only the elements that are always part of the normal model.
  initialize_element_params(params, 0, 10, energies, counts,
                            KAtomicData::m_uKalpha1_energy);
  initialize_element_params(params, 10, 20, energies, counts,
                            KAtomicData::m_puKalpha1_energy);
  initialize_element_params(params, 30, 40, energies, counts,
                            KAtomicData::m_amKalpha1_energy);

  // Base model: U + Pu + Am + continuum. Np and Cm are truly inactive in
  // the optimizer tangent space unless residual evidence supports them.
  std::vector<int> fixedIndices;
  fixedIndices.reserve(20);
  for (int i = 20; i < 30; ++i) fixedIndices.push_back(i); // Np
  for (int i = 40; i < 50; ++i) fixedIndices.push_back(i); // Cm

  ceres::Problem problem;
  build_problem(problem, params, energies, counts, constants, fixedIndices);

  std::cout << "\n=== Base fit: U + Pu + Am ===\n";
  if (!solve_stage(problem, "Base U/Pu/Am"))
  {
    return false;
  }

  // Matched-template significance for an optional element. The template uses
  // all ten known lines, not one channel at K-alpha-1. This is much less
  // sensitive to noise, overlap, and small calibration offsets.
  const auto optionalElementScore = [&](int startIndex) -> double
  {
    static constexpr double relativeIntensity[10] = {
        1.00, 0.60, 0.10, 0.30, 0.15,
        0.10, 0.05, 0.05, 0.03, 0.03};

    double templateParams[m_numParams] = {};
    for (int line = 0; line < 10; ++line)
    {
      templateParams[startIndex + line] = relativeIntensity[line];
    }

    double numerator = 0.0;
    double denominator2 = 0.0;

    for (const std::size_t i : roiIndices)
    {
      const double sigma = std::sqrt(std::max(counts[i], 1.0));
      const double residual = (counts[i] - KXRFSourceTerm::evaluate(energies[i], params)) / sigma;
      const double unitTemplate = KXRFSourceTerm::evaluate(energies[i], templateParams) / sigma;

      numerator += residual * unitTemplate;
      denominator2 += unitTemplate * unitTemplate;
    }

    if (denominator2 <= 1.0e-20)
    {
      return 0.0;
    }
    return numerator / std::sqrt(denominator2);
  };

  const double npScore = optionalElementScore(20);
  const double cmScore = optionalElementScore(40);

  // Four sigma is deliberately conservative because an unnecessary optional
  // element adds ten correlated amplitudes to the fit.
  constexpr double detectionThreshold = 4.0;
  const bool npPresent = npScore >= detectionThreshold;
  const bool cmPresent = cmScore >= detectionThreshold;

  std::cout << "Optional-element scores: Np = " << npScore
            << " sigma, Cm = " << cmScore << " sigma\n";

  if (npPresent)
  {
    initialize_element_params(params, 20, 30, energies, counts,
                              KAtomicData::m_npKalpha1_energy);
  }
  if (cmPresent)
  {
    initialize_element_params(params, 40, 50, energies, counts,
                              KAtomicData::m_cmKalpha1_energy);
  }

  // Only perform a second solve when at least one optional element is found.
  if (npPresent || cmPresent)
  {
    fixedIndices.clear();
    if (!npPresent)
    {
      for (int i = 20; i < 30; ++i) fixedIndices.push_back(i);
    }
    if (!cmPresent)
    {
      for (int i = 40; i < 50; ++i) fixedIndices.push_back(i);
    }

    problem.SetManifold(params, new ceres::SubsetManifold(m_numParams, fixedIndices));

    std::cout << "\n=== Optional-element refinement:"
              << (npPresent ? " Np" : "")
              << (cmPresent ? " Cm" : "") << " ===\n";

    if (!solve_stage(problem, "Optional refinement"))
    {
      return false;
    }
  }

  const auto elementIsActive = [&params](int beginIndex)
  {
    double total = 0.0;
    for (int i = beginIndex; i < beginIndex + 10; ++i)
    {
      total += params[i];
    }
    return total >= 1.0;
  };

  m_elementStatus.u_active = elementIsActive(0);
  m_elementStatus.pu_active = elementIsActive(10);
  m_elementStatus.np_active = npPresent && elementIsActive(20);
  m_elementStatus.am_active = elementIsActive(30);
  m_elementStatus.cm_active = cmPresent && elementIsActive(40);

  compute_final_statistics(params, energies, counts, constants);

  const std::vector<double> coincCounts = compute_coinc_summing_tsn(counts);

  std::vector<double> fittedEs;
  std::vector<double> fittedYs;
  std::vector<double> fitROIData;
  std::vector<double> fittedCoinces;
  fittedEs.reserve(roiIndices.size());
  fittedYs.reserve(roiIndices.size());
  fitROIData.reserve(roiIndices.size());
  fittedCoinces.reserve(roiIndices.size());

  for (const std::size_t i : roiIndices)
  {
    fittedEs.push_back(energies[i]);
    fitROIData.push_back(counts[i]);
    fittedYs.push_back(KXRFSourceTerm::evaluate(energies[i], params));
    fittedCoinces.push_back(coincCounts[i]);
  }

  m_xrfResults.m_fitROIEnergies = std::move(fittedEs);
  m_xrfResults.m_fitROIDataCounts = std::move(fitROIData);
  m_xrfResults.m_fitROIModelCounts = std::move(fittedYs);
  m_xrfResults.m_continuumCounts.clear();
  m_xrfResults.m_coincCounts = std::move(fittedCoinces);

  emit send_XRF_results(m_xrfResults);
  return true;
}

// --------------------------------------------------------------
// Helper Functions
// --------------------------------------------------------------

void KXRFEngine::initialize_element_params(double* a_params,
                                           int a_startIndex,
                                           int a_endIndex,
                                           const std::vector<double>& a_energies,
                                           const std::vector<double>& counts,
                                           double a_primaryPeakEnergy)
{
  if (a_params == nullptr || a_energies.size() != counts.size() ||
      a_startIndex < 0 || a_endIndex > 50 || a_startIndex >= a_endIndex)
  {
    return;
  }

  auto peakIt = std::lower_bound(a_energies.begin(), a_energies.end(),
                                 a_primaryPeakEnergy - 1.0);
  if (peakIt == a_energies.end())
  {
    return;
  }

  const std::ptrdiff_t peakIndex = std::distance(a_energies.begin(), peakIt);
  const std::ptrdiff_t first = std::max<std::ptrdiff_t>(0, peakIndex - 8);
  const std::ptrdiff_t last = std::min<std::ptrdiff_t>(
      static_cast<std::ptrdiff_t>(counts.size()), peakIndex + 9);

  double maxExcess = 0.0;
  for (std::ptrdiff_t i = first; i < last; ++i)
  {
    const double prediction = KXRFSourceTerm::evaluate(a_energies[static_cast<std::size_t>(i)], a_params);
    maxExcess = std::max(maxExcess, counts[static_cast<std::size_t>(i)] - prediction);
  }

  // Avoid forcing a nonexistent element to start with a large amplitude.
  const double baseAmplitude = std::max(0.0, maxExcess);
  static constexpr double relativeIntensity[10] = {
      1.00, 0.60, 0.10, 0.30, 0.15,
      0.10, 0.05, 0.05, 0.03, 0.03};

  const int numberToInitialize = std::min(10, a_endIndex - a_startIndex);
  for (int line = 0; line < numberToInitialize; ++line)
  {
    a_params[a_startIndex + line] = baseAmplitude * relativeIntensity[line];
  }
}

bool KXRFEngine::is_element_present(const std::vector<double>& a_energies,
                                    const std::vector<double>& a_counts,
                                    const double* a_params,
                                    double a_primaryPeakEnergy)
{
  if (a_params == nullptr || a_energies.size() != a_counts.size() || a_energies.empty())
  {
    return false;
  }

  const auto peakIt = std::lower_bound(a_energies.begin(), a_energies.end(),
                                       a_primaryPeakEnergy);
  if (peakIt == a_energies.end())
  {
    return false;
  }

  const std::size_t peakIndex = static_cast<std::size_t>(std::distance(a_energies.begin(), peakIt));
  const double model = KXRFSourceTerm::evaluate(a_energies[peakIndex], a_params);
  const double sigma = std::sqrt(std::max(a_counts[peakIndex], 1.0));
  return ((a_counts[peakIndex] - model) / sigma) > 3.0;
}

void KXRFEngine::build_problem(ceres::Problem& a_problem,
                               double* a_params,
                               const std::vector<double>& a_energies,
                               const std::vector<double>& a_counts,
                               const parameter_data::KParameterData& a_constants,
                               const std::vector<int>& a_fixedIndices)
{
  // One shared robust-loss object is sufficient for every residual block.
  auto* loss = new ceres::HuberLoss(1.0);

  for (std::size_t i = 0; i < a_energies.size(); ++i)
  {
    const double energy = a_energies[i];
    if (energy <= a_constants.m_XRFFitLowerBound || energy >= a_constants.m_XRFFitUpperBound)
    {
      continue;
    }

    auto* cost = new ceres::AutoDiffCostFunction<KXRFSourceTerm, 1, m_numParams>(
        new KXRFSourceTerm(energy, a_counts[i]));
    a_problem.AddResidualBlock(cost, loss, a_params);
  }

  // Peak amplitudes are physical count amplitudes and cannot be negative.
  for (int i = 0; i < 50; ++i)
  {
    a_problem.SetParameterLowerBound(a_params, i, 0.0);
  }

  a_problem.SetManifold(a_params, new ceres::SubsetManifold(m_numParams, a_fixedIndices));
}

bool KXRFEngine::solve_stage(ceres::Problem& a_problem,
                             const std::string& a_stageName)
{
  ceres::Solver::Options options;
  options.max_num_iterations = (a_stageName.find("Optional") != std::string::npos) ? 20 : 45;
  options.linear_solver_type = ceres::DENSE_QR;
  options.minimizer_progress_to_stdout = false;
  options.function_tolerance = 1.0e-6;
  options.gradient_tolerance = 1.0e-8;
  options.parameter_tolerance = 1.0e-8;
  options.num_threads = std::max(1u, std::thread::hardware_concurrency());

  ceres::Solver::Summary summary;
  ceres::Solve(options, &a_problem, &summary);

  std::cout << a_stageName << " fit: "
            << ceres::TerminationTypeToString(summary.termination_type)
            << " | cost " << summary.initial_cost
            << " -> " << summary.final_cost
            << " | successful steps " << summary.num_successful_steps
            << " | time " << summary.total_time_in_seconds << " s\n";

  if (!summary.IsSolutionUsable())
  {
    std::cerr << a_stageName << " failed: " << summary.message << '\n';
    return false;
  }

  return true;
}

void KXRFEngine::compute_final_statistics(double* a_params,
                                          const std::vector<double>& a_energies,
                                          const std::vector<double>& a_counts,
                                          const parameter_data::KParameterData& a_constants)
{
  std::cout << "\n=== Computing Final Statistics ===\n";
  
  // --------------------------------------------------------------
  // Identify which parameters should be fixed (near zero)
  // --------------------------------------------------------------
  std::vector<int> fixed_indices;
  for (int i = 0; i < 50; ++i) {
    if (std::abs(a_params[i]) < 1.0) {
      fixed_indices.push_back(i);
    }
  }
  
  // --------------------------------------------------------------
  // Build final problem for covariance computation
  // --------------------------------------------------------------
  ceres::Problem final_problem;
  
  for (int i = 0; i < a_energies.size(); ++i) {
    const double E = a_energies[i];
    const double Y = a_counts[i];
    
    if (E <= a_constants.m_XRFFitLowerBound || 
        E >= a_constants.m_XRFFitUpperBound) continue;
    
    auto* term = new KXRFSourceTerm(E, Y);
    //ceres::CostFunction* cost =
    //    new ceres::NumericDiffCostFunction<KXRFSourceTerm, ceres::CENTRAL, 1, m_numParams>(term);
    ceres::CostFunction* cost = new ceres::AutoDiffCostFunction<KXRFSourceTerm, 1, m_numParams>(term);
    final_problem.AddResidualBlock(cost, new ceres::HuberLoss(1.0), a_params);
  }
  
  // Apply bounds
  for (int i = 0; i < 50; ++i) {
    final_problem.SetParameterLowerBound(a_params, i, 0.0);
  }
  
  // Fix parameters that are essentially zero using SubsetManifold
  if (!fixed_indices.empty()) {
    ceres::SubsetManifold* subset_manifold = 
        new ceres::SubsetManifold(m_numParams, fixed_indices);
    final_problem.SetManifold(a_params, subset_manifold);
  }
  
  // --------------------------------------------------------------
  // Compute goodness of fit statistics
  // --------------------------------------------------------------
  double total_chi_squared = 0.0;
  int num_data_points = 0;
  
  std::vector<double> residuals;
  std::vector<double> fitROIEnergies;
  std::vector<double> fitROIData;
  std::vector<double> fitROIModel;
  
  for (int i = 0; i < a_energies.size(); ++i) {
    const double E = a_energies[i];
    const double Y = a_counts[i];
    
    if (E <= a_constants.m_XRFFitLowerBound || 
        E >= a_constants.m_XRFFitUpperBound) continue;
    
    double model_value = KXRFSourceTerm::evaluate(E, a_params);
    double sigma = std::sqrt(std::max(Y, 1.0));
    double residual = (Y - model_value) / sigma;
    
    total_chi_squared += residual * residual;
    residuals.push_back(residual);
    num_data_points++;
    
    // Store for plotting
    fitROIEnergies.push_back(E);
    fitROIData.push_back(Y);
    fitROIModel.push_back(model_value);
  }
  
  // Count active (non-fixed) parameters
  int num_active_params = m_numParams - fixed_indices.size();
  
  int dof = num_data_points - num_active_params;
  double reduced_chi_squared = (dof > 0) ? (total_chi_squared / dof) : 0.0;
  
  std::cout << "Data points: " << num_data_points << "\n";
  std::cout << "Active parameters: " << num_active_params << "\n";
  std::cout << "Fixed parameters: " << fixed_indices.size() << "\n";
  std::cout << "Chi-squared: " << total_chi_squared << "\n";
  std::cout << "Degrees of freedom: " << dof << "\n";
  std::cout << "Reduced chi-squared: " << reduced_chi_squared << "\n";
  
  // --------------------------------------------------------------
  // Compute covariance matrix
  // --------------------------------------------------------------
  ceres::Covariance::Options cov_options;
  cov_options.algorithm_type = ceres::DENSE_SVD;
  cov_options.null_space_rank = -1;  // Auto-detect
  cov_options.min_reciprocal_condition_number = 1e-14;
  
  ceres::Covariance covariance(cov_options);
  
  std::vector<std::pair<const double*, const double*>> covariance_blocks;
  covariance_blocks.push_back(std::make_pair(a_params, a_params));
  
  double cov[m_numParams * m_numParams];
  std::fill(cov, cov + m_numParams*m_numParams, 0.0);
  
  bool cov_success = false;
  if (covariance.Compute(covariance_blocks, &final_problem)) 
  {
    covariance.GetCovarianceBlock(a_params, a_params, cov);
    cov_success = true;
    std::cout << "Covariance computation successful\n";
  } 
  else 
  {
    std::cerr << "WARNING: Covariance computation failed!\n";
    std::cerr << "This may indicate rank deficiency or numerical issues.\n";
  }
  
  // --------------------------------------------------------------
  // Extract parameter uncertainties
  // --------------------------------------------------------------
  std::vector<double> param_uncertainties(m_numParams, 0.0);
  
  if (cov_success && reduced_chi_squared > 0) 
  {
    for (int i = 0; i < m_numParams; ++i) 
    {
      double variance = cov[i * m_numParams + i];
      if (variance > 0) 
      {
        // Scale by reduced chi-squared
        param_uncertainties[i] = std::sqrt(variance * reduced_chi_squared);
      } 
      else if (variance < 0) 
      {
        std::cerr << "WARNING: Negative variance for parameter " << i << "\n";
      }
    }
  }
  
  // --------------------------------------------------------------
  // Print key results
  // --------------------------------------------------------------
  std::cout << "\n=== Fitted Parameters and Uncertainties ===\n";
  
  const char* element_names[] = {"U", "Pu", "Np", "Am", "Cm"};
  const char* line_names[] = {"Kα1", "Kα2", "Kα3", "Kβ1", "Kβ2", 
                              "Kβ3", "Kβ4", "Kβ5", "KO23", "KP23"};
  
  for (int elem = 0; elem < 5; ++elem) {
    bool element_present = false;
    for (int line = 0; line < 10; ++line) 
    {
      int idx = elem * 10 + line;
      if (std::abs(a_params[idx]) >= 1.0) 
      {
        element_present = true;
        break;
      }
    }
    
    if (element_present) {
      std::cout << "\n" << element_names[elem] << " X-rays:\n";
      for (int line = 0; line < 10; ++line) 
      {
        int idx = elem * 10 + line;
        if (std::abs(a_params[idx]) >= 1.0) 
        {
          double relative_unc = (a_params[idx] > 0) ? 
                                (param_uncertainties[idx] / a_params[idx] * 100.0) : 0.0;
          std::cout << "  " << line_names[line] << ": " 
                    << std::fixed << std::setprecision(1)
                    << a_params[idx] << " ± " << param_uncertainties[idx] 
                    << " (" << relative_unc << "%)\n";
        }
      }
    }
  }
  
  std::cout << "\nContinuum:\n";
  std::cout << "  Scaler:  " << std::scientific << a_params[50] 
            << " +/- " << param_uncertainties[50] << std::endl;
  //std::cout << "  eb1    " << a_params[51] 
  //          << " +/- " << param_uncertainties[51] << "\n";
  
  // --------------------------------------------------------------
  // Compute element totals with propagated uncertainties
  // --------------------------------------------------------------
  std::cout << "\n=== Total X-ray Intensities by Element ===\n";
  
  for (int elem = 0; elem < 5; ++elem) 
  {
    double total_intensity = 0.0;
    double total_variance = 0.0;
    bool element_present = false;
    
    std::vector<int> active_lines;
    
    for (int line = 0; line < 10; ++line) 
    {
      int idx = elem * 10 + line;
      if (std::abs(a_params[idx]) >= 1.0) 
      {
        element_present = true;
        active_lines.push_back(idx);
        total_intensity += a_params[idx];
        
        // Add variance
        total_variance += param_uncertainties[idx] * param_uncertainties[idx];
      }
    }
    
    // Add covariances between lines of same element
    if (cov_success) 
    {
      for (size_t i = 0; i < active_lines.size(); ++i) 
      {
        for (size_t j = i + 1; j < active_lines.size(); ++j) 
        {
          int idx1 = active_lines[i];
          int idx2 = active_lines[j];
          total_variance += 2.0 * cov[idx1 * m_numParams + idx2] * reduced_chi_squared;
        }
      }
    }
    
    if (element_present) 
    {
      double total_uncertainty = std::sqrt(std::max(total_variance, 0.0));
      double relative_uncertainty = (total_intensity > 0) ? 
                                    (total_uncertainty / total_intensity * 100.0) : 0.0;
      
      std::cout << std::fixed << std::setprecision(1)
                << element_names[elem] << ": " 
                << total_intensity << " ± " << total_uncertainty 
                << " (" << relative_uncertainty << "%)\n";
    }
  }
  
  // --------------------------------------------------------------
  // Compute residual statistics
  // --------------------------------------------------------------
  double meanResidual = 0.0;
  for (double r : residuals) 
  {
    meanResidual += r;
  }
  meanResidual /= residuals.size();
  
  double stdResidual = 0.0;
  for (double r : residuals) 
  {
    stdResidual += (r - meanResidual) * (r - meanResidual);
  }
  stdResidual = std::sqrt(stdResidual / (residuals.size() - 1));
  
  double maxResidual = 0.0;
  int maxResidualIndex = 0;
  for (int i = 0; i < residuals.size(); ++i) 
  {
    if (std::abs(residuals[i]) > std::abs(maxResidual)) 
    {
      maxResidual = residuals[i];
      maxResidualIndex = i;
    }
  }
  
  std::cout << "\n=== Residual Statistics ===\n";
  std::cout << std::setprecision(3)
            << "Mean residual: " << meanResidual << " σ\n";
  std::cout << "Std dev of residuals: " << stdResidual << " σ\n";
  std::cout << "Max residual: " << maxResidual << " σ at E = " 
            << fitROIEnergies[maxResidualIndex] << " keV\n";
  
  // Check for systematic issues
  if (std::abs(meanResidual) > 0.5) 
  {
    std::cout << "WARNING: Large mean residual suggests systematic bias!\n";
  }
  if (stdResidual > 1.5) {
    std::cout << "WARNING: Large residual scatter - model may be inadequate!\n";
  }
  
  // --------------------------------------------------------------
  // Store results in member variable
  // --------------------------------------------------------------
  m_xrfResults.m_finalCost = total_chi_squared / 2.0;
  m_xrfResults.m_numResiduals = num_data_points;
  m_xrfResults.m_numParams = num_active_params;
  m_xrfResults.m_converged = (reduced_chi_squared > 0.5 && reduced_chi_squared < 2.0);
  
  m_xrfResults.m_fitROIEnergies = fitROIEnergies;
  m_xrfResults.m_fitROIDataCounts = fitROIData;
  m_xrfResults.m_fitROIModelCounts = fitROIModel;
  
  std::cout << "\n=== Statistics Computation Complete ===\n";
}

 std::vector<double> KXRFEngine::compute_continuum(const std::vector<double>& a_counts)
{
  std::vector<double> continuumCounts;
  const std::size_t numChannels = a_counts.size();
  const parameter_data::KParameterData& constants =
        KFileManager::getInstance().m_workingParameterSet;

  //using SpecUtils method to integrate low E bkgd region
  const double lowECountSum = KFileManager::getInstance().m_xrfSampData.m_measurement->gamma_integral(constants.m_XRFBkgdLowLowBound, constants.m_XRFBkgdLowHighBound);
  //FRY: needs to be updated to account for fractional bins FRY//
  const int lowEChannelSum = 
            KFileManager::getInstance().m_xrfSampData.m_measurement->find_gamma_channel(constants.m_XRFBkgdLowHighBound) -
            KFileManager::getInstance().m_xrfSampData.m_measurement->find_gamma_channel(constants.m_XRFBkgdLowLowBound);
  
  double lowECountAvg, highECountAvg;
  //make sure num channels is not zero or negative. If it is, use the bin value
  //at right side of bkgd region
  if(lowEChannelSum > 0)
  {
    lowECountAvg = lowECountSum/lowEChannelSum;
  }
  else
  {
    int bin = KFileManager::getInstance().m_xrfSampData.m_measurement->find_gamma_channel(constants.m_XRFBkgdLowHighBound);
    lowECountAvg = KFileManager::getInstance().m_xrfSampData.m_originalCounts.at(bin);
  }

  //using SpecUtils method to integrate high E bkgd region
  const double highECountSum = KFileManager::getInstance().m_xrfSampData.m_measurement->gamma_integral(constants.m_XRFBkgdHighLowBound, constants.m_XRFBkgdHighHighBound);

  //FRY: needs to be updated to account for fractional bins FRY//
  const int highEChannelSum = 
            KFileManager::getInstance().m_xrfSampData.m_measurement->find_gamma_channel(constants.m_XRFBkgdHighHighBound) -
            KFileManager::getInstance().m_xrfSampData.m_measurement->find_gamma_channel(constants.m_XRFBkgdHighLowBound);
  
  //make sure num channels is not zero or negative. If it is, use the bin value
  //at right side of bkgd region
  if(highEChannelSum > 0)
  {
    highECountAvg = highECountSum/highEChannelSum;
  }
  else
  {
    int bin = KFileManager::getInstance().m_xrfSampData.m_measurement->find_gamma_channel(constants.m_XRFBkgdHighHighBound);
    highECountAvg = KFileManager::getInstance().m_xrfSampData.m_originalCounts.at(bin);
  }

  //using SpecUtils method to integrate ROI denominator region
  const double roiCountSum = KFileManager::getInstance().m_xrfSampData.m_measurement->gamma_integral(constants.m_XRFBkgdLowHighBound, constants.m_XRFBkgdHighLowBound);

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

std::vector<double> KXRFEngine::compute_coinc_summing(const std::vector<double>& a_counts)
{
  std::vector<double> coincCounts;
  const double liveTime = KFileManager::getInstance().m_xrfSampData.m_measurement->live_time();
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

std::vector<double> KXRFEngine::compute_coinc_summing_tsn(const std::vector<double>& a_counts)
{
  std::vector<double> coincCounts;
  const double liveTime = KFileManager::getInstance().m_xrfSampData.m_measurement->live_time();
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