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

#ifndef KENERGYCALIBRATIONPRIOR_H
#define KENERGYCALIBRATIONPRIOR_H

// Project includes
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

struct KEnergyCalibrationPrior 
{
  KEnergyCalibrationPrior(double a_offsetSigma,
                          double a_gainSigma)
      : m_offsetSigma(a_offsetSigma),
        m_gainSigma(a_gainSigma) {}

  template <typename T>
  bool operator()(const T* const p, T* residual) const
  {
    residual[0] = p[15] / T(m_offsetSigma);
    residual[1] = p[16] / T(m_gainSigma);
    return true;
  }

  double m_offsetSigma;
  double m_gainSigma;
};

#endif