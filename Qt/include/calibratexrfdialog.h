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

#ifndef CALIBRATEXRFDIALOG_H
#define CALIBRATEXRFDIALOG_H

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
#include "kcalibrateworker.h"

//C++ includes
#include <string>

/**
 * Class for building the 'XRF Calibrate' dialog box
 **/
class CalibrateXRFDialog : public QDialog
{
  Q_OBJECT

public:
  explicit CalibrateXRFDialog(QWidget* parent = nullptr);
  ~CalibrateXRFDialog();

  std::string strip_path(std::string a_filename);

private:
  bool check_if_data_loaded();
  bool can_analyze() const;
  void update_analyze_button_state();

  //Various widgets.
  //These are only the ones that need to be accessed at some point after creation.
  NonUpdatableLabel* m_loadXRFBkgdField;
  NonUpdatableLabel* m_loadXRFRefField;
  NonUpdatableLabel* m_statusField;

  UpdatableLabel* m_facilityField;
  UpdatableLabel* m_commentField;
  QCheckBox* m_subBkgdCheckBox;
  QLabel* m_subBkgdCheckLabel;
  QComboBox* m_paramComboBox;

  OkButton* m_calibrateButton;

  unsigned int m_fieldHeight = 25;
  unsigned int m_minFieldWidth = 200;

  QPointer<QThread> m_calibrateThread;
  QPointer<KCalibrateWorker> m_calibrateWorker;

signals:
  void load_new_spectra();
  void load_spectrum(KSpectralData& a_dataset);

private slots:
  void handle_field_update();
  void update_status_field(QString a_status);
  void XRF_calibration_failure();
  void on_bkgd_delete_button_clicked();
  void on_ref_delete_button_clicked();
  void on_bkgd_browse_button_clicked(std::string a_fileName);
  void on_ref_browse_button_clicked(std::string a_fileName);
  void on_calibrate_button_clicked();
  void on_thread_finished();
};

#endif