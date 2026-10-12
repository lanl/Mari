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

#ifndef KVKEDPLOTSTATE_H
#define KVKEDPLOTSTATE_H

//Project includes
#include "kfilemanager.h"
#include "kmath.h"
//External includes
#include "qcustomplot.h"
//C++ includes

/**
 * Abstract State Pattern interface class 
 **/
class KVKEDPlotState: public QObject
{
  Q_OBJECT; //call Q_OBJECT meta object macro

public:
/**
 * Default constructor.
 **/
  KVKEDPlotState();

/**
 * Default destructor.
 **/
  virtual ~KVKEDPlotState();

/**
 * Implements state behavior
 **/
  virtual void apply(QCustomPlot* kedPlot, int  a_sampColor, 
                                           int  a_bkgdColor, 
                                           int  a_refColor,
                                           bool a_sampDraw, 
                                           bool a_bkgdDraw, 
                                           bool a_refDraw,
                                           bool a_isFitted=false);

protected:
  double m_xScaler = 1.0;
  double m_yScaler = 1.1;
  std::map<int, QPen> m_colorMap;

signals:
  void plot_fit_too();
};


/**
 * Concrete implementation of abstract plot state.
 * State = no graphs
 * State number = 0
 **/
class KKEDDefaultPlotState : public KVKEDPlotState
{
public:
/**
 * Default constructor.
 **/
  KKEDDefaultPlotState();

/**
 * Default destructor.
 **/
  ~KKEDDefaultPlotState() override;

/**
 * Override of base class virtual method
 * Implements state behavior.
 **/
  virtual void apply(QCustomPlot* kedPlot, int  a_sampColor, 
                                           int  a_bkgdColor, 
                                           int  a_refColor,
                                           bool a_sampDraw, 
                                           bool a_bkgdDraw, 
                                           bool a_refDraw,
                                           bool a_isFitted=false) override;

private:
};


/**
 * Concrete implementation of abstract plot state.
 * State = Sample graph only
 * State number = 4
 **/
class KKEDSampPlotState : public KVKEDPlotState
{
public:
/**
 * Default constructor.
 **/
  KKEDSampPlotState();

/**
 * Default destructor.
 **/
  ~KKEDSampPlotState() override;

/**
 * Override of base class virtual method
 * Implements state behavior.
 **/
  virtual void apply(QCustomPlot* kedPlot, int  a_sampColor, 
                                           int  a_bkgdColor, 
                                           int  a_refColor,
                                           bool a_sampDraw, 
                                           bool a_bkgdDraw, 
                                           bool a_refDraw,
                                           bool a_isFitted=false) override;

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
class KKEDBkgdPlotState : public KVKEDPlotState
{
public:
/**
 * Default constructor.
 **/
  KKEDBkgdPlotState();

/**
 * Default destructor.
 **/
  ~KKEDBkgdPlotState() override;

/**
 * Override of base class virtual method
 * Implements state behavior.
 **/
  virtual void apply(QCustomPlot* kedPlot, int  a_sampColor, 
                                           int  a_bkgdColor, 
                                           int  a_refColor,
                                           bool a_sampDraw, 
                                           bool a_bkgdDraw, 
                                           bool a_refDraw,
                                           bool a_isFitted=false) override;

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
class KKEDRefPlotState : public KVKEDPlotState
{
public:
/**
 * Default constructor.
 **/
  KKEDRefPlotState();

/**
 * Default destructor.
 **/
  ~KKEDRefPlotState() override;

/**
 * Override of base class virtual method
 * Implements state behavior.
 **/
  virtual void apply(QCustomPlot* kedPlot, int  a_sampColor, 
                                           int  a_bkgdColor, 
                                           int  a_refColor,
                                           bool a_sampDraw, 
                                           bool a_bkgdDraw, 
                                           bool a_refDraw,
                                           bool a_isFitted=false) override;

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
class KKEDSampBkgdPlotState : public KVKEDPlotState
{
public:
/**
 * Default constructor.
 **/
  KKEDSampBkgdPlotState();

/**
 * Default destructor.
 **/
  ~KKEDSampBkgdPlotState() override;

/**
 * Override of base class virtual method
 * Implements state behavior.
 **/
  virtual void apply(QCustomPlot* kedPlot, int  a_sampColor, 
                                           int  a_bkgdColor, 
                                           int  a_refColor,
                                           bool a_sampDraw, 
                                           bool a_bkgdDraw, 
                                           bool a_refDraw,
                                           bool a_isFitted=false) override;

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
class KKEDSampRefPlotState : public KVKEDPlotState
{
public:
/**
 * Default constructor.
 **/
  KKEDSampRefPlotState();

/**
 * Default destructor.
 **/
  ~KKEDSampRefPlotState() override;

/**
 * Override of base class virtual method
 * Implements state behavior.
 **/
  virtual void apply(QCustomPlot* kedPlot, int  a_sampColor, 
                                           int  a_bkgdColor, 
                                           int  a_refColor,
                                           bool a_sampDraw, 
                                           bool a_bkgdDraw, 
                                           bool a_refDraw,
                                           bool a_isFitted=false) override;

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
class KKEDBkgdRefPlotState : public KVKEDPlotState
{
public:
/**
 * Default constructor.
 **/
  KKEDBkgdRefPlotState();

/**
 * Default destructor.
 **/
  ~KKEDBkgdRefPlotState() override;

/**
 * Override of base class virtual method
 * Implements state behavior.
 **/
  virtual void apply(QCustomPlot* kedPlot, int  a_sampColor, 
                                           int  a_bkgdColor, 
                                           int  a_refColor,
                                           bool a_sampDraw, 
                                           bool a_bkgdDraw, 
                                           bool a_refDraw,
                                           bool a_isFitted=false) override;

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
class KKEDAllPlotState : public KVKEDPlotState
{
public:
/**
 * Default constructor.
 **/
  KKEDAllPlotState();

/**
 * Default destructor.
 **/
  ~KKEDAllPlotState() override;

/**
 * Override of base class virtual method
 * Implements state behavior.
 **/
  virtual void apply(QCustomPlot* kedPlot, int  a_sampColor, 
                                           int  a_bkgdColor, 
                                           int  a_refColor,
                                           bool a_sampDraw, 
                                           bool a_bkgdDraw, 
                                           bool a_refDraw,
                                           bool a_isFitted=false) override;

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
