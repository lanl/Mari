#include "kcalkedsourceterm.h"

KCalKEDSourceTerm::KCalKEDSourceTerm(double a_energy_keV, 
                               double a_counts,
                               double a_continuum)
: x_(a_energy_keV), y_(a_counts), c_(a_continuum)
{
}

KCalKEDSourceTerm::~KCalKEDSourceTerm()
{
  //nothing to do;
}