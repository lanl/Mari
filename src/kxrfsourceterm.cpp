#include "kxrfsourceterm.h"

KXRFSourceTerm::KXRFSourceTerm(double a_energy_keV, 
                               double a_counts)
: x_(a_energy_keV), y_(a_counts)
{
}

KXRFSourceTerm::~KXRFSourceTerm()
{
  //nothing to do;
}