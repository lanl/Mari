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

#ifndef NEWPARAMETERDIALOG_H
#define NEWPARAMETERDIALOG_H

//Qt includes
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFontMetrics>
#include <QCalendarWidget>
#include <QDoubleValidator>
//Project Qt includes
#include "okbutton.h"
#include "cancelbutton.h"
#include "questionbutton.h"
#include "updatablelabel.h"
#include "nonupdatablelabel.h"
#include "loadspectrumdialog.h"
#include "custommsgbox.h"
//Project includes
#include "kfilemanager.h"
#include "kparameterdata.h"
//C++ includes
#include <algorithm>
#include <cctype>

/**
 * Class for building the 'New Paramter set' dialog box
 **/
class NewParameterDialog : public QDialog 
{
  Q_OBJECT

public:
  explicit NewParameterDialog(const int a_state, QWidget* parent = nullptr);
  ~NewParameterDialog();

  /**
   * Read user inputed data and put into KParameterData struct to
   * send to JSON dump.
   **/
  void build_parameter_struct();

protected:

private:
  /**
   * Setup the GUI for the general info tab in the main tab widget.
   * Sends layout back to constructor.
   **/
  QWidget* layout_general_tab();

  /**
   * Setup the GUI for the instrument tab in the main tab widget.
   * Sends layout back to constructor.
   **/
  QWidget* layout_inst_tab();

  /**
   * Setup the GUI for the instrument design constants tab in the main tab widget.
   * Sends layout back to constructor.
   **/
  QWidget* layout_inst_design_tab();

  /**
   * Setup the GUI for the instrument XRF empirical constants tab in the main tab widget.
   * Sends layout back to constructor.
   **/
  QWidget* layout_inst_KED_empirical_tab();

  /**
   * Setup the GUI for the instrument XRF empirical constants tab in the main tab widget.
   * Sends layout back to constructor.
   **/
  QWidget* layout_inst_XRF_empirical_tab();
  /**
   * Setup the GUI for the HPGe Detector tab in the main tab widget.
   * Sends layout back to constructor.
   **/
  QWidget* layout_detector_tab();

  /**
   * Setup the GUI for the Source tab in the main tab widget.
   * Sends layout back to constructor.
   **/
  QWidget* layout_sample_tab();

  /**
   * Setup the GUI for the KED ROIs tab in the main tab widget.
   * Sends layout back to constructor.
   **/
  QWidget* layout_roi_tab();

  /**
   * Setup the GUI for the KED ROIs tab in the main tab widget.
   * Sends layout back to constructor.
   **/
  QWidget* layout_roi_KED_tab();

  /**
   * Setup the GUI for the XRF ROIs tab in the main tab widget.
   * Sends layout back to constructor.
   **/
  QWidget* layout_roi_XRF_tab();

  /**
   * Fill in fields for a new parameter set session
   **/
  void setup_for_new();

  /**
   * Fill in fields for an edit parameter set session
   **/
  void setup_for_edit();

  /**
   * Don't allow user to select 'Ok' if not all fields are valid
   **/
  void update_ok_button_state();

  //Various widgets. 
  //These are only the ones that need to be accessed at some point after creation.
  QTabWidget* m_mainTabWidget;
  OkButton* m_okButton;
  //The struct to store the inputted data
  parameter_data::KParameterData m_workingParameterSet;
  //Fields to  place into struct
  UpdatableLabel* m_paramNameField;
  UpdatableLabel* m_authorNameField;
  UpdatableLabel* m_facilityNameField;
  QDateEdit* m_dateEdit;
  UpdatableLabel* m_commentField;
  UpdatableLabel* m_hvField;
  UpdatableLabel* m_incidenceField;
  UpdatableLabel* m_takeoffField;
  UpdatableLabel* m_backscatterField;
  UpdatableLabel* m_beWindowField;
  UpdatableLabel* m_cdFilterField;
  UpdatableLabel* m_feFilterField;
  UpdatableLabel* m_xfrEntranceField;
  UpdatableLabel* m_xfrExitField;
  UpdatableLabel* m_KEDA1Field;
  UpdatableLabel* m_KEDA2Field;
  UpdatableLabel* m_KEDA3Field;
  UpdatableLabel* m_KEDT1Field;
  UpdatableLabel* m_KEDT2Field;
  UpdatableLabel* m_KEDT3Field;
  UpdatableLabel* m_KEDT4Field;
  UpdatableLabel* m_XRFA1Field;
  UpdatableLabel* m_XRFA2Field;
  UpdatableLabel* m_XRFA3Field;
  UpdatableLabel* m_XRFT1Field;
  UpdatableLabel* m_XRFT2Field;
  UpdatableLabel* m_XRFT3Field;
  UpdatableLabel* m_XRFT4Field;
  UpdatableLabel* m_vialIDField;
  UpdatableLabel* m_vialODField;
  QComboBox* m_vialMaterialComboBox;
  QComboBox* m_matrixMaterialComboBox;
  UpdatableLabel* m_KEDLowerBoundField;
  UpdatableLabel* m_KEDUpperBoundField;
  UpdatableLabel* m_KEDBkgdLowLowBoundField;
  UpdatableLabel* m_KEDBkgdLowHighBoundField;
  UpdatableLabel* m_KEDBkgdHighLowBoundField;
  UpdatableLabel* m_KEDBkgdHighHighBoundField;
  UpdatableLabel* m_XRFLowerBoundField;
  UpdatableLabel* m_XRFUpperBoundField;
  UpdatableLabel* m_XRFBkgdLowLowBoundField;
  UpdatableLabel* m_XRFBkgdLowHighBoundField;
  UpdatableLabel* m_XRFBkgdHighLowBoundField;
  UpdatableLabel* m_XRFBkgdHighHighBoundField;
  UpdatableLabel* m_KEDCdCorrField;
  UpdatableLabel* m_KEDBkgdNormField;
  UpdatableLabel* m_KEDTempField;
  QuestionButton* m_backScattBremButton;
  UpdatableLabel* m_CField;
  UpdatableLabel* m_b3Field;
  UpdatableLabel* m_b4Field;
  UpdatableLabel* m_b5Field;
  UpdatableLabel* m_b6Field;
  UpdatableLabel* m_b7Field;
  UpdatableLabel* m_b8Field;
  UpdatableLabel* m_b9Field;
  UpdatableLabel* m_b10Field;
  UpdatableLabel* m_b11Field;
  UpdatableLabel* m_b12Field;
  UpdatableLabel* m_b13Field;
  UpdatableLabel* m_b14Field;

  QString m_loadedParamSetName;
  
signals:

private slots:
  /**
   * Make sure user enters a file name that doesn't contain spaces
   **/
  bool check_valid_string(const QString& a_text);

  /**
   * Check and save the parameter set
   **/
  void on_ok_button_clicked();

  /**
   * Question button info
   **/
  void on_backscattBrem_button_clicked();

};

#endif
