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

#ifndef TEST_MAINWINDOW_H
#define TEST_MAINWINDOW_H

//Qt includes
#include <QtTest/QtTest>
#include <QObject>
#include <QApplication>
//Project includes
#include "mainwindow.h"

/**
 * Unit test module for the mainwindow. 
 * Includes things like widget heirachies
 **/
class Test_MainWindow : public QObject 
{
  Q_OBJECT

private slots:
  /**
   * Data structure for the topLevelWidget test.
   * Uses Qt's data-driven matrix format.
   **/
  void topLevelWidgets_data();

  /**
   * Test to make sure all widgets will get automaticlly deleted by
   * Qt's garbage management. 
   * Does this by checking that there are no rogue widgets without a parent 
   * (except for the ones that normally won't).
   **/
  void topLevelWidgets();
};

#endif 