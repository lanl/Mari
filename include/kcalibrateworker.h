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

#ifndef KCALIBRATEWORKER_H
#define KCALIBRATEWORKER_H

//Project includes
#include "kdevlogger.h"
#include "kcalibrateengine.h"
#include "kresultsmanager.h"
//Qt includes
#include <QObject>
#include <QThread>

/**
 * Sacrificial calibrate worker class. 
 * Acts as an calibrate manager but lives for 'this' calibrate only
 **/
class KCalibrateWorker : public QObject
{
  Q_OBJECT

public:
  explicit KCalibrateWorker(QObject* parent = nullptr);
  ~KCalibrateWorker();

  /**
   * Does any necssary work such as calculating residuals, then sends
   * the results to the results manager
   **/
  void process_KED_cal_results();
  void process_XRF_cal_results();

  /**
   * Setters for analysis options flags
   **/
  void set_doKED(bool a_state);
  void set_doXRF(bool a_state);

  /**
   * Setters for analysis header info
   **/
  void set_operator_name(const QString& a_operator);
  void set_facility_name(const QString& a_facility);
  void set_comment(const QString& a_comment);
  void set_date(const QDateTime& a_date);
  void set_KED_sample_file(const QString& a_file);
  void set_KED_bkgd_file(const QString& a_file);
  void set_KED_ref_file(const QString& a_file);
  void set_XRF_sample_file(const QString& a_file);
  void set_XRF_bkgd_file(const QString& a_file);
  void set_XRF_ref_file(const QString& a_file);
  void set_param_set(const QString& a_paramSet);
  void set_ana_mode(const int a_anaMode);


signals:
  void update_status(const QString& a_msg);
  void cal_KED_success(const QString& a_msg);
  void cal_XRF_success(const QString& a_msg);
  void cal_KED_fail(const QString& a_msg);
  void cal_XRF_fail(const QString& a_msg);
  void finished(); // emitted just before the thread quits

  /**
   * Send the KED calibration results to the ResultsManager
   **/
  void send_KED_cal_results(const KCalibrateKEDResults& a_KEDCalResults);

    /**
   * Send the XRF calibration results to the ResultsManager
   **/
  void send_XRF_cal_results(const KCalibrateXRFResults& a_XRFCalResults);

  /**
   * Send the final analysis results to the ResultsManager
   **/
  void send_results_summary(const KResultsSummary& a_resultsSummary);

public slots:
  /**
   * Run the instrument calibration. 
   * This method is thread‑safe because it doesn't touch any widget directly 
   * – it only emits signals.
   * Called when the thread starts
   **/
   void do_inst_calibration();

  /**
   * Receive the KED calibration results from Calibration Engine
   **/
  void receive_KED_cal_results(const KCalibrateKEDResults& a_KEDCalResults);

  /**
   * Receive the XRF calibration results from Calibration Engine
   **/
  void receive_XRF_cal_results(const KCalibrateXRFResults& a_XRFCalResults);

private:

  KResultsSummary m_resultsSummary;
  /**
   * Flags for analysis options
   **/
  bool m_doKED = false;
  bool m_doXRF = false;

};

#endif