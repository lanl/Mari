#include "kxrfresults.h"

KXRFResults::KXRFResults() : 
  m_degreesOfFreedom(0),
  m_chiSquared(0.0),
  m_reducedChiSquared(0.0)
{
  KDevLogger::getInstance().logEvent("KXRFResults object constructed");
}

KXRFResults::~KXRFResults()
{
  KDevLogger::getInstance().logEvent("KXRFResults object destructed");
}

void KXRFResults::clear_data() 
{
}