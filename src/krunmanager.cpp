#include "krunmanager.h"

KRunManager::KRunManager()
{
  KDevLogger::getInstance().logEvent("Singleton KRunManager object constructed");
  KFileManager& fileManager = KFileManager::getInstance();
  KKEDPlotManager& kedPlotManager = KKEDPlotManager::getInstance();
  KXRFPlotManager& xrfPlotManager = KXRFPlotManager::getInstance();
  KResultsManager& resultsManager = KResultsManager::getInstance();
}

KRunManager::~KRunManager()
{
  KDevLogger::getInstance().logEvent("Singleton KRunManager object destructed");
}

KRunManager& KRunManager::getInstance()
{
  static KRunManager instance;
  return instance;
}


void KRunManager::send(int a_message)
{
}

void KRunManager::receive(int a_message)
{
}

void KRunManager::loadSpectrum(KSpectralData& a_dataset)
{
  //Instruct KFileManager to load the data into memory
  KFileManager::getInstance().load_with_SpecUtils(a_dataset);
}

void KRunManager::cal_XRF_success()
{
  emit send_cal_XRF_success();
}