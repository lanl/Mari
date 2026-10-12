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

#ifndef KRESULTSMANAGER_H
#define KRESULTSMANAGER_H

//Qt includes
#include <QtCore/QObject>
#include <QMetaType>
//Project includes
#include "kvmanager.h"
#include "kkedresults.h"
#include "kxrfresults.h"
#include "kcalibratekedresults.h"
#include "kcalibratexrfresults.h"
#include "kresultssummary.h"
//C++ includes
#include <vector>

/*
  KKEDEngine emits custom data and connects them in the worker thread 
  to KResultsManager, which lives on the GUI thread. 
  That makes them queued connections across threads.
  Qt requires queued argument types to be registered with the meta-object system.
*/
Q_DECLARE_METATYPE(std::vector<double>)
Q_DECLARE_METATYPE(KKEDResults)
Q_DECLARE_METATYPE(KXRFResults)
Q_DECLARE_METATYPE(KCalibrateKEDResults)
Q_DECLARE_METATYPE(KCalibrateXRFResults)
Q_DECLARE_METATYPE(KResultsSummary)

/**
 * Overall results manager ans stitcher class
 **/
class KResultsManager : public QObject, public KVManager
{

  Q_OBJECT //call Q_OBJECT meta object macro

public:
  /**
   * Global access getter for the single instance
   **/
  static KResultsManager& getInstance();

  /**
   * Need to delete the copy constructor and assignment operators
   * to prevent copying
   **/
  KResultsManager(const KResultsManager&) = delete;
  KResultsManager& operator=(const KResultsManager&) = delete;

  /**
   * Obey abstract base class contract and override pure virtual functions
   **/
  virtual void send(int a_message) override;
  virtual void receive(int a_message) override;

  //KKEDResults m_KEDResults;
  //KXRFResults m_XRFResults;
  KCalibrateKEDResults m_KEDCalResults;
  KCalibrateXRFResults m_XRFCalResults;

  
public slots:
  /**
   * Receive the KED analysis results
   **/
  void receive_KED_results(const KKEDResults& a_KEDResults);

  /**
   * Receive the XRF analysis results
   **/
  void receive_XRF_results(const KXRFResults& a_XRFResults);

  /**
   * Receive the KED calibration results
   **/
  void receive_KED_calibration_results(const KCalibrateKEDResults& a_KEDCalResults);

  /**
   * Receive the XRF calibration results
   **/
  void receive_XRF_calibration_results(const KCalibrateXRFResults& a_XRFCalResults);

  /**
   * Receive the analysis results summary
   **/
  void receive_results_summary(const KResultsSummary& a_resultsSummary);


private:
/**
 * Default constructor.
 **/
  KResultsManager();

/**
 * Default destructor.
 **/
  ~KResultsManager();

  int a_message;

signals:
  void send_KED_results(const KKEDResults& a_KEDResults);
  void send_XRF_results(const KXRFResults& a_XRFResults);
  void send_KED_calibration_results(const KCalibrateKEDResults& a_KEDCalResults);
  void send_XRF_calibration_results(const KCalibrateXRFResults& a_XRFCalResults);
  void send_results_summary(const KResultsSummary& a_resultsSummary);

  void printable_summary(const QString& a_summary);

};

#endif 
