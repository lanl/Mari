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

#ifndef KCERESRESULTS_H
#define KCERESRESULTS_H

//Project includes
#include "kdevlogger.h"
//C++ includes
#include <string>
#include <vector>

/**
 * POD Class (Struct) to hold Ceres Solver results
 **/
struct KCeresResults
{
public:
   KCeresResults();
  ~KCeresResults();

  void clear_data();

  //Algorithm & Convergence
  std::string m_linearSolverType;
  std::string m_terminationType;
  std::string m_message;
  double m_gradMaxNorm;
  double m_relDecrease;
  double m_stepNorm;
  bool m_stepSucess;
  double m_reducedChiSquared;

  //Cost and residuals
  double m_initialCost;
  double m_finalCost;
  double m_costDrop;
  int m_numResidualEvals;
  int m_numJacobianEvals;

  //Execution Performance & Iterations
  int m_totalIterations;
  int m_numSuccessfulSteps;
  int m_numUnsuccessfulSteps;
  double m_totalTime;
  double m_jacobianTime;
  double m_linearSolverTime;
};

#endif 