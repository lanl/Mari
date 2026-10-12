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

#ifndef KXRFSOURCETERM_H
#define KXRFSOURCETERM_H

// Project includes
#include "katomicdata.h"
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

/*======================================================================
  Gauss-Hermite quadrature utilities (order 8)
======================================================================*/
namespace GaussHermiteXRF
{
  constexpr int N = 8;

  constexpr std::array<double, N> nodes = {
      -2.930637420257244,
      -1.981656756695843,
      -1.157193712446780,
      -0.381186990207322,
       0.381186990207322,
       1.157193712446780,
       1.981656756695843,
       2.930637420257244
  };
  constexpr std::array<double, N> weights = {
      0.000199604072211,
      0.017077983007413,
      0.207802325814891,
      0.661147012558241,
      0.661147012558241,
      0.207802325814891,
      0.017077983007413,
      0.000199604072211
  };
}

/**---------------------------------------------------------------------------
 *  XRF source term - physics model for the XRF source term.
 * --------------------------------------------------------------------------**/
class KXRFSourceTerm
{
public:
  explicit KXRFSourceTerm(double a_energy_keV, 
                          double a_counts);

  ~KXRFSourceTerm();

  inline static std::vector<std::pair<double, double>> m_muPuLorentz;
  inline static std::vector<std::pair<double, double>> m_muULorentz;

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
  //  Tunable controls
  // -----------------------------------------------------------------------
  static constexpr double m_minEdgeSmoothWidthKeV = 0.06;
  static constexpr double m_edgeSmoothFractionOfFwhm = 1.5;


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

  static double interp_mu(const std::vector<std::pair<double, double>>& table,
                          double x)
  {
    return KMath::loglog_linear_interpolation<double>(table, x);
    //return KMath::linear_interpolation<double>(table, x);
  }

  template <typename Scalar, int N>
  static ceres::Jet<Scalar, N> interp_mu(
      const std::vector<std::pair<double, double>>& table,
      const ceres::Jet<Scalar, N>& x)
  {
    return ceres::Jet<Scalar, N>(
        KMath::loglog_linear_interpolation<double>(table, static_cast<double>(x.a)));
        //KMath::linear_interpolation<double>(table, static_cast<double>(x.a)));
  }

  // -----------------------------------------------------------------------
  //  Smoothstep for local edge blending
  // -----------------------------------------------------------------------
  template <typename T>
  static T smooth_step_01(const T& x)
  {
    if (scalar_value(x) <= 0.0)
    {
      return T(0.0);
    }
    if (scalar_value(x) >= 1.0)
    {
      return T(1.0);
    }
    return x * x * (T(3.0) - T(2.0) * x);
  }

  template <typename T>
  static T edge_blend_weight(const T& E, double edgeKeV, double widthKeV)
  {
    // Maps [edge-width/2, edge+width/2] -> [0,1]
    return smooth_step_01((E - T(edgeKeV - 0.5 * widthKeV)) / T(widthKeV));
  }

  // -----------------------------------------------------------------------
  //  Common valid energy domain
  // -----------------------------------------------------------------------
  static double grid_min_energy()
  {
    return std::max({KAtomicData::m_muW.front().first,
                     KAtomicData::m_muCd.front().first,
                     KAtomicData::m_muFe.front().first,
                     KAtomicData::m_muU.front().first,
                     KAtomicData::m_muPu.front().first,
                     KAtomicData::m_muHNO3.front().first});
  }

  static double grid_max_energy()
  {
    const parameter_data::KParameterData& constants =
        KFileManager::getInstance().m_workingParameterSet;

    return std::min({KAtomicData::m_muW.back().first,
                     KAtomicData::m_muCd.back().first,
                     KAtomicData::m_muFe.back().first,
                     KAtomicData::m_muU.back().first,
                     KAtomicData::m_muPu.back().first,
                     KAtomicData::m_muHNO3.back().first,
                     constants.m_generatorHVSetpoint});
  }

  template <typename T>
  static bool is_physically_valid_energy(const T& E)
  {
    const double e = scalar_value(E);
    return (e > 0.0) && (e >= grid_min_energy()) && (e < grid_max_energy());
  }


  // indices
  // p[0] = Pb xray 84.4 Ht    
  // p[1] = Pb xray 84.4 Sigma 
  // p[2] = Pb xray 84.9 Ht    
  // p[3] = Pb xray 84.9 Sigma 
  // p[4] = Pb xray 87.3 Ht    
  // p[5] = Pb xray 87.3 Sigma 



  // -----------------------------------------------------------------------
  //  U x-rays
  // -----------------------------------------------------------------------
  template <typename T>
  static T u_xray_impl(const T& E,
                       const T* const p)
  {
    using ceres::exp;

    const T uPks =  p[0]*exp( -(E - T(KAtomicData::m_uKalpha1_energy))*(E - T(KAtomicData::m_uKalpha1_energy))/(T(2.*0.315*0.315))) +
                    p[1]*exp( -(E - T(KAtomicData::m_uKalpha2_energy))*(E - T(KAtomicData::m_uKalpha2_energy))/(T(2.*0.315*0.315))) +
                    p[2]*exp( -(E - T(KAtomicData::m_uKalpha3_energy))*(E - T(KAtomicData::m_uKalpha3_energy))/(T(2.*0.315*0.315))) +
                    p[3]*exp( -(E - T(KAtomicData::m_uKbeta1_energy))*( E - T(KAtomicData::m_uKbeta1_energy))/(T(2.*0.315*0.315))) +
                    p[4]*exp( -(E - T(KAtomicData::m_uKbeta2_energy))*( E - T(KAtomicData::m_uKbeta2_energy))/(T(2.*0.315*0.315))) +
                    p[5]*exp( -(E - T(KAtomicData::m_uKbeta3_energy))*( E - T(KAtomicData::m_uKbeta3_energy))/(T(2.*0.315*0.315))) + 
                    p[6]*exp( -(E - T(KAtomicData::m_uKbeta4_energy))*( E - T(KAtomicData::m_uKbeta4_energy))/(T(2.*0.315*0.315))) + 
                    p[7]*exp( -(E - T(KAtomicData::m_uKbeta5_energy))*( E - T(KAtomicData::m_uKbeta5_energy))/(T(2.*0.315*0.315))) +
                    p[8]*exp( -(E - T(KAtomicData::m_uKO23_energy))*(   E - T(KAtomicData::m_uKO23_energy))/(T(2.*0.315*0.315))) + 
                    p[9]*exp( -(E - T(KAtomicData::m_uKP23_energy))*(   E - T(KAtomicData::m_uKP23_energy))/(T(2.*0.315*0.315)));

    return uPks;
  }

  // -----------------------------------------------------------------------
  //  Pu x-rays
  // -----------------------------------------------------------------------
  template <typename T>
  static T pu_xray_impl(const T& E,
                        const T* const p)
  {
    using ceres::exp;

    const T puPks =  p[10]*exp( -(E - T(KAtomicData::m_puKalpha1_energy))*(E - T(KAtomicData::m_puKalpha1_energy))/(T(2.*0.315*0.315))) +
                     p[11]*exp( -(E - T(KAtomicData::m_puKalpha2_energy))*(E - T(KAtomicData::m_puKalpha2_energy))/(T(2.*0.315*0.315))) +
                     p[12]*exp( -(E - T(KAtomicData::m_puKalpha3_energy))*(E - T(KAtomicData::m_puKalpha3_energy))/(T(2.*0.315*0.315))) +
                     p[13]*exp( -(E - T(KAtomicData::m_puKbeta1_energy))*( E - T(KAtomicData::m_puKbeta1_energy))/(T(2.*0.315*0.315))) +
                     p[14]*exp( -(E - T(KAtomicData::m_puKbeta2_energy))*( E - T(KAtomicData::m_puKbeta2_energy))/(T(2.*0.315*0.315))) +
                     p[15]*exp( -(E - T(KAtomicData::m_puKbeta3_energy))*( E - T(KAtomicData::m_puKbeta3_energy))/(T(2.*0.315*0.315))) + 
                     p[16]*exp( -(E - T(KAtomicData::m_puKbeta4_energy))*( E - T(KAtomicData::m_puKbeta4_energy))/(T(2.*0.315*0.315))) + 
                     p[17]*exp( -(E - T(KAtomicData::m_puKbeta5_energy))*( E - T(KAtomicData::m_puKbeta5_energy))/(T(2.*0.315*0.315))) +
                     p[18]*exp( -(E - T(KAtomicData::m_puKO23_energy))*(   E - T(KAtomicData::m_puKO23_energy))/(T(2.*0.315*0.315))) + 
                     p[19]*exp( -(E - T(KAtomicData::m_puKP23_energy))*(   E - T(KAtomicData::m_puKP23_energy))/(T(2.*0.315*0.315)));

    return puPks;
  }

  // -----------------------------------------------------------------------
  //  Np x-rays
  // -----------------------------------------------------------------------
  template <typename T>
  static T np_xray_impl(const T& E,
                        const T* const p)
  {
    using ceres::exp;

    const T npPks =  p[20]*exp( -(E - T(KAtomicData::m_npKalpha1_energy))*(E - T(KAtomicData::m_npKalpha1_energy))/(T(2.*0.315*0.315))) +
                     p[21]*exp( -(E - T(KAtomicData::m_npKalpha2_energy))*(E - T(KAtomicData::m_npKalpha2_energy))/(T(2.*0.315*0.315))) +
                     p[22]*exp( -(E - T(KAtomicData::m_npKalpha3_energy))*(E - T(KAtomicData::m_npKalpha3_energy))/(T(2.*0.315*0.315))) +
                     p[23]*exp( -(E - T(KAtomicData::m_npKbeta1_energy))*( E - T(KAtomicData::m_npKbeta1_energy))/(T(2.*0.315*0.315))) +
                     p[24]*exp( -(E - T(KAtomicData::m_npKbeta2_energy))*( E - T(KAtomicData::m_npKbeta2_energy))/(T(2.*0.315*0.315))) +
                     p[25]*exp( -(E - T(KAtomicData::m_npKbeta3_energy))*( E - T(KAtomicData::m_npKbeta3_energy))/(T(2.*0.315*0.315))) + 
                     p[26]*exp( -(E - T(KAtomicData::m_npKbeta4_energy))*( E - T(KAtomicData::m_npKbeta4_energy))/(T(2.*0.315*0.315))) + 
                     p[27]*exp( -(E - T(KAtomicData::m_npKbeta5_energy))*( E - T(KAtomicData::m_npKbeta5_energy))/(T(2.*0.315*0.315))) +
                     p[28]*exp( -(E - T(KAtomicData::m_npKO23_energy))*(   E - T(KAtomicData::m_npKO23_energy))/(T(2.*0.315*0.315))) + 
                     p[29]*exp( -(E - T(KAtomicData::m_npKP23_energy))*(   E - T(KAtomicData::m_npKP23_energy))/(T(2.*0.315*0.315)));

    return npPks;
  }

  // -----------------------------------------------------------------------
  //  Am x-rays
  // -----------------------------------------------------------------------
  template <typename T>
  static T am_xray_impl(const T& E,
                        const T* const p)
  {
    using ceres::exp;

    const T amPks =  p[30]*exp( -(E - T(KAtomicData::m_amKalpha1_energy))*(E - T(KAtomicData::m_amKalpha1_energy))/(T(2.*0.315*0.315))) +
                     p[31]*exp( -(E - T(KAtomicData::m_amKalpha2_energy))*(E - T(KAtomicData::m_amKalpha2_energy))/(T(2.*0.315*0.315))) +
                     p[32]*exp( -(E - T(KAtomicData::m_amKalpha3_energy))*(E - T(KAtomicData::m_amKalpha3_energy))/(T(2.*0.315*0.315))) +
                     p[33]*exp( -(E - T(KAtomicData::m_amKbeta1_energy))*( E - T(KAtomicData::m_amKbeta1_energy))/(T(2.*0.315*0.315))) +
                     p[34]*exp( -(E - T(KAtomicData::m_amKbeta2_energy))*( E - T(KAtomicData::m_amKbeta2_energy))/(T(2.*0.315*0.315))) +
                     p[35]*exp( -(E - T(KAtomicData::m_amKbeta3_energy))*( E - T(KAtomicData::m_amKbeta3_energy))/(T(2.*0.315*0.315))) + 
                     p[36]*exp( -(E - T(KAtomicData::m_amKbeta4_energy))*( E - T(KAtomicData::m_amKbeta4_energy))/(T(2.*0.315*0.315))) + 
                     p[37]*exp( -(E - T(KAtomicData::m_amKbeta5_energy))*( E - T(KAtomicData::m_amKbeta5_energy))/(T(2.*0.315*0.315))) +
                     p[38]*exp( -(E - T(KAtomicData::m_amKO23_energy))*(   E - T(KAtomicData::m_amKO23_energy))/(T(2.*0.315*0.315))) + 
                     p[39]*exp( -(E - T(KAtomicData::m_amKP23_energy))*(   E - T(KAtomicData::m_amKP23_energy))/(T(2.*0.315*0.315)));

    return amPks;
  }

  // -----------------------------------------------------------------------
  //  Cm x-rays
  // -----------------------------------------------------------------------
  template <typename T>
  static T cm_xray_impl(const T& E,
                        const T* const p)
  {
    using ceres::exp;

    const T cmPks =  p[40]*exp( -(E - T(KAtomicData::m_cmKalpha1_energy))*(E - T(KAtomicData::m_cmKalpha1_energy))/(T(2.*0.315*0.315))) +
                     p[41]*exp( -(E - T(KAtomicData::m_cmKalpha2_energy))*(E - T(KAtomicData::m_cmKalpha2_energy))/(T(2.*0.315*0.315))) +
                     p[42]*exp( -(E - T(KAtomicData::m_cmKalpha3_energy))*(E - T(KAtomicData::m_cmKalpha3_energy))/(T(2.*0.315*0.315))) +
                     p[43]*exp( -(E - T(KAtomicData::m_cmKbeta1_energy))*( E - T(KAtomicData::m_cmKbeta1_energy))/(T(2.*0.315*0.315))) +
                     p[44]*exp( -(E - T(KAtomicData::m_cmKbeta2_energy))*( E - T(KAtomicData::m_cmKbeta2_energy))/(T(2.*0.315*0.315))) +
                     p[45]*exp( -(E - T(KAtomicData::m_cmKbeta3_energy))*( E - T(KAtomicData::m_cmKbeta3_energy))/(T(2.*0.315*0.315))) + 
                     p[46]*exp( -(E - T(KAtomicData::m_cmKbeta4_energy))*( E - T(KAtomicData::m_cmKbeta4_energy))/(T(2.*0.315*0.315))) + 
                     p[47]*exp( -(E - T(KAtomicData::m_cmKbeta5_energy))*( E - T(KAtomicData::m_cmKbeta5_energy))/(T(2.*0.315*0.315))) +
                     p[48]*exp( -(E - T(KAtomicData::m_cmKO23_energy))*(   E - T(KAtomicData::m_cmKO23_energy))/(T(2.*0.315*0.315))) + 
                     p[49]*exp( -(E - T(KAtomicData::m_cmKP23_energy))*(   E - T(KAtomicData::m_cmKP23_energy))/(T(2.*0.315*0.315)));

    return cmPks;
  }

  // -----------------------------------------------------------------------
  //  Empirical Bremm continuum McElroy Eqn 16 App A
  // -----------------------------------------------------------------------
  template <typename T>
  static T bremm_cont_impl(const T& E,
                           const T* const p)
  {
    const double b3  =  4.312;
    const double b4  =  348.879;
    const double b5  = -27.491;
    const double b6  =  275.427;
    const double b7  = -28.125;
    const double b8  =  1419.462;
    const double b9  =  111.853;
    const double b10 =  644.646;
    const double b11 =  2352.294;
    const double b12 =  2725.173;
    const double b13 =  3448.158;
    const double b14 =  11.291;

    using ceres::cosh;
    using ceres::pow;
  const T bremmCont = p[50] * (
                                T(b4)  * safe_sech((E - T(85.283)) / T(b3)) +
                                T(b5)  * safe_sech((E - T(85.283) - T(2.390)) / T(b3)) +
                                T(b7)  * safe_sech(pow(E - T(85.283), 2) / T(b6)) +
                                T(b8)  * safe_sech(pow(E - T(85.283) - T(2.390), 2) / T(b6)) +
                                T(b10) * safe_sech(pow(E - T(85.283), 3) / T(b9)) +
                                T(b11) * safe_sech(pow(E - T(85.283) - T(2.390), 3) / T(b9)) +
                                T(b13) * safe_sech(pow(E - T(85.283), 4) / T(b12)) +
                                T(b14) * safe_sech(pow(E - T(85.283) - T(2.390), 4) / T(b12))
                              );
    return bremmCont;
  }


  // -----------------------------------------------------------------------
  //  Double exponential continuum
  // -----------------------------------------------------------------------
  template <typename T>
  static T double_exp_cont_impl(const T& E,
                                const T* const p)
  { 
    using ceres::exp;
    const double refE = 85.0;

    const T dblExpCont = p[50]*exp( -p[51]*(E - T(refE))) + 
                         p[52]*exp( -p[53]*(E - T(refE))) +
                         p[54];
    return dblExpCont;
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
  //  Fast Gauss-Hermite convolution of full model
  // -----------------------------------------------------------------------
  template <typename T>
  static T convolve_with_gaussian(const T& E,
                                  const T* const p)
  {
    const parameter_data::KParameterData& constants =
        KFileManager::getInstance().m_workingParameterSet;
    const double A1 = constants.m_XRFA1;
    const double A2 = constants.m_XRFA2;
    const double A3 = constants.m_XRFA3;
    double fwhm = KFWHMCalibration::calc_fwhm(scalar_value(E), A1, A2,A3);
    const double sigma = fwhm / (2.0 * std::sqrt(2.0 * std::log(2.0)));

    if (sigma <= 1.0e-9)
    {
      if (!is_physically_valid_energy(E))
      {
        return T(0.0);
      }
      return bare_model_impl<T>(E, p) * bare_attenuation_impl<T>(E, p) +
             cd_peak_impl<T>(E, p, T(88.03)) + u_xray_impl<T>(E, p);
    }

    constexpr double sqrt2 = 1.4142135623730950488;
    constexpr double invSqrtPi = 0.56418958354775628695;

    T sum = T(0.0);
    T weight_sum = T(0.0);

    for (int i = 0; i < GaussHermiteXRF::N; ++i)
    {
      const T shiftedE = E + T(sqrt2 * sigma) * T(GaussHermiteXRF::nodes[i]);
      const T shiftedCdCentroid = 88.03 + T(sqrt2 * sigma) * T(GaussHermiteXRF::nodes[i]);

      if (!is_physically_valid_energy(shiftedE))
      {
        continue;
      }

      const T fullAtShiftedE =
                                bare_model_impl<T>(shiftedE, p)       *
                                bare_attenuation_impl<T>(shiftedE, p) +
                                cd_peak_impl<T>(shiftedE, p, shiftedCdCentroid) + 
                                u_xray_impl<T>(shiftedE, p);

      const T wi = T(GaussHermiteXRF::weights[i]);
      sum += wi * fullAtShiftedE;
      weight_sum += wi;
    }

    if (scalar_value(weight_sum) <= 1.0e-18)
    {
      return T(0.0);
    }

    // Renormalize by surviving weights to keep the effective kernel normalized
    // when nodes are dropped at the physical boundaries.
    return T(invSqrtPi) * sum / (T(invSqrtPi) * weight_sum);
  }

  // -----------------------------------------------------------------------
  //  Public-facing full forward model
  // -----------------------------------------------------------------------
  template <typename T>
  static T model_impl(const T& E,
                      const T* const p)
  {
    /* 
       Change return statement to 'convolve_with_gaussian()' to do full 
       Gauss-Hermite quadrature. 
       Good but VERY slow
    */
    //return convolve_with_gaussian<T>(E, p, c);
    
    //const T E = corrected_energy(EObs, p);

    if (!is_physically_valid_energy(
      E))
    {
      return T(1.0e12);
    }

    return  u_xray_impl<T>(E, p) + 
            pu_xray_impl<T>(E, p) + 
            np_xray_impl<T>(E, p) + 
            am_xray_impl<T>(E, p) + 
            cm_xray_impl<T>(E, p) + 
            bremm_cont_impl<T>(E, p);
  }

  const double x_;  //x values (Energies in keV)
  const double y_;  //y-values (Bin counts)


};

#endif


/*
first 20 nodes
      -5.387480890011232862, -4.603682449550744273, -3.944764040115625210,
      -3.347854567383216124, -2.788806058428130830, -2.254974002089275523,
      -1.738537712116586208, -1.234076215395323007, -0.737473728545394306,
      -0.245340708300901262,  0.245340708300901262,  0.737473728545394306,
       1.234076215395323007,  1.738537712116586208,  2.254974002089275523,
       2.788806058428130830,  3.347854567383216124,  3.944764040115625210,
       4.603682449550744273,  5.387480890011232862
*/
/*
first 20 weights
      2.229393645534151292e-13, 4.399340992273180554e-10, 1.086069370769281694e-07,
      7.802556478532063694e-06, 0.000228338636016353967, 0.003243773342237814368,
      0.02481052088746389117,  0.1090172060200233220,   0.286675505362834131,
      0.462243669600610089,    0.462243669600610089,    0.286675505362834131,
      0.1090172060200233220,   0.02481052088746389117,  0.003243773342237814368,
      0.000228338636016353967, 7.802556478532063694e-06, 1.086069370769281694e-07,
      4.399340992273180554e-10, 2.229393645534151292e-13
*/

