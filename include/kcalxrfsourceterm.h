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

#ifndef KCALXRFSOURCETERM_H
#define KCALXRFSOURCETERM_H

// Project includes
#include "kmath.h"
#include "kfilemanager.h"
#include "kparameterdata.h"
#include "kfwhmcalibration.h"
// Fix for Ceres-Windows compatibility issues
#include <cmath>
#include <array>
#include <vector>
#include <utility>
#include <algorithm>
#ifdef isinf
#undef isinf
#endif
#ifdef isnan
#undef isnan
#endif
#include "ceres/ceres.h"
#include "ceres/jet.h"



/**---------------------------------------------------------------------------
 *  XRF source term - physics model for the XRF instrument calibration source term.
 * --------------------------------------------------------------------------**/
class KCalXRFSourceTerm
{
public:
  explicit KCalXRFSourceTerm(double a_energy_keV, 
                             double a_counts);

  ~KCalXRFSourceTerm();

  static double evaluate(double E_keV,
                         const double* const p)
  {
    return model_impl<double>(double(E_keV), p);
  }

  template <typename T>
  bool operator()(const T* const p,
                  T* residual) const
  {
    const T model = model_impl<T>(T(x_), p);
    const double sigma = std::sqrt(std::max(y_, 1.0));
    residual[0] = (T(y_) - model) / T(sigma);
    return true;
  }

private:



  // -----------------------------------------------------------------------
  //  Scalar extraction helpers
  // -----------------------------------------------------------------------
  static double scalar_value(double x)
  {
    return x;
  }

  template <typename Scalar, int N>
  static double scalar_value(const ceres::Jet<Scalar, N>& x)
  {
    return static_cast<double>(x.a);
  }

  // -----------------------------------------------------------------------
  //  Table-domain helpers
  // -----------------------------------------------------------------------
  static bool is_within_table_domain(const std::vector<std::pair<double, double>>& table,
                                  double x)
  {
    if (table.empty())
    {
      return false;
    }
    return (x >= table.front().first) && (x <= table.back().first);
  }


  //FRY unhardcode energies
  // -----------------------------------------------------------------------
  //  Common valid energy domain
  // -----------------------------------------------------------------------
  static double grid_min_energy()
  {
    return 85;
  }

  static double grid_max_energy()
  {
    return 110;
  }

  template <typename T>
  static bool is_physically_valid_energy(const T& E)
  {
    const double e = scalar_value(E);
    return (e > 0.0) && (e >= grid_min_energy()) && (e <= grid_max_energy());
  }


  // indices
    // p[0]  = C    10;
    // p[1]  = b3   4.312;
    // p[2]  = b4   348.879;
    // p[3]  = b5  -27.491;
    // p[4]  = b6   275.427;
    // p[5]  = b7  -28.125;
    // p[6]  = b8   1419.462;
    // p[7]  = b9   111.853;
    // p[8]  = b10  644.646;
    // p[9]  = b11  2352.294;
    // p[10] = b12  2725.173;
    // p[11] = b13  3448.158;
    // p[12] = b14  11.291;

  // -----------------------------------------------------------------------
  //  Empirical backscattered bremm continuum McElroy Eqn 16 App A
  // -----------------------------------------------------------------------
  template <typename T>
  static T bremm_cont_impl(const T& E,
                           const T* const p)
  {
    using ceres::pow;
    const T bremmCont = p[0] * (
                                  p[2]  * safe_sech((E - T(85.283)) / p[1]) +
                                  p[3]  * safe_sech((E - T(85.283) - T(2.390)) / p[1]) +
                                  p[5]  * safe_sech(pow(E - T(85.283), 2) / p[4]) +
                                  p[6]  * safe_sech(pow(E - T(85.283) - T(2.390), 2) / p[4]) +
                                  p[8]  * safe_sech(pow(E - T(85.283), 3) / p[7]) +
                                  p[9]  * safe_sech(pow(E - T(85.283) - T(2.390), 3) / p[7]) +
                                  p[11] * safe_sech(pow(E - T(85.283), 4) / p[10]) +
                                  p[12] * safe_sech(pow(E - T(85.283) - T(2.390), 4) / p[10])
                                );
    return bremmCont;
  }

  template <typename T>
  static T safe_sech(const T& x)
  {
    using ceres::cosh;
    
    // For |x| > 20, sech(x) ≈ 0 (cosh overflows but sech → 0)
    const double abs_x = scalar_value(x);
    if (abs_x > 20.0)
    {
      return T(0.0);
    }
    
    // For moderate x, use standard formula
    if (abs_x < 1.0)
    {
      return T(1.0) / cosh(x);
    }
    
    // For 1 < |x| < 20, use exponential form to avoid overflow:
    // sech(x) = 2/(e^x + e^-x) = 2*e^-x / (1 + e^-2x)
    using ceres::exp;
    const T neg_x = -x;
    const T exp_neg_x = exp(neg_x);
    const T exp_neg_2x = exp(T(2.0) * neg_x);
    
    return T(2.0) * exp_neg_x / (T(1.0) + exp_neg_2x);
  }

//  // -----------------------------------------------------------------------
//  //  Allow for small shifts in energy calibration
//  // -----------------------------------------------------------------------
//  template <typename T>
//  static T corrected_energy(const T& E,
//                            const T* const p)
//  {
//    const parameter_data::KParameterData& constants =
//          KFileManager::getInstance().m_workingParameterSet;
//
//    const T Eref = T(0.5 * (constants.m_XRFFitLowerBound + constants.m_XRFFitUpperBound));
//
//    return E + p[12] + p[13] * (E - Eref);
//  }


  // -----------------------------------------------------------------------
  //  Public-facing full forward model
  // -----------------------------------------------------------------------
  template <typename T>
  static T model_impl(const T& E,
                      const T* const p)
  {
    //const T E = corrected_energy(EObs, p);

   if (!is_physically_valid_energy(E))
   {
     return T(1.0e12);
   }

    return bremm_cont_impl<T>(E, p);
  }

  const double x_;  //x values (Energies in keV)
  const double y_;  //y-values (Bin counts)


};

#endif


