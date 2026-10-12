#include "kspectraldata.h"

void KSpectralData::clear_data() 
{
  m_filename.clear();
  m_filenameAndPath.clear();
  m_counts.clear();
  m_channels.clear();
  m_energies.clear();
  m_loadMe = false;
  m_isLoaded = false;
}