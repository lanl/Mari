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

#ifndef KCALIBRATEENGINE_H
#define KCALIBRATEENGINE_H

//Qt includes
#include <QtCore/QObject>
//external includes 
#include <ceres/covariance.h>
//Project includes
#include "kvaction.h"
#include "kfilemanager.h"
#include "kcalkedsourceterm.h"
#include "kcalxrfsourceterm.h"
#include "kenergycalibrationprior.h"
#include "kcalibratekedresults.h"
#include "kcalibratexrfresults.h"
#include "katomicdata.h"

class KCalibrateEngine : public QObject, public KVAction
{

  Q_OBJECT //call Q_OBJECT meta object macro

public:
  KCalibrateEngine();
  ~KCalibrateEngine();

  virtual void send(int a_message) override;
  virtual void receive(int a_message) override;

  bool calibrate_KED();
  bool calibrate_XRF();



  /**
   * Calculate the value of the smooth step function continuum at a given bin.
   * Eqn 13 pg A-10 of McElroy MEKED
   * a_bin = bin number for numerator sum start
   * a_lowEAvg = b_lower
   * a_highEAvg = b_upper
   * a_roiSum = SIGMA_lower^upper (C(E))
   **/
   std::vector<double> compute_continuum(const std::vector<double>& a_counts);

  /**
   * Implement McElroy Eqn 22 pg A-15
   **/
  std::vector<double> compute_coinc_summing(const std::vector<double>& a_counts);

  /**
   * Similar to McElroy Eqn 22 except subtract out the coincidnece value for each
   * bin since this is the actual number of photons of this energy incident
   * on the detector.
   **/
  std::vector<double> compute_coinc_summing_tsn(const std::vector<double>& a_counts);

private:
  int a_message;

  KCalibrateKEDResults   m_calKEDResults;
  KCalibrateXRFResults   m_calXRFResults;

signals:
  void send_Ceres_results(const KCeresResults& a_results);
  void send_KED_cal_results(const KCalibrateKEDResults& a_results);
  void send_XRF_cal_results(const KCalibrateXRFResults& a_results);
  void fit_summary(const QString& a_briefReport);
};

#endif 
