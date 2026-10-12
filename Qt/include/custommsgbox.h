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

#ifndef CUSTOMMSGBOX_H
#define CUSTOMMSGBOX_H

//Qt includes
#include <QMessageBox>
#include <QString>
//Qt project includes
#include "okbutton.h"
#include "cancelbutton.h"

class CustomMsgBox : public QMessageBox
{
  Q_OBJECT

public:
  explicit CustomMsgBox(const QString& a_content, QWidget* parent = nullptr);

protected:
  void setIcon(QMessageBox::Icon);
  QString m_content;
};

class WarningMsgBox : public CustomMsgBox
{
  Q_OBJECT

public:
  explicit WarningMsgBox(const QString& a_content, QWidget* parent = nullptr);
};

class YesNoMsgBox : public CustomMsgBox
{
  Q_OBJECT

public:
  explicit YesNoMsgBox(const QString& a_content, QWidget* parent = nullptr);
};

#endif