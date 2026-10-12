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

#ifndef CALIBRATEXRFRESULTSDIALOG_H
#define CALIBRATEXRFRESULTSDIALOG_H

//Qt includes
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFontMetrics>
#include <QTabWidget>
//External includes
#include "qcustomplot.h"
//Project Qt includes
#include "okbutton.h"
#include "cancelbutton.h"
#include "questionbutton.h"
#include "updatablelabel.h"
#include "nonupdatablelabel.h"
#include "custommsgbox.h"
//Project includes
#include "kfilemanager.h"
#include "kresultsmanager.h"
//C++ includes
#include <string>

/**
 * Class for building the XRF 'Calibrate' dialog box
 **/
class CalibrateXRFResultsDialog : public QDialog
{
  Q_OBJECT

public:
  explicit CalibrateXRFResultsDialog(QWidget* parent = nullptr);
 
  ~CalibrateXRFResultsDialog();
  
  /**
   * Plots the curve fit
   **/
  void plot_fit();

  /**
   * Displays the Ceres Solver solution
   **/
  void display_solver_results();

private:
  QWidget* layout_plot_tab();
  QWidget* layout_results_tab();

  /**
   * Save the calibration fit results to the laoded parameter set
   **/
  void save_cal_values();
  
  //Various widgets.
  //These are only the ones that need to be accessed at some point after creation.
  QTabWidget* m_mainTabWidget;

  QCustomPlot* m_XRFPlot;
  QCustomPlot* m_residualsPlot;

  NonUpdatableLabel* m_algoField;
  QuestionButton*    m_algoAndConvButton;
  NonUpdatableLabel* m_terminationField;
  NonUpdatableLabel* m_msgField;
  QuestionButton*    m_chiSqButton;
  NonUpdatableLabel* m_chiSqField;
  QuestionButton*    m_initCostButton;
  NonUpdatableLabel* m_initCostField;
  QuestionButton*    m_finalCostButton;
  NonUpdatableLabel* m_finalCostField;
  QuestionButton*    m_stepSuccessButton;
  NonUpdatableLabel* m_stepSuccessField;
  QuestionButton*    m_gradMaxButton;
  NonUpdatableLabel* m_gradMaxField;
  QuestionButton*    m_relDecreaseButton;
  NonUpdatableLabel* m_relDecreaseField;
  QuestionButton*    m_stepNormButton;
  NonUpdatableLabel* m_stepNormField;
  NonUpdatableLabel* m_CField;
  NonUpdatableLabel* m_B3Field;
  NonUpdatableLabel* m_B4Field;
  NonUpdatableLabel* m_B5Field;
  NonUpdatableLabel* m_B6Field;
  NonUpdatableLabel* m_B7Field;
  NonUpdatableLabel* m_B8Field;
  NonUpdatableLabel* m_B9Field;
  NonUpdatableLabel* m_B10Field;
  NonUpdatableLabel* m_B11Field;
  NonUpdatableLabel* m_B12Field;
  NonUpdatableLabel* m_B13Field;
  NonUpdatableLabel* m_B14Field;

  OkButton* m_acceptButton;

  unsigned int m_fieldHeight = 25;
  unsigned int m_minFieldWidth = 200;

signals:


private slots:
  void on_algoAndConv_button_clicked();
  void on_initCost_button_clicked();
  void on_finalCost_button_clicked();
  void on_stepSuccess_button_clicked();
  void on_chiSq_button_clicked();
  void on_gradMax_button_clicked();
  void on_relDecrease_button_clicked();
  void on_stepNorm_button_clicked();

  void on_accept_button_clicked();
};

#endif