#include "kanalysisworker.h"

KAnalysisWorker::KAnalysisWorker(QObject* parent) : QObject(parent)
{
  KDevLogger::getInstance().logEvent("KAnalysisWorker object constructed");
}

KAnalysisWorker::~KAnalysisWorker()
{
  KDevLogger::getInstance().logEvent("KAnalysisWorker object destructed");
}

void KAnalysisWorker::set_doKED(bool a_checkedState)
{
  m_doKED = a_checkedState;
}

void KAnalysisWorker::set_doXRF(bool a_checkedState)
{
  m_doXRF = a_checkedState;
}

void KAnalysisWorker::set_doLowCXRF(bool a_checkedState)
{
  m_doLowCXRF = a_checkedState;
}

void KAnalysisWorker::set_doFittingAna(bool a_checkedState)
{
  m_doFittingAna = a_checkedState;
}

void KAnalysisWorker::set_doROIAna(bool a_checkedState)
{
  m_doROIAna = a_checkedState;
}

void KAnalysisWorker::set_samp_temp(const double a_sampTemp)
{
  m_sampTemp = a_sampTemp;
}

void KAnalysisWorker::set_date(const QDateTime& a_date)
{
  m_resultsSummary.m_date = a_date;
}

void KAnalysisWorker::set_operator_name(const QString& a_operator)
{
  m_resultsSummary.m_operator = a_operator;
}

void KAnalysisWorker::set_facility_name(const QString& a_facility)
{
  m_resultsSummary.m_facility = a_facility;
}

void KAnalysisWorker::set_comment(const QString& a_comment)
{
  m_resultsSummary.m_comment = a_comment;
}

void KAnalysisWorker::set_temp(const double a_temp)
{
  m_resultsSummary.m_sampTemp = a_temp;
}

void KAnalysisWorker::set_KED_sample_file(const QString& a_file)
{
  m_resultsSummary.m_kedSampleFile = a_file;
}

void KAnalysisWorker::set_KED_bkgd_file(const QString& a_file)
{
  m_resultsSummary.m_kedBkgdFile = a_file;
}

void KAnalysisWorker::set_KED_ref_file(const QString& a_file)
{
  m_resultsSummary.m_kedRefFile = a_file;
}

void KAnalysisWorker::set_XRF_sample_file(const QString& a_file)
{
  m_resultsSummary.m_xrfSampleFile = a_file;
}

void KAnalysisWorker::set_XRF_bkgd_file(const QString& a_file)
{
  m_resultsSummary.m_xrfBkgdFile = a_file;
}

void KAnalysisWorker::set_XRF_ref_file(const QString& a_file)
{
  m_resultsSummary.m_xrfRefFile = a_file;
}

void KAnalysisWorker::set_param_set(const QString& a_paramSet)
{
  m_resultsSummary.m_paramSet = a_paramSet;
}

void KAnalysisWorker::set_ana_mode(const int a_anaMode)
{
  m_resultsSummary.m_anaMode = a_anaMode;
}

void KAnalysisWorker::set_KED_mode(const int a_KEDMode)
{
  m_resultsSummary.m_KEDMode = a_KEDMode;
}

void KAnalysisWorker::do_analysis()
{
  if(m_doKED == true)
  {
    bool KEDAnaSuccess = false;
    KKEDEngine kedEngine;

    //Receive the KED results from the KED Engine
    connect(&kedEngine, &KKEDEngine::send_KED_results, 
            this, &KAnalysisWorker::receive_KED_results);

    emit update_status("Doing KED Analysis");
    if(m_doFittingAna == true)
    {
      emit update_status("Doing KED Spectral Fitting");
      KEDAnaSuccess = kedEngine.fitting_analysis(m_sampTemp);
    }
    if(m_doROIAna == true)
    {
      emit update_status("Doing KED 2-ROI Analysis");
      KEDAnaSuccess = kedEngine.two_roi_analysis();
    }

    if(KEDAnaSuccess)
    {
      //Stitch the KED and XRF results 
      process_results();
      emit ana_success("KED Analysis Complete!");
    }
    else
    {
      emit ana_fail("KED Analysis Failed!");
    }
  }

  if(m_doXRF == true)
  {
    bool XRFAnaSuccess = false;
    KXRFEngine xrfEngine;

    //Receive the XRF results from the XRF Engine
    connect(&xrfEngine, &KXRFEngine::send_XRF_results, 
            this, &KAnalysisWorker::receive_XRF_results);

    emit update_status("Doing XRF Spectral Fitting");
    XRFAnaSuccess = xrfEngine.fitting_analysis();

    if(XRFAnaSuccess)
    {
      //Stitch the KED and XRF results 
      process_results();
      emit ana_success("XRF Analysis Complete!");
    }
    else
    {
      emit ana_fail("XRF Analysis Failed!");
    }
  }



  // Tell the dialog that we are done (so it can quit the thread)
  emit finished();
}

void KAnalysisWorker::process_results()
{
  //do work on ked and xrf results
    //
    //code here
    //

  //now emit the final results for printing in GUI
  if (m_kedResults) 
  {
    //calculate the residuals and add to ked results
    std::vector<double> residualX;
    std::vector<double> residualY = KMath::calculate_normalized_residuals<KKEDResults>(m_kedResults.value(),
                                                                                residualX);
    m_kedResults->m_residualsX = residualX;
    m_kedResults->m_residualsY = residualY;

    //For now we aren't doing any math and just setting final equal to KED fit
    //value
    m_resultsSummary.m_uConcentration  = m_kedResults->m_uConcentration;
    m_resultsSummary.m_npConcentration = m_kedResults->m_npConcentration;
    m_resultsSummary.m_puConcentration = m_kedResults->m_puConcentration;
    m_resultsSummary.m_amConcentration = m_kedResults->m_amConcentration;
    m_resultsSummary.m_cmConcentration = m_kedResults->m_cmConcentration;
    //sigma
    m_resultsSummary.m_uSigma  = m_kedResults->m_uSigma;
    m_resultsSummary.m_npSigma = m_kedResults->m_npSigma;
    m_resultsSummary.m_puSigma = m_kedResults->m_puSigma;
    m_resultsSummary.m_amSigma = m_kedResults->m_amSigma;
    m_resultsSummary.m_cmSigma = m_kedResults->m_cmSigma;
    //RSD
    m_resultsSummary.m_uRSD  = m_kedResults->m_uRSD;
    m_resultsSummary.m_npRSD = m_kedResults->m_npRSD;
    m_resultsSummary.m_puRSD = m_kedResults->m_puRSD;
    m_resultsSummary.m_amRSD = m_kedResults->m_amRSD;
    m_resultsSummary.m_cmRSD = m_kedResults->m_cmRSD;

    //Send KED results to the Results Manager
    connect(this, &KAnalysisWorker::send_KED_results, 
            &KResultsManager::getInstance(), &KResultsManager::receive_KED_results);
    emit send_KED_results(m_kedResults.value());
  }

  if (m_xrfResults) 
  {
    //calculate the residuals and add to XRF results
    std::vector<double> residualX;
    std::vector<double> residualY = KMath::calculate_normalized_residuals<KXRFResults>(m_xrfResults.value(),
                                                                                residualX);
    m_xrfResults->m_residualsX = residualX;
    m_xrfResults->m_residualsY = residualY;

    //Send XRF results to the Results Manager
    connect(this, &KAnalysisWorker::send_XRF_results, 
            &KResultsManager::getInstance(), &KResultsManager::receive_XRF_results);
    emit send_XRF_results(m_xrfResults.value());
  }

  //Send final results to the Results Manager
  connect(this, &KAnalysisWorker::send_results_summary, 
          &KResultsManager::getInstance(), &KResultsManager::receive_results_summary);
  emit send_results_summary(m_resultsSummary);
}


void KAnalysisWorker::receive_KED_results(const KKEDResults& a_kedResults) 
{
  m_kedResults = a_kedResults;
}

void KAnalysisWorker::receive_XRF_results(const KXRFResults& a_xrfResults) 
{
  m_xrfResults = a_xrfResults;
}