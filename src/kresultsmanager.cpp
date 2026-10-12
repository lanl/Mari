#include "kresultsmanager.h"

KResultsManager::KResultsManager()
{
  KDevLogger::getInstance().logEvent("Singleton KResultsManager object constructed");
}

KResultsManager::~KResultsManager()
{
  KDevLogger::getInstance().logEvent("Singleton KResultsManager object destructed");
}


KResultsManager& KResultsManager::getInstance()
{
  static KResultsManager instance;
  return instance;
}

void KResultsManager::send(int a_message)
{
}

void KResultsManager::receive(int a_message)
{
}


//----------Public Slots--------------------------------
void KResultsManager::receive_KED_results(const KKEDResults& a_KEDResults) 
{
  emit send_KED_results(a_KEDResults);
}

void KResultsManager::receive_XRF_results(const KXRFResults& a_XRFResults) 
{
  emit send_XRF_results(a_XRFResults);
}

void KResultsManager::receive_KED_calibration_results(const KCalibrateKEDResults& a_KEDCalResults) 
{
  m_KEDCalResults = a_KEDCalResults;
}

void KResultsManager::receive_XRF_calibration_results(const KCalibrateXRFResults& a_XRFCalResults) 
{
  m_XRFCalResults = a_XRFCalResults;
}

void KResultsManager::receive_results_summary(const KResultsSummary& a_resultsSummary) 
{
  emit send_results_summary(a_resultsSummary);
}