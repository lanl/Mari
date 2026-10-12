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

#ifndef KRUNMANAGER_H
#define KRUNMANAGER_H

//Project includes
#include "kfilemanager.h"
#include "kkedplotmanager.h"
#include "kxrfplotmanager.h"
#include "kresultsmanager.h"
//Qt includes
#include <QObject>
//External includes
//C++ includes


/**
 * Primary manager class. Handles all backend processes.
 * Acts as an interface between backend and frontend.
 * Singleton to prevent multiple instances.
 **/
class KRunManager : public QObject, public KVManager
{
   Q_OBJECT //call Q_OBJECT meta object macro

public:
  /**
   * Global access getter for the single instance
   * Meyer's Singleton
   **/
  static KRunManager& getInstance();

  /**
   * Need to delete the copy constructor and assignment operators
   * to prevent copying
   **/
  KRunManager(const KRunManager&) = delete;
  KRunManager& operator=(const KRunManager&) = delete;

  /**
   * Obey abstract base class contract and override pure virtual functions
   **/
  virtual void send(int a_message) override;
  virtual void receive(int a_message) override;


private:
  /**
   * Default constructor. Private for Singleton
   **/
  KRunManager();

  /**
   * Default destructor.
   **/
  ~KRunManager();

//---Qt signals and slots---/
signals:
  //void spectraLoaded(KSpectralData& a_dataset);
  void send_cal_XRF_success();

public slots:
  /**
   * Intermediary function to pass load spectrum call from dialog box and
   * pass it to KFileManager spectrum loading method
   **/ 
  void loadSpectrum(KSpectralData& a_dataset);

  /**
   * Recieves that the XRF instrument calibration passed and sends this info to
   * MainWindow
   **/
  void cal_XRF_success();
};

#endif 
