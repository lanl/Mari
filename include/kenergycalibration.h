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

#ifndef KENERGYCALIBRATION_H
#define KENERGYCALIBRATION_H

//Project includes
#include "kvaction.h"
#include "kspectraldata.h"
//C++ includes
#include <array>

class KEnergyCalibration : public KVAction
{

public:
  KEnergyCalibration();
  ~KEnergyCalibration();

  virtual void send(int a_message) override;
  virtual void receive(int a_message) override;

  std::array<float,3> get_energy_calibration(KSpectralData& a_dataset);
  void set_energy_calibration(KSpectralData& a_dataset,
                                  const std::array<float,3> a_newCoeffs);

  /**
   * Updates spectrum energy calibration when user right-clicks and
   * drags on the plot
   **/
  std::array<float,3> calibrate_from_mouse_drag(KSpectralData& a_dataset, 
                                                const float a_peakLocActual, 
                                                const float a_peakLocDesired);

  /**
   * Updates spectrum energy calibration when user changes value
   * in one of QLineEdit ECal boxes
   **/
  void calibrate_from_box(KSpectralData& a_dataset, 
                          const int a_param, 
                          const float a_newValue);

  /**
   * Recalculates the m_energies vector after a change in the energy
   * calibration coefficients
   **/
  void update_energy_vec(KSpectralData& a_dataset);

private:
  //int m_message;


};

#endif 
