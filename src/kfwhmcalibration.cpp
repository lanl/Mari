#include "kfwhmcalibration.h"

//#define MYMANAGER KFileManager::getInstance()

KFWHMCalibration::KFWHMCalibration()
{
  KDevLogger::getInstance().logEvent("KFWHMCalibration object constructed");
}

KFWHMCalibration::~KFWHMCalibration()
{
  KDevLogger::getInstance().logEvent("KFWHMCalibration object destructed");
}

void KFWHMCalibration::send(int a_message)
{
}

void KFWHMCalibration::receive(int a_message)
{
}


double KFWHMCalibration::calc_fwhm(double a_energy, double a_A1, double a_A2, double a_A3)
{
  return std::sqrt(a_A1 + a_A2*a_A2 + a_A3/a_energy);
}