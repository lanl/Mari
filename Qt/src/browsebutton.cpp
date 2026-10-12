#include "browsebutton.h"

BrowseButton::BrowseButton(QWidget *parent)
  : QPushButton(parent)
{
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setFixedSize(70, 25);
  setText("Browse...");
  setStyleSheet("QPushButton{"
                             "background-color: #f8f9fa;" 
                             "border: 1px solid gray;"
                             "border-radius: 8px;"
                             "padding-top: 3px;"
                             "padding-bottom: 3px;"
                             "padding-left: 6px;"
                             "padding-right: 6px;}"
                "QPushButton:hover{"
                             "background-color: #006b75;"
                             "color: white;" 
                             "border: 1px solid #005975;}"
                "QPushButton:pressed{"
                             "background-color: white;"
                             "color: #343a40;" 
                             "border: 1px solid #343a40;}");
  //Internal connection between browse button and file searching dialog
  connect(this, &QPushButton::clicked, this, &BrowseButton::on_browseButton_clicked);
}

void BrowseButton::on_browseButton_clicked()
{
  QString fileName = QFileDialog::getOpenFileName(this,
                                                  tr("Open File"),
                                                  "/home/user/documents", // Initial directory
                                                  tr("All Files (*.*);;Text Files (*.txt)"));
  if (!fileName.isEmpty()) 
  {
    emit foundFile(fileName.toStdString());
  }
}
