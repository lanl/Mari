#include "kcalxrfsourceterm.h"

KCalXRFSourceTerm::KCalXRFSourceTerm(double a_energy_keV, 
                               double a_counts)
: x_(a_energy_keV), y_(a_counts)
{
}

KCalXRFSourceTerm::~KCalXRFSourceTerm()
{
  //nothing to do;
}