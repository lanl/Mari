#include "kceresresults.h"

KCeresResults::KCeresResults()
{
  KDevLogger::getInstance().logEvent("KCeresResults object constructed");
}

KCeresResults::~KCeresResults()
{
  KDevLogger::getInstance().logEvent("KCeresResults object destructed");
}

void KCeresResults::clear_data() 
{
}