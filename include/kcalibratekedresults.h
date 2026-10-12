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

#ifndef KCALIBRATEKEDRESULTS_H
#define KCALIBRATEKEDRESULTS_H

//Project includes
#include "kdevlogger.h"
#include "kceresresults.h"
//C++ includes
#include <string>
#include <vector>

/**
 * POD Class (Struct) to hold KED instrument calibration results
 **/
struct KCalibrateKEDResults
{
public:
   KCalibrateKEDResults();
  ~KCalibrateKEDResults();

  void clear_data();

  //fitting statistics
  double m_initialCost;//cost = 0.5*chi-square
  double m_finalCost;
  int m_numResiduals;
  int m_numParams;
  bool m_converged;

  //results



  //Fitted curve values
  std::vector<double> m_fitROIEnergies;
  std::vector<double> m_fitROIDataCounts;
  std::vector<double> m_fitROIModelCounts;
  std::vector<double> m_continuumCounts;
  std::vector<double> m_coincCounts;
  std::vector<double> m_residualsX;
  std::vector<double> m_residualsY;

  KCeresResults m_ceresResults;
};

#endif 