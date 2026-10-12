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

#ifndef EDITPARAMETERDIALOG_H
#define EDITPARAMETERDIALOG_H

//Qt includes
#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QFontMetrics>
#include <QComboBox>
//Project Qt includes
#include "okbutton.h"
#include "cancelbutton.h"
#include "updatablelabel.h"
#include "nonupdatablelabel.h"
#include "custommsgbox.h"
//Project includes
#include "kfilemanager.h"
#include "kparameterdata.h"
//C++ includes


/**
 * Class for building the 'Edit Parameter Set' dialog box
 **/
class EditParameterDialog : public QDialog 
{
  Q_OBJECT

public:
  explicit EditParameterDialog(QWidget* parent = nullptr);
  ~EditParameterDialog();

private:
  OkButton* m_okButton;
  QComboBox* m_paramComboBox;

signals:
  void open_parameter_set_for_editing();

private slots:
  void on_ok_button_clicked();
};

#endif
