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

#ifndef KCALKEDSOURCETERM_H
#define KCALKEDSOURCETERM_H

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
namespace GaussHermiteKEDCal
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
 *  KED source term - physics model for the KED source term.
 * ---------------------------------------------------------------------------
 *  Fast version:
 *    - keeps Gauss-Hermite convolution in the cost functor
 *    - keeps attenuation inside convolution
 *    - applies smooth edge treatment ONLY to the U and Pu attenuation jumps
 *
 *  The U/Pu edge smoothing is local and cheap:
 *    - away from each edge, raw table interpolation is used
 *    - near each edge, values are blended across the discontinuity with a
 *      smoothstep weight over a width tied to the detector FWHM
 *
 *  This is intended to suppress artificial jaggedness near the real
 *  attenuation jumps without rebuilding a dense blurred grid per residual.
 **/
class KCalKEDSourceTerm
{
public:
  explicit KCalKEDSourceTerm(double a_energy_keV, 
                          double a_counts,
                          double a_continuum);

  ~KCalKEDSourceTerm();

  inline static std::vector<std::pair<double, double>> m_muPuLorentz;
  inline static std::vector<std::pair<double, double>> m_muULorentz;

  static double evaluate(double E_keV,
                         const double* const p,
                         double c)
  {
    return model_impl<double>(double(E_keV), p, c);
  }

  template <typename T>
  bool operator()(const T* const p,
                  T* residual) const
  {
    const T model = model_impl<T>(T(x_), p, T(c_));
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

  // Real tabulated jump locations in uploaded U / Pu tables.
  static constexpr double m_uEdgeKeV  = 115.6;
  static constexpr double m_puEdgeKeV = 121.8;

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

  // -----------------------------------------------------------------------
  //  U / Pu edge-smoothed table lookup
  // -----------------------------------------------------------------------
  template <typename T>
  static T interp_mu_with_single_edge_smoothing(const std::vector<std::pair<double, double>>& table,
                                                const T& E,
                                                double edgeKeV,
                                                double widthKeV)
  {
    const double e = scalar_value(E);

    if (!is_within_table_domain(table, e))
    {
      return T(0.0);
    }

    if (widthKeV <= 1.0e-12)
    {
      return interp_mu(table, E);
    }

    const double leftEnd  = edgeKeV - 0.5 * widthKeV;
    const double rightEnd = edgeKeV + 0.5 * widthKeV;

    if (e <= leftEnd || e >= rightEnd)
    {
      return interp_mu(table, E);
    }

    // Blend between values sampled just to the left and right of the edge.
    const double xLeft  = std::max(table.front().first, edgeKeV - 0.5 * widthKeV);
    const double xRight = std::min(table.back().first,  edgeKeV + 0.5 * widthKeV);

    const T yLeft  = interp_mu(table, T(xLeft));
    const T yRight = interp_mu(table, T(xRight));
    const T w = edge_blend_weight(E, edgeKeV, widthKeV);
    return (T(1.0) - w) * yLeft + w * yRight;
  }

  template <typename T>
  static T interp_mu_u(const T& E)
  {
    const parameter_data::KParameterData& constants =
        KFileManager::getInstance().m_workingParameterSet;
    const double A1 = constants.m_KEDA1;
    const double A2 = constants.m_KEDA2;
    const double A3 = constants.m_KEDA3;
    double fwhm = KFWHMCalibration::calc_fwhm(scalar_value(E), A1, A2,A3);
    const double widthKeV = std::max(m_minEdgeSmoothWidthKeV,
                                     m_edgeSmoothFractionOfFwhm * fwhm);
    return interp_mu_with_single_edge_smoothing(m_muULorentz, E, m_uEdgeKeV, widthKeV);
    //return interp_mu_with_single_edge_smoothing(KAtomicData::m_muU, E, m_uEdgeKeV, widthKeV);
  }

  template <typename T>
  static T interp_mu_pu(const T& E)
  {
    const parameter_data::KParameterData& constants =
        KFileManager::getInstance().m_workingParameterSet;
    const double A1 = constants.m_KEDA1;
    const double A2 = constants.m_KEDA2;
    const double A3 = constants.m_KEDA3;
    double fwhm = KFWHMCalibration::calc_fwhm(scalar_value(E), A1, A2,A3);
    const double widthKeV = std::max(m_minEdgeSmoothWidthKeV,
                                     m_edgeSmoothFractionOfFwhm * fwhm);
    return interp_mu_with_single_edge_smoothing(m_muPuLorentz, E, m_puEdgeKeV, widthKeV);
    //return interp_mu_with_single_edge_smoothing(KAtomicData::m_muPuLorentz, E, m_puEdgeKeV, widthKeV);
  }

  // indices
  // p[0]  = C
  // p[1]  = chi
  // p[2]  = rhoU
  // p[3]  = rhoPu
  // p[4]  = rho matrix material
  // p[5]  = Cd-109 height
  // p[6]  = Cd-109 sigma
  // p[7]  = Cd attenuator thickness
  // p[8]  = Fe attenuator thickness
  // p[9]  = Pb xray 84.4 Ht    
  // p[10] = Pb xray 84.4 Sigma 
  // p[11] = Pb xray 84.9 Ht    
  // p[12] = Pb xray 84.9 Sigma 
  // p[13] = Pb xray 87.3 Ht    
  // p[14] = Pb xray 87.3 Sigma 

  // -----------------------------------------------------------------------
  //  Bare unconvolved source term
  // -----------------------------------------------------------------------
  template <typename T>
  static T bare_model_impl(const T& E,
                           const T* const p)
  {
    using std::log;
    using std::pow;
    using std::sqrt;
    using std::sin;
    using ceres::exp;

    const parameter_data::KParameterData& constants =
        KFileManager::getInstance().m_workingParameterSet;

    const double J = 0.0135 * KAtomicData::m_ZTungsten;
    const double m = 0.1382 - 0.9211 / std::sqrt(KAtomicData::m_ZTungsten);
    const T Uo = T(constants.m_generatorHVSetpoint) / E;

    const T muW = interp_mu(KAtomicData::m_muW, E);

    const T eta = pow(T(constants.m_generatorHVSetpoint), T(m)) *
                  (T(0.1904) - T(0.2236) * log(T(KAtomicData::m_ZTungsten)) +
                   T(0.1292) * pow(log(T(KAtomicData::m_ZTungsten)), T(2)) -
                   T(0.0149) * pow(log(T(KAtomicData::m_ZTungsten)), T(3)));

    const T rhoZm = (T(KAtomicData::m_massTungsten) / T(KAtomicData::m_ZTungsten)) *
                    (T(0.787e-5) * sqrt(T(J)) *
                         pow(T(constants.m_generatorHVSetpoint), T(1.5)) +
                     T(0.735e-6) *
                         pow(T(constants.m_generatorHVSetpoint), T(2)));

    const T rhoZBar = rhoZm * log(Uo) *
        ((T(0.49269) - T(1.0987) * eta + T(0.78557) * pow(eta, T(2))) /
         (T(0.70256) - T(1.09865) * eta + T(1.0046) * pow(eta, T(2)) + log(Uo)));

    const T denom = T(2.0) * muW * rhoZBar *
                    sin(T(constants.m_beamIncidenceAngle)) /
                    sin(T(constants.m_takeoffAngle));

    const T faofE = (T(1.0) - exp(-denom)) / denom;

    return p[0] *
           T(m_solidAngle) * T(m_beamCurrent) * T(KAtomicData::m_ZTungsten) *
           faofE *
           pow(T(constants.m_generatorHVSetpoint) / E - T(1.0), p[1]);
  }

  // -----------------------------------------------------------------------
  //  Attenuation evaluated at energy E
  // -----------------------------------------------------------------------
  template <typename T>
  static T bare_attenuation_impl(const T& E,
                                 const T* const p)
  {
    using ceres::exp;

    const parameter_data::KParameterData& constants =
        KFileManager::getInstance().m_workingParameterSet;

    const T muCd        = interp_mu(KAtomicData::m_muCd, E);
    const T muFe        = interp_mu(KAtomicData::m_muFe, E);
    const T muBe        = interp_mu(KAtomicData::m_muBe, E);
    const T muU         = interp_mu_u(E);
    const T muPu        = interp_mu_pu(E);
    const T muMatrixMat = interp_mu(KAtomicData::m_muHNO3, E);
    const T muVialMat   = interp_mu(KAtomicData::m_muSiO2, E);

    const T hwAttn = exp(-(
        muCd * T(KAtomicData::m_rhoCd) * p[7]   +
        muFe * T(KAtomicData::m_rhoFe) * p[8]   +
        muBe * T(KAtomicData::m_rhoBe) * T(constants.m_beWindowThickness)   +
        muFe * T(KAtomicData::m_rhoFe) * T(constants.m_xfrEntraceThickness) +
        muFe * T(KAtomicData::m_rhoFe) * T(constants.m_xfrExitThickness)
    ));

    const T sampAttn = exp(-(
        muU         * p[2] * T(constants.m_vialID / 10.0) +
        muPu        * p[3] * T(constants.m_vialID / 10.0) +
        muMatrixMat * p[4] * T(constants.m_vialID / 10.0) +
        muVialMat   * T(KAtomicData::m_rhoSiO2) * T((constants.m_vialOD - constants.m_vialID) / 10.0)
    ));

    return hwAttn * sampAttn;
  }

  // -----------------------------------------------------------------------
  //  Cd-109 peak
  // -----------------------------------------------------------------------
  template <typename T>
  static T cd_peak_impl(const T& E,
                        const T* const p,
                        const T& centroid)
  {
    using ceres::exp;

    const T cdPk = p[5]*exp( -(E - centroid)*(E - centroid)/(2.*p[6]*p[6]));

    return cdPk;
  }

  // -----------------------------------------------------------------------
  //  Pb x-rays
  // -----------------------------------------------------------------------
  template <typename T>
  static T pb_xray_impl(const T& E,
                        const T* const p)
  {
    using ceres::exp;

    const T pbPks = p[9]*exp( - (E - T(84.4))*(E - T(84.4))/(2.*p[10]*p[10])) +
                    p[11]*exp( -(E - T(84.9))*(E - T(84.9))/(2.*p[12]*p[12])) +
                    p[13]*exp( -(E - T(87.3))*(E - T(87.3))/(2.*p[14]*p[14]));

    return pbPks;
  }

  // -----------------------------------------------------------------------
  //  Allow for small shifts in energy calibration
  // -----------------------------------------------------------------------
  template <typename T>
  static T corrected_energy(const T& E,
                            const T* const p)
  {
      const parameter_data::KParameterData& constants =
          KFileManager::getInstance().m_workingParameterSet;

      const T Eref = T(0.5 * (constants.m_KEDFitLowerBound + constants.m_KEDFitUpperBound));

      return E + p[15] + p[16] * (E - Eref);
  }

  // -----------------------------------------------------------------------
  //  Fast Gauss-Hermite convolution of full model
  // -----------------------------------------------------------------------
  template <typename T>
  static T convolve_with_gaussian(const T& E,
                                  const T* const p,
                                  const T& c)
  {
    const parameter_data::KParameterData& constants =
        KFileManager::getInstance().m_workingParameterSet;
    const double A1 = constants.m_KEDA1;
    const double A2 = constants.m_KEDA2;
    const double A3 = constants.m_KEDA3;
    double fwhm = KFWHMCalibration::calc_fwhm(scalar_value(E), A1, A2,A3);
    const double sigma = fwhm / (2.0 * std::sqrt(2.0 * std::log(2.0)));

    if (sigma <= 1.0e-9)
    {
      if (!is_physically_valid_energy(E))
      {
        return T(0.0);
      }
      return bare_model_impl<T>(E, p) * bare_attenuation_impl<T>(E, p) +
             cd_peak_impl<T>(E, p, T(88.03)) + pb_xray_impl<T>(E, p) + c;
    }

    constexpr double sqrt2 = 1.4142135623730950488;
    constexpr double invSqrtPi = 0.56418958354775628695;

    T sum = T(0.0);
    T weight_sum = T(0.0);

    for (int i = 0; i < GaussHermiteKEDCal::N; ++i)
    {
      const T shiftedE = E + T(sqrt2 * sigma) * T(GaussHermiteKEDCal::nodes[i]);
      const T shiftedCdCentroid = 88.03 + T(sqrt2 * sigma) * T(GaussHermiteKEDCal::nodes[i]);

      if (!is_physically_valid_energy(shiftedE))
      {
        continue;
      }

      const T fullAtShiftedE =
                                bare_model_impl<T>(shiftedE, p)       *
                                bare_attenuation_impl<T>(shiftedE, p) +
                                cd_peak_impl<T>(shiftedE, p, shiftedCdCentroid) + 
                                pb_xray_impl<T>(shiftedE, p) +
                                c;

      const T wi = T(GaussHermiteKEDCal::weights[i]);
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
  static T model_impl(const T& EObs,
                      const T* const p,
                      const T& c)
  {
    /* 
       Change return statement to 'convolve_with_gaussian()' to do full 
       Gauss-Hermite quadrature. 
       Good but VERY slow
    */
    //return convolve_with_gaussian<T>(E, p, c);
    
    const T E = corrected_energy(EObs, p);

    if (!is_physically_valid_energy(E))
    {
      return T(1.0e12);
    }

    return bare_model_impl<T>(E, p) * bare_attenuation_impl<T>(E, p) +
           cd_peak_impl<T>(E, p, T(88.03)) + pb_xray_impl<T>(E, p) + c;
  }

  const double x_;  //x values (Energues in keV)
  const double y_;  //y-values (Bin counts)
  const double c_;  //Step continuum counts

  static constexpr double m_beamCurrent = 150.0;  // mA
  static constexpr double m_solidAngle  = 100000.; // sr

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

