#include "kenergycalibration.h"

//#define MYMANAGER KFileManager::getInstance()

KEnergyCalibration::KEnergyCalibration()
{
  KDevLogger::getInstance().logEvent("KEnergyCalibration object constructed");
}

KEnergyCalibration::~KEnergyCalibration()
{
  KDevLogger::getInstance().logEvent("KEnergyCalibration object destructed");
}

void KEnergyCalibration::send(int a_message)
{
  //MYMANAGER.m_message = a_message;
}

void KEnergyCalibration::receive(int a_message)
{
}

std::array<float,3> KEnergyCalibration::get_energy_calibration(KSpectralData& a_dataset)
{
  std::array<float,3> eCalCoeffs = {0.,0.,0.};
  eCalCoeffs.at(0) = a_dataset.m_coeffs.at(0);
  eCalCoeffs.at(1) = a_dataset.m_coeffs.at(1);
  eCalCoeffs.at(2) = a_dataset.m_coeffs.at(2);
  return eCalCoeffs;
}

void KEnergyCalibration::set_energy_calibration(KSpectralData& a_dataset, 
                                                const std::array<float,3> a_newCoeffs)
{
  a_dataset.m_coeffs.at(0) = a_newCoeffs.at(0);
  a_dataset.m_coeffs.at(1) = a_newCoeffs.at(1);
  a_dataset.m_coeffs.at(2) = a_newCoeffs.at(2);
}

std::array<float,3> KEnergyCalibration::calibrate_from_mouse_drag(KSpectralData& a_dataset, 
                                                                  const float a_peakLocActual, 
                                                                  const float a_peakLocDesired)
{
  std::array<float,3> eCalCoeffs = get_energy_calibration(a_dataset);
  float slopeInitial = eCalCoeffs.at(1);
  float slopeRatio = a_peakLocActual/a_peakLocDesired;
  float slopeFinal = slopeInitial/slopeRatio;
  eCalCoeffs.at(1) = slopeFinal;//change the slope in the array
  set_energy_calibration(a_dataset, eCalCoeffs);//change slope in memory
  update_energy_vec(a_dataset);
  return eCalCoeffs;
}

void KEnergyCalibration::calibrate_from_box(KSpectralData& a_dataset, 
                                            const int a_param, 
                                            const float a_newValue)
{
  if(a_param < 0 || a_param > 2)
  {
    KDevLogger::getInstance().logEvent("ERROR: Tried to set energy calibration for something other than offset, linear, or quadratic");
    return;
  }
  std::array<float,3> eCalCoeffs = get_energy_calibration(a_dataset);
  for(int i=0; i<3; i++)
  {
    if(i == a_param)
    {
      eCalCoeffs.at(i) = a_newValue;
    } 
  }
  set_energy_calibration(a_dataset, eCalCoeffs);
  update_energy_vec(a_dataset);
}

void KEnergyCalibration::update_energy_vec(KSpectralData& a_dataset)
{
  a_dataset.m_energies.clear();
  for(size_t i=0; i<a_dataset.m_measurement->num_gamma_channels(); ++i)
  {
    double tempEnergy = a_dataset.m_coeffs.at(0) + a_dataset.m_coeffs.at(1)*i + a_dataset.m_coeffs.at(2)*i*i;
    a_dataset.m_energies.push_back(tempEnergy);
  }
}//maybe pass passed/failed as a bool by reference from manger 