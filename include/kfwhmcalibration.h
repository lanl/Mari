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

#ifndef KFWHMCALIBRATION_H
#define KFWHMCALIBRATION_H

//Project includes
#include "kvaction.h"
//C++ inclides
#include <cmath>


class KFWHMCalibration : public KVAction
{

public:
  KFWHMCalibration();
  ~KFWHMCalibration();

  virtual void send(int a_message) override;
  virtual void receive(int a_message) override;

  static double calc_fwhm(double a_energy, double a_A1, double a_A2, double a_A3);

private:
  //int m_message;


};

#endif 
