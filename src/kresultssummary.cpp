#include "kresultssummary.h"

KResultsSummary::KResultsSummary()
{
  KDevLogger::getInstance().logEvent("KResultsSummary object constructed");
}

KResultsSummary::~KResultsSummary()
{
  KDevLogger::getInstance().logEvent("KResultsSummary object destructed");
}

void KResultsSummary::clear_data() 
{
  m_operator.clear();
  m_facility.clear();
  m_comment.clear();

  m_kedSampleFile.clear();
  m_kedBkgdFile.clear();
  m_kedRefFile.clear();
  m_xrfSampleFile.clear();
  m_xrfBkgdFile.clear();
  m_xrfRefFile.clear();

  m_paramSet.clear();
  m_sampTemp = 25.;
  m_uConcentration = 0.;
  m_uSigma = 0.;
  m_uRSD = 0.;
  m_npConcentration = 0.;
  m_npSigma = 0.;
  m_npRSD = 0.;
  m_puConcentration = 0.;
  m_puSigma = 0.;
  m_puRSD = 0.;
  m_amConcentration = 0.;
  m_amSigma = 0.;
  m_amRSD = 0.;
  m_cmConcentration = 0.;
  m_cmSigma = 0.;
  m_cmRSD = 0.;

}