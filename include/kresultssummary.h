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

#ifndef KRESULTSSUMMARY_H
#define KRESULTSSUMMARY_H

//Qt includes
#include <QString>
#include <QDateTime>
//Project includes
#include "kdevlogger.h"
//C++ includes
#include <string>

/**
 * POD Class (Struct) to hold final analysis results
 **/
struct KResultsSummary
{
public:
   KResultsSummary();
  ~KResultsSummary();

  void clear_data();

  //header info
  QString m_operator;
  QString m_facility;
  QString m_comment;
  QDateTime m_date;

  //file info
  QString m_kedSampleFile;
  QString m_kedBkgdFile;
  QString m_kedRefFile;
  QString m_xrfSampleFile;
  QString m_xrfBkgdFile;
  QString m_xrfRefFile;

  //Analysis options
  QString m_paramSet;
  double m_sampTemp;
  int m_anaMode;   //0=hybrid, 1=KED only, 2=XRf only, 3=no analysis
  int m_KEDMode;  //0=fitting, 1=Two-ROI, 2=unkown
  int m_XRFMode;  //0=Normal, 1=Low concentration, 2=unkown
  
  //results
  double m_uConcentration{0.};
  double m_uSigma{0.};
  double m_uRSD{0.};
  double m_npConcentration{0.};
  double m_npSigma{0.};
  double m_npRSD{0.};
  double m_puConcentration{0.};
  double m_puSigma{0.};
  double m_puRSD{0.};
  double m_amConcentration{0.};
  double m_amSigma{0.};
  double m_amRSD{0.};
  double m_cmConcentration{0.};
  double m_cmSigma{0.};
  double m_cmRSD{0.};

private:

};

#endif 