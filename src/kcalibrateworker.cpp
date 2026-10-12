#include "kcalibrateworker.h"

KCalibrateWorker::KCalibrateWorker(QObject* parent) : QObject(parent)
{
  KDevLogger::getInstance().logEvent("KCalibrateWorker object constructed");
}

KCalibrateWorker::~KCalibrateWorker()
{
  KDevLogger::getInstance().logEvent("KCalibrateWorker object destructed");
}

void KCalibrateWorker::set_doKED(bool a_state)
{
  m_doKED = a_state;
}

void KCalibrateWorker::set_doXRF(bool a_state)
{
  m_doXRF = a_state;
}

void KCalibrateWorker::set_date(const QDateTime& a_date)
{
  m_resultsSummary.m_date = a_date;
}

void KCalibrateWorker::set_operator_name(const QString& a_operator)
{
  m_resultsSummary.m_operator = a_operator;
}

void KCalibrateWorker::set_facility_name(const QString& a_facility)
{
  m_resultsSummary.m_facility = a_facility;
}

void KCalibrateWorker::set_comment(const QString& a_comment)
{
  m_resultsSummary.m_comment = a_comment;
}

void KCalibrateWorker::set_KED_sample_file(const QString& a_file)
{
  m_resultsSummary.m_kedSampleFile = a_file;
}

void KCalibrateWorker::set_KED_bkgd_file(const QString& a_file)
{
  m_resultsSummary.m_kedBkgdFile = a_file;
}

void KCalibrateWorker::set_KED_ref_file(const QString& a_file)
{
  m_resultsSummary.m_kedRefFile = a_file;
}

void KCalibrateWorker::set_XRF_sample_file(const QString& a_file)
{
  m_resultsSummary.m_xrfSampleFile = a_file;
}

void KCalibrateWorker::set_XRF_bkgd_file(const QString& a_file)
{
  m_resultsSummary.m_xrfBkgdFile = a_file;
}

void KCalibrateWorker::set_XRF_ref_file(const QString& a_file)
{
  m_resultsSummary.m_xrfRefFile = a_file;
}

void KCalibrateWorker::set_param_set(const QString& a_paramSet)
{
  m_resultsSummary.m_paramSet = a_paramSet;
}

void KCalibrateWorker::set_ana_mode(const int a_anaMode)
{
  m_resultsSummary.m_anaMode = a_anaMode;
}

void KCalibrateWorker::do_inst_calibration()
{
  bool calSuccess = false;
  KCalibrateEngine calibrateEngine;

  if(m_doKED == true)
  {
    //Receive the KED calibration results from the calibration engine
    connect(&calibrateEngine, &KCalibrateEngine::send_KED_cal_results, 
           this, &KCalibrateWorker::receive_KED_cal_results);

    emit update_status("Doing KED Calibration");
    calSuccess = calibrateEngine.calibrate_KED();

    if(calSuccess)
    {
      process_KED_cal_results();
      emit cal_KED_success("KED Calibration Complete!");
    }
    else
    {
      emit cal_KED_fail("KED Calibration Failed!");
    }
  }

  if(m_doXRF == true)
  {
    //Receive the XRF calibration results from the calibration engine
    connect(&calibrateEngine, &KCalibrateEngine::send_XRF_cal_results, 
           this, &KCalibrateWorker::receive_XRF_cal_results);

    emit update_status("Doing XRF Calibration");
    calSuccess = calibrateEngine.calibrate_XRF();

    if(calSuccess)
    {
      process_XRF_cal_results();
      emit cal_XRF_success("XRF Calibration Complete!");
    }
    else
    {
      emit cal_XRF_fail("XRF Calibration Failed!");
    }
  }

  // Tell the dialog that we are done (so it can quit the thread)
  emit finished();
}

void KCalibrateWorker::process_KED_cal_results()
{
  //Send calibration results to the Results Manager
  //connect(this, &KCalibrateWorker::send_KED_cal_results, 
  //        &KResultsManager::getInstance(), &KResultsManager::receive_KED_calibration_results);
  //emit send_KED_cal_results(m_KEDCalResults.value());
}

void KCalibrateWorker::process_XRF_cal_results()
{
  //calculate the residuals and add to XRF cal results
  std::vector<double> residualX;
  std::vector<double> residualY = KMath::calculate_normalized_residuals<KCalibrateXRFResults>(KResultsManager::getInstance().m_XRFCalResults,
                                                                                              residualX);
  KResultsManager::getInstance().m_XRFCalResults.m_residualsX = residualX;
  KResultsManager::getInstance().m_XRFCalResults.m_residualsY = residualY;

  //Send calibration results to the Results Manager
  //connect(this, &KCalibrateWorker::send_XRF_cal_results, 
  //        &KResultsManager::getInstance(), &KResultsManager::receive_XRF_calibration_results);
  //emit send_XRF_cal_results(m_XRFCalResults.value());
}

void KCalibrateWorker::receive_KED_cal_results(const KCalibrateKEDResults& a_KEDCalResults) 
{
  KResultsManager::getInstance().m_KEDCalResults = a_KEDCalResults;
}

void KCalibrateWorker::receive_XRF_cal_results(const KCalibrateXRFResults& a_XRFCalResults) 
{
  KResultsManager::getInstance().m_XRFCalResults = a_XRFCalResults;
}