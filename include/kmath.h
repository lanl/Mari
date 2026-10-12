/*
© 2026. Triad National Security, LLC. All rights reserved.

This program was produced under U.S. Government contract 89233218CNA000001 for 
Los Alamos National Laboratory (LANL), which is operated by Triad National 
Security, LLC for the U.S. Department of Energy/National Nuclear Security 
Administration. 
All rights in the program are reserved by Triad National Security, LLC, 
and the U.S. Department of Energy/National Nuclear Security Administration. 
The Government is granted for itself and others acting on its behalf a 
nonexclusive, paid-up, irrevocable worldwide license in this material to 
reproduce, prepare. derivative works, distribute copies to the public, 
perform publicly and display publicly, and to permit others to do so.

                          (End of Notice)
*/

#ifndef KMATH_H
#define KMATH_H

//Project includes
#include "kdevlogger.h"
//C++ includes
#include <vector>
#include <algorithm>
#include <utility> 
#include <stdexcept>
//External includes
#include "ceres/jet.h"

/**
 * Class to store useful math stuff
 **/
class KMath 
{

public:
  KMath();
  ~KMath();

  /**
   * Gets max bin value (counts) of all spectra to be plotted
   * to control vertical scale of plot.
   * Uses recursive variadic templates
   **/
  template <typename T>
  static T maxCounts(const std::vector<T>& v) 
  {
    return* std::max_element(v.begin(), v.end());
  }

  template <typename T, typename... Vecs>
  static T maxCounts(const std::vector<T>& first, const Vecs&... rest) 
  {
    T first_max = maxCounts(first);          // max of the first vector
    T rest_max  = std::max({maxCounts(rest)...});  // max of the rest
    return std::max(first_max, rest_max);         // overall max
  }

  /**
   * Gets max channel number of all spectra to be plotted
   * to control horizontal scale of plot.
   * Uses recursive variadic templates
   **/
  template <typename T>
  static T maxChannels(const T bin) 
  {
    return bin;
  }

  template <typename T, typename... Channels>
  static T maxChannels(const T first, const Channels... rest) 
  {
    return std::max(first, maxChannels(rest...));
  }

  /**
   * Performs linear interpolation of (x,y) data
   * Data must already be sorted in acending order.
   * If it is not, call sort_data() first
   **/
//  static double linear_interpolation(const std::vector<std::pair<double, double>>& a_data, 
//                              double a_xInterp);

  template <typename T>
  inline static T linear_interpolation(const std::vector<std::pair<double, double>>& a_data, double a_xInterp)
  {
    if (a_data.empty()) 
    {
      throw std::runtime_error("Data vector cannot be empty.");
    }
  
    // Find the first point with an x-value greater than or equal to a_xInterp
    auto it = std::lower_bound(a_data.begin(), a_data.end(), a_xInterp,
                                      [](const std::pair<double, double>& p, double x) 
                                         {return p.first < x;});
  
    if (a_xInterp <= a_data.front().first)
    {
      return T(a_data.front().second);
    }
          
    if (a_xInterp >= a_data.back().first)
    {
      return T(a_data.back().second);
    }
  
    // Points P0(x0, y0) and P1(x1, y1) for interpolation
    const auto& p1 = *it;
    const auto& p0 = *(--it); // The point before p1
  
    double x0 = p0.first;
    double y0 = p0.second;
    double x1 = p1.first;
    double y1 = p1.second;
  
    // Check for division by zero (if x0 and x1 are the same)
    if (x1 == x0)
    {
      return T(y0);
    }
  
    // Linear interpolation formula: 
    //y = y0 + (y1 - y0) * (x - x0) / (x1 - x0)
    T yInterp = T(y0) + (T(y1) - T(y0)) * (T(a_xInterp) - T(x0)) / (T(x1) - T(x0));
  
    return yInterp;
  }

  /**
   * Log-log interpolation. Assumes power-law behavior -> matches attenuation data better
   **/
  template <typename T>
  inline static T loglog_linear_interpolation(const std::vector<std::pair<double, double>>& a_data, double a_xInterp)
  {
    if (a_data.empty()) 
    {
      throw std::runtime_error("Data vector cannot be empty.");
    }
  
    // Find the first point with an x-value greater than or equal to a_xInterp
    auto it = std::lower_bound(a_data.begin(), a_data.end(), a_xInterp,
                                      [](const std::pair<double, double>& p, double x) 
                                         {return p.first < x;});
  
    if (a_xInterp <= a_data.front().first)
    {
      return T(a_data.front().second);
    }
          
    if (a_xInterp >= a_data.back().first)
    {
      return T(a_data.back().second);
    }
  
    // Points P0(x0, y0) and P1(x1, y1) for interpolation
    const auto& p1 = *it;
    const auto& p0 = *(--it); // The point before p1
  
    double x0 = std::log(p0.first);
    double y0 = std::log(p0.second);
    double x1 = std::log(p1.first);
    double y1 = std::log(p1.second);
  
    // Check for division by zero (if x0 and x1 are the same)
    if (x1 == x0)
    {
      return T(y0);
    }
  
    // Linear interpolation formula: 
    //y = y0 + (y1 - y0) * (x - x0) / (x1 - x0)
    T yInterp = T(y0) + (T(y1) - T(y0)) * (T(std::log(a_xInterp)) - T(x0)) / (T(x1) - T(x0));
  
    return std::exp(yInterp);
  }

  /**
   * Sort (x,y) data
   **/
  static std::vector<std::pair<double, double>> sort_data(std::vector<std::pair<double, double>> a_data);

  /**
   * Do Lorentzian step function smoothing
   **/
  static std::vector<std::pair<double, double>> lorentz_broadening_step(std::vector<std::pair<double, double>>& a_data,
                                                                        const double a_stepLocation,
                                                                        const double a_width);

  /**
   * Pre-broaden a tabulated attenuation curve by replacing each tabulated value 
   * with a local Gaussian-weighted average of nearby tabulated values.
   * A1-A3 are detector FWHM calibration constants.
   * */
  static std::vector<std::pair<double, double>> gaussian_broaden_table(const std::vector<std::pair<double, double>>& table,
                                                                       double A1,
                                                                       double A2,
                                                                       double A3,
                                                                       double n_sigma = 5.0);

  /**
   * Calc FWHM based on calibration constants
   **/
  static double calc_fwhm(double a_energy, double a_A1, double a_A2, double a_A3);

  static std::vector<std::pair<double, double>> empirical_mu_adjust(const std::vector<std::pair<double, double>>& a_table,
                                                                    const double a_l0,
                                                                    const double a_l1,
                                                                    const double a_l2,
                                                                    const double a_l3,
                                                                    const double a_h1,
                                                                    const double a_h2,
                                                                    const double a_h3,
                                                                    const double a_kedge);

  /**
   * Calculate the normalized residuals from the fit.
   * res_i = (data_i - model_i)/sqrt(model_i)
   **/
  template<typename T>
  static std::vector<double> calculate_normalized_residuals(
                                                 const T& a_results,
                                                 std::vector<double>& residualX)
  {
    std::vector<double> residualY;
    residualX.clear();
  
    const auto& energies = a_results.m_fitROIEnergies;
    const auto& observedCounts = a_results.m_fitROIDataCounts;
    const auto& modelCounts = a_results.m_fitROIModelCounts;
  
    if (energies.size() != observedCounts.size() ||
        energies.size() != modelCounts.size() ||
        energies.empty())
    {
      return residualY;
    }
  
    residualX.reserve(energies.size());
    residualY.reserve(energies.size());
  
    constexpr double minSigma = 1.0;
  
    for (std::size_t i = 0; i < energies.size(); ++i)
    {
      const double x = energies[i];
      const double observed = observedCounts[i];
      const double expected = modelCounts[i];

      if (!std::isfinite(x) ||
          !std::isfinite(observed) ||
          !std::isfinite(expected) ||
          expected <= 0.0)
      {
        continue;
      }

      const double sigma = std::max(std::sqrt(expected), minSigma);
  
      residualX.push_back(x);
      residualY.push_back((observed - expected) / sigma);
    }
  
    return residualY;
  }

};



#endif