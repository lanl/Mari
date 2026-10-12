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

#ifndef KXRFPLOTMANAGER_H
#define KXRFPLOTMANAGER_H

//Qt includes
#include <QtCore/QObject>
//Project includes
#include "kvmanager.h"
#include "kvxrfplotstate.h"
//External includes
//C++ includes


/**
 * State pattern context class for XRF plotter 
 **/
class KXRFPlotManager : public QObject, public KVManager
{
  Q_OBJECT; //call Q_OBJECT meta object macro
  
public:
  /**
   * Global access getter for the single instance
   * Meyer's Singleton
   **/
  static KXRFPlotManager& getInstance();

  /**
   * Need to delete the copy constructor and assignment operators
   * to prevent copying
   **/
  KXRFPlotManager(const KXRFPlotManager&) = delete;
  KXRFPlotManager& operator=(const KXRFPlotManager&) = delete;

  /**
   * Obey abstract base class contract and override pure virtual functions
   **/
  virtual void send(int a_message) override;
  virtual void receive(int a_message) override;

  /**
   * Sets the context class (this) state member to to correct 
   * concrete state implementation
   **/
  void setState(int a_stateNumber);

  /**
   * Calls correct concrete apply method.
   * Accepts main window plot widget as pointer instead of
   * using signals/slots. SHOULD THIS BE CHANGED?
   **/
  void applyState(QCustomPlot* xrfPlot);

  /**
   * Factory function to create concrete state objects based on stateNumber
   **/
  KVXRFPlotState* createState(int a_stateNumber);

  KVXRFPlotState* get_plot_state() const;

public slots:
  void set_widget_status(int a_sampColor, int a_bkgdColor, int a_refColor,
                         bool a_sampDraw, bool a_bkgdDraw, bool a_refDraw);

private:
  /**
   * Default constructor.
   **/
  KXRFPlotManager();

  /**
   * Default destructor.
   **/
  ~KXRFPlotManager();

  KVXRFPlotState* m_plotState;

  int m_sampColor, m_bkgdColor, m_refColor;

  bool m_sampDraw, m_bkgdDraw, m_refDraw;

signals:
  void updateStateFields();

};

#endif 
