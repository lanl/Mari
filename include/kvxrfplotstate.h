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

#ifndef KVXRFPLOTSTATE_H
#define KVXRFPLOTSTATE_H

//Project includes
#include "kfilemanager.h"
#include "kmath.h"
//External includes
#include "qcustomplot.h"
//C++ includes

/**
 * Abstract State Pattern interface class 
 **/
class KVXRFPlotState
{
public:
/**
 * Default constructor.
 **/
  KVXRFPlotState();

/**
 * Default destructor.
 **/
  virtual ~KVXRFPlotState();

/**
 * Implements state behavior
 **/
  virtual void apply(QCustomPlot* xrfPlot, int  a_sampColor, 
                                           int  a_bkgdColor, 
                                           int  a_refColor,
                                           bool a_sampDraw, 
                                           bool a_bkgdDraw, 
                                           bool a_refDraw);

protected:
  double m_xScaler = 1.0;
  double m_yScaler = 1.1;
  std::map<int, QPen> m_colorMap;
};


/**
 * Concrete implementation of abstract plot state.
 * State = no graphs
 * State number = 0
 **/
class KXRFDefaultPlotState : public KVXRFPlotState
{
public:
/**
 * Default constructor.
 **/
  KXRFDefaultPlotState();

/**
 * Default destructor.
 **/
  ~KXRFDefaultPlotState() override;

/**
 * Override of base class virtual method
 * Implements state behavior.
 **/
  virtual void apply(QCustomPlot* xrfPlot, int  a_sampColor, 
                                           int  a_bkgdColor, 
                                           int  a_refColor,
                                           bool a_sampDraw, 
                                           bool a_bkgdDraw, 
                                           bool a_refDraw) override;

private:
};


/**
 * Concrete implementation of abstract plot state.
 * State = Sample graph only
 * State number = 4
 **/
class KXRFSampPlotState : public KVXRFPlotState
{
public:
/**
 * Default constructor.
 **/
  KXRFSampPlotState();

/**
 * Default destructor.
 **/
  ~KXRFSampPlotState() override;

/**
 * Override of base class virtual method
 * Implements state behavior.
 **/
  virtual void apply(QCustomPlot* xrfPlot, int  a_sampColor, 
                                           int  a_bkgdColor, 
                                           int  a_refColor,
                                           bool a_sampDraw, 
                                           bool a_bkgdDraw, 
                                           bool a_refDraw) override;

private:
  QVector<double> m_qSampCounts;
  QVector<double> m_qSampChannels;
  QVector<double> m_qSampEnergies;
};


/**
 * Concrete implementation of abstract plot state.
 * State = Background graph only
 * State number = 2
 **/
class KXRFBkgdPlotState : public KVXRFPlotState
{
public:
/**
 * Default constructor.
 **/
  KXRFBkgdPlotState();

/**
 * Default destructor.
 **/
  ~KXRFBkgdPlotState() override;

/**
 * Override of base class virtual method
 * Implements state behavior.
 **/
  virtual void apply(QCustomPlot* xrfPlot, int  a_sampColor, 
                                           int  a_bkgdColor, 
                                           int  a_refColor,
                                           bool a_sampDraw, 
                                           bool a_bkgdDraw, 
                                           bool a_refDraw) override;

private:
  QVector<double> m_qBkgdCounts;
  QVector<double> m_qBkgdChannels;
  QVector<double> m_qBkgdEnergies;
};


/**
 * Concrete implementation of abstract plot state.
 * State = Reference graph only
 * State number = 1
 **/
class KXRFRefPlotState : public KVXRFPlotState
{
public:
/**
 * Default constructor.
 **/
  KXRFRefPlotState();

/**
 * Default destructor.
 **/
  ~KXRFRefPlotState() override;

/**
 * Override of base class virtual method
 * Implements state behavior.
 **/
  virtual void apply(QCustomPlot* xrfPlot, int  a_sampColor, 
                                           int  a_bkgdColor, 
                                           int  a_refColor,
                                           bool a_sampDraw, 
                                           bool a_bkgdDraw, 
                                           bool a_refDraw) override;

private:
  QVector<double> m_qRefCounts;
  QVector<double> m_qRefChannels;
  QVector<double> m_qRefEnergies;
};


/**
 * Concrete implementation of abstract plot state.
 * State = Sample and background graphs
 * State number = 6
 **/
class KXRFSampBkgdPlotState : public KVXRFPlotState
{
public:
/**
 * Default constructor.
 **/
  KXRFSampBkgdPlotState();

/**
 * Default destructor.
 **/
  ~KXRFSampBkgdPlotState() override;

/**
 * Override of base class virtual method
 * Implements state behavior.
 **/
  virtual void apply(QCustomPlot* xrfPlot, int  a_sampColor, 
                                           int  a_bkgdColor, 
                                           int  a_refColor,
                                           bool a_sampDraw, 
                                           bool a_bkgdDraw, 
                                           bool a_refDraw) override;

private:
  QVector<double> m_qSampCounts;
  QVector<double> m_qSampChannels;
  QVector<double> m_qSampEnergies;
  QVector<double> m_qBkgdCounts;
  QVector<double> m_qBkgdChannels;
  QVector<double> m_qBkgdEnergies;
};


/**
 * Concrete implementation of abstract plot state.
 * State = Sample and reference graphs
 * State number = 5
 **/
class KXRFSampRefPlotState : public KVXRFPlotState
{
public:
/**
 * Default constructor.
 **/
  KXRFSampRefPlotState();

/**
 * Default destructor.
 **/
  ~KXRFSampRefPlotState() override;

/**
 * Override of base class virtual method
 * Implements state behavior.
 **/
  virtual void apply(QCustomPlot* xrfPlot, int  a_sampColor, 
                                           int  a_bkgdColor, 
                                           int  a_refColor,
                                           bool a_sampDraw, 
                                           bool a_bkgdDraw, 
                                           bool a_refDraw) override;

protected:


private:
  QVector<double> m_qSampCounts;
  QVector<double> m_qSampChannels;
  QVector<double> m_qSampEnergies;
  QVector<double> m_qRefCounts;
  QVector<double> m_qRefChannels;
  QVector<double> m_qRefEnergies;
};


/**
 * Concrete implementation of abstract plot state.
 * State = Background and reference graphs
 * State number = 3
 **/
class KXRFBkgdRefPlotState : public KVXRFPlotState
{
public:
/**
 * Default constructor.
 **/
  KXRFBkgdRefPlotState();

/**
 * Default destructor.
 **/
  ~KXRFBkgdRefPlotState() override;

/**
 * Override of base class virtual method
 * Implements state behavior.
 **/
  virtual void apply(QCustomPlot* xrfPlot, int  a_sampColor, 
                                           int  a_bkgdColor, 
                                           int  a_refColor,
                                           bool a_sampDraw, 
                                           bool a_bkgdDraw, 
                                           bool a_refDraw) override;

private:
  QVector<double> m_qBkgdCounts;
  QVector<double> m_qBkgdChannels;
  QVector<double> m_qBkgdEnergies;
  QVector<double> m_qRefCounts;
  QVector<double> m_qRefChannels;
  QVector<double> m_qRefEnergies;
};


/**
 * Concrete implementation of abstract plot state.
 * State = All three graphs
 * State number = 7
 **/
class KXRFAllPlotState : public KVXRFPlotState
{
public:
/**
 * Default constructor.
 **/
  KXRFAllPlotState();

/**
 * Default destructor.
 **/
  ~KXRFAllPlotState() override;

/**
 * Override of base class virtual method
 * Implements state behavior.
 **/
  virtual void apply(QCustomPlot* xrfPlot, int  a_sampColor, 
                                           int  a_bkgdColor, 
                                           int  a_refColor,
                                           bool a_sampDraw, 
                                           bool a_bkgdDraw, 
                                           bool a_refDraw) override;

private:
  QVector<double> m_qSampCounts;
  QVector<double> m_qSampChannels;
  QVector<double> m_qSampEnergies;
  QVector<double> m_qBkgdCounts;
  QVector<double> m_qBkgdChannels;
  QVector<double> m_qBkgdEnergies;
  QVector<double> m_qRefCounts;
  QVector<double> m_qRefChannels;
  QVector<double> m_qRefEnergies;
};

#endif 

