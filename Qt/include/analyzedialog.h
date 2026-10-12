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

#ifndef ANALYZEDIALOG_H
#define ANALYZEDIALOG_H

//Qt includes
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFontMetrics>
#include <QThread>
#include <QPointer>
#include <QCheckBox>
#include <QRadioButton>
#include <QComboBox>
#include <QEvent>
#include <QKeyEvent>
#include <QDateTime>
#include <QDoubleValidator>
//Project Qt includes
#include "okbutton.h"
#include "cancelbutton.h"
#include "browsebutton.h"
#include "updatablelabel.h"
#include "nonupdatablelabel.h"
#include "loadspectrumdialog.h"
#include "custommsgbox.h"

//Project includes
#include "krunmanager.h"
#include "kfilemanager.h"
#include "kanalysisworker.h"

//C++ includes
#include <string>

/**
 * Class for building the 'Analyze' dialog box
 **/
class AnalyzeDialog : public QDialog
{
  Q_OBJECT

public:
  explicit AnalyzeDialog(QWidget* parent = nullptr);
  ~AnalyzeDialog();

  std::string strip_path(std::string a_filename);

  /**
   * Control analysis based on user selection of analysis options
   **/
  void set_analysis_options(KAnalysisWorker* a_worker);

  /**
   * Save the user entered header info to be printed later
   **/
  void save_analysis_header_info(KAnalysisWorker* a_worker);

  bool eventFilter(QObject *obj, QEvent *event) override;

private:
  bool check_if_data_loaded();
  bool can_analyze() const;
  void update_analyze_button_state();
  void update_temp_field_style(bool valid);

  //Various widgets.
  //These are only the ones that need to be accessed at some point after creation.
  NonUpdatableLabel* m_loadKEDSampField;
  NonUpdatableLabel* m_loadKEDBkgdField;
  NonUpdatableLabel* m_loadKEDRefField;
  NonUpdatableLabel* m_loadXRFSampField;
  NonUpdatableLabel* m_loadXRFBkgdField;
  NonUpdatableLabel* m_loadXRFRefField;
  NonUpdatableLabel* m_statusField;

  UpdatableLabel* m_operatorField;
  UpdatableLabel* m_facilityField;
  UpdatableLabel* m_commentField;

  QCheckBox* m_kedCheckBox;
  QCheckBox* m_xrfCheckBox;
  QCheckBox* m_lowCXrfCheckBox;
  QLabel*    m_fittingCheckLabel;
  QRadioButton* m_kedFittingModeButton;
  QRadioButton* m_kedROIModeButton;
  QLabel*    m_roiCheckLabel;

  QComboBox* m_paramComboBox;
  UpdatableLabel* m_tempField;

  OkButton* m_analyzeButton;

  unsigned int m_fieldHeight = 25;
  unsigned int m_minFieldWidth = 200;

  QPointer<QThread> m_analysisThread;
  QPointer<KAnalysisWorker> m_analysisWorker;

signals:
  void load_new_spectra();

private slots:
  void handle_field_update();
  void update_status_field(QString a_status);
  void analysis_failure();
  void on_kedCheckBox_clicked();
  void on_xrfCheckBox_clicked();
  void on_load_spec_button_clicked();
  void on_KED_samp_delete_button_clicked();
  void on_KED_bkgd_delete_button_clicked();
  void on_KED_ref_delete_button_clicked();
  void on_XRF_samp_delete_button_clicked();
  void on_XRF_bkgd_delete_button_clicked();
  void on_XRF_ref_delete_button_clicked();
  void on_analyze_button_clicked();
  void on_thread_finished();
};

#endif