#include "kkedresults.h"

KKEDResults::KKEDResults()
{
  KDevLogger::getInstance().logEvent("KKEDResults object constructed");
}

KKEDResults::~KKEDResults()
{
  KDevLogger::getInstance().logEvent("KKEDResults object destructed");
}

void KKEDResults::clear_data() 
{
}