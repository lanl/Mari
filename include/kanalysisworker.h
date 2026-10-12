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

#ifndef KANALYSISWORKER_H
#define KANALYSISWORKER_H

//Project includes
#include "kdevlogger.h"
#include "kkedengine.h"
#include "kxrfengine.h"
#include "kresultsmanager.h"
//Qt includes
#include <QObject>
#include <QThread>

/**
 * Sacrificial analysis worker class. 
 * Acts as an analysis manager but lives for 'this' analysis only
 **/
class KAnalysisWorker : public QObject
{
  Q_OBJECT

public:
  explicit KAnalysisWorker(QObject* parent = nullptr);
  ~KAnalysisWorker();

  void process_results();

  /**
   * Setters for analysis options flags
   **/
  void set_doKED(bool a_checkedState);
  void set_doXRF(bool a_checkedState);
  void set_doLowCXRF(bool a_checkedState);
  void set_doFittingAna(bool a_checkedState);
  void set_doROIAna(bool a_checkedState);
  void set_samp_temp(const double a_sampTemp);

  /**
   * Setters for analysis header info
   **/
  void set_operator_name(const QString& a_operator);
  void set_facility_name(const QString& a_facility);
  void set_comment(const QString& a_comment);
  void set_date(const QDateTime& a_date);
  void set_temp(const double a_temp);
  void set_KED_sample_file(const QString& a_file);
  void set_KED_bkgd_file(const QString& a_file);
  void set_KED_ref_file(const QString& a_file);
  void set_XRF_sample_file(const QString& a_file);
  void set_XRF_bkgd_file(const QString& a_file);
  void set_XRF_ref_file(const QString& a_file);
  void set_param_set(const QString& a_paramSet);
  void set_ana_mode(const int a_anaMode);
  void set_KED_mode(const int a_anaMode);
  void set_XRF_mode(const int a_anaMode);

signals:
  void update_status(const QString& a_msg);
  void ana_success(const QString& a_msg);
  void ana_fail(const QString& a_msg);
  void finished(); // emitted just before the thread quits

  /**
   * Send the KED analysis results to the ResultsManager
   **/
  void send_KED_results(const KKEDResults& a_KEDResults);

    /**
   * Send the XRF analysis results to the ResultsManager
   **/
  void send_XRF_results(const KXRFResults& a_XRFResults);

  /**
   * Send the final analysis results to the ResultsManager
   **/
  void send_results_summary(const KResultsSummary& a_resultsSummary);

public slots:
  /**
   * Run the analysis. This method is thread‑safe because it doesn't 
   * touch any widget directly – it only emits signals.
   * Called when the thread starts
   **/
   void do_analysis();

  /**
   * Receive the KED Ceres Solver analysis results
   **/
  //void receive_KED_Ceres_results(const KCeresResults& a_ceresResults);

  /**
   * Receive the XRF Ceres Solver analysis results
   **/
  //void receive_XRF_Ceres_results(const KCeresResults& a_ceresResults);

  /**
   * Receive the KED analysis results from KKEDEngine
   **/
  void receive_KED_results(const KKEDResults& a_kedResults);

  /**
   * Receive the XRF analysis results from KXRFEngine
   **/
  void receive_XRF_results(const KXRFResults& a_xrfResults);

private:
  //use optionals since we can run the two analyses separately 
  std::optional<KKEDResults> m_kedResults;
  std::optional<KXRFResults> m_xrfResults;
  //we will always have a Ceres results and a final results
  //KCeresResults   m_kedCeresResults;
  //KCeresResults   m_xrfCeresResults;
  KResultsSummary m_resultsSummary;

  /**
   * Flags for analysis options
   **/
  bool m_doKED = false;
  bool m_doXRF = false;
  bool m_doLowCXRF = false;
  bool m_doFittingAna = false;
  bool m_doROIAna = false;
  double m_sampTemp;

};

#endif