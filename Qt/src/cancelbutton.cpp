#include "cancelbutton.h"

CancelButton::CancelButton(QWidget *parent)
  : QPushButton(parent)
{
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setFixedSize(70, 30);
  setText("Cancel");
  setStyleSheet("QPushButton{"
                             "background-color: white;" 
                             "border: 1px solid #006b75;"
                             "color: #006b75;"
                             "font-weight: bold;"
                             "border-radius: 8px;"
                             "padding-top: 3px;"
                             "padding-bottom: 3px;"
                             "padding-left: 12px;"
                             "padding-right: 12px;}"
                "QPushButton:hover{"
                             "background-color:#006b75;"
                             "color: white;" 
                             "border: 2px solid #b84e41;}"
                "QPushButton:pressed{"
                             "background-color: white;"
                             "color: #343a40;" 
                             "border: 1px solid #005975;}");
}