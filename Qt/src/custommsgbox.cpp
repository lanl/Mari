#include "custommsgbox.h"

CustomMsgBox::CustomMsgBox(const QString& a_content, QWidget* parent)
  : m_content(a_content), QMessageBox(parent)
{
  setStyleSheet("background-color: white");
  setMinimumSize(300,200);
  setWindowTitle("Information"); 
  setText(m_content); 
  //setInformativeText("More detailed information can go here."); // Set informative text
  setIcon(QMessageBox::NoIcon); 
  OkButton* customOkButton = new OkButton();
  // Add the custom button to the QMessageBox with the AcceptRole
  // This makes it behave like an "OK" button (e.g., handling the Enter key)
  addButton(customOkButton, QMessageBox::AcceptRole);
  // Set the custom button as the default button (activated by pressing Enter)
  setDefaultButton(customOkButton);
}

void CustomMsgBox::setIcon(QMessageBox::Icon a_icon)
{
  QMessageBox::setIcon(a_icon);
}

WarningMsgBox::WarningMsgBox(const QString& a_content, QWidget* parent)
  : CustomMsgBox(a_content, parent)
{
  setWindowTitle("Warning"); 
  setIcon(QMessageBox::Warning); 
}

YesNoMsgBox::YesNoMsgBox(const QString& a_content, QWidget* parent)
  : CustomMsgBox(a_content, parent)
{
  setWindowTitle(""); 
  setText(a_content);
  CancelButton* customCancelButton = new CancelButton();
  addButton(customCancelButton, QMessageBox::RejectRole);
}