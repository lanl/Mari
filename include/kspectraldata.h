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

#ifndef KSPECTRALDATA_H
#define KSPECTRALDATA_H

//External includes
#include "SpecUtils/SpecFile.h"
#include "SpecUtils/EnergyCalibration.h"
//C++ includes
#include <memory>
#include <numeric>

class KSpectralData
{
public:
  void clear_data();

  std::string m_filenameAndPath;
  std::string m_filename;
  std::shared_ptr<const SpecUtils::Measurement> m_measurement;
  std::vector<double> m_counts;
  std::vector<double> m_originalCounts;
  std::vector<size_t> m_channels;
  std::vector<double> m_energies;
  std::vector<float> m_coeffs;
  bool m_loadMe = false;
  bool m_isLoaded = false;
};

#endif 