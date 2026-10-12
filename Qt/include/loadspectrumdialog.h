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

#ifndef LOADSPECTRUMDIALOG_H
#define LOADSPECTRUMDIALOG_H

//Qt includes
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
//Project Qt includes
#include "okbutton.h"
#include "cancelbutton.h"
#include "browsebutton.h"
#include "deletebutton.h"
#include "nonupdatablelabel.h"
//Project includes
#include "krunmanager.h"
#include "kfilemanager.h"
//C++ includes


class LoadSpectrumDialog : public QDialog 
{
  Q_OBJECT

public:
  explicit LoadSpectrumDialog(QWidget *parent = nullptr);
  ~LoadSpectrumDialog();

  std::string strip_path(std::string a_filename);

private:
  //Various widgets. 
  //These are only the ones that need to be accessed at some point after creation.
  NonUpdatableLabel* m_loadKedSampField;
  NonUpdatableLabel* m_loadKedBkgdField;
  NonUpdatableLabel* m_loadKedRefField;
  NonUpdatableLabel* m_loadXrfSampField;
  NonUpdatableLabel* m_loadXrfBkgdField;
  NonUpdatableLabel* m_loadXrfRefField;

signals:
  void handle_graphing();
  void update_state_fields();
  void load_spectrum(KSpectralData& a_dataset);

private slots:
  void on_KED_samp_browse_button_clicked(std::string a_filenameAndPath);
  void on_KED_bkgd_browse_button_clicked(std::string a_fileName);
  void on_KED_ref_browse_button_clicked(std::string a_fileName);
  void on_XRF_samp_browse_button_clicked(std::string a_fileName);
  void on_XRF_bkgd_browse_button_clicked(std::string a_fileName);
  void on_XRF_ref_browse_button_clicked(std::string a_fileName);

  void on_KED_samp_delete_button_clicked();
  void on_KED_bkgd_delete_button_clicked();
  void on_KED_ref_delete_button_clicked();
  void on_XRF_samp_delete_button_clicked();
  void on_XRF_bkgd_delete_button_clicked();
  void on_XRF_ref_delete_button_clicked();

  void on_ok_button_clicked();
};

#endif
