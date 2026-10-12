#include "okbutton.h"

OkButton::OkButton(QWidget* parent)
    : QPushButton(parent)
{
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
  setFixedSize(70, 30);
  setText("Ok");
  setStyleSheet("QPushButton{"
                             "background-color: #006b75;"
                             "border: 1px solid #005975;"
                             "color: white;" 
                             "font-weight: bold;"
                             "border-radius: 8px;"
                             "padding-top: 3px;"
                             "padding-bottom: 3px;"
                             "padding-left: 20px;"
                             "padding-right: 20px;}"
                "QPushButton:hover{"
                             "background-color: white;"
                             "color: #005975;" 
                             "border: 2px solid #b84e41;}"
                "QPushButton:pressed{"
                             "background-color: #006b75;"
                             "color: #343a40;" 
                             "border: 1px solid #005975;}");
}

void OkButton::set_style(bool a_state)
{
  if(a_state)
  {
    setStyleSheet("QPushButton{"
                               "background-color: #006b75;"
                               "border: 1px solid #005975;"
                               "color: white;" 
                               "font-weight: bold;"
                               "border-radius: 8px;"
                               "padding-top: 3px;"
                               "padding-bottom: 3px;"
                               "padding-left: 20px;"
                               "padding-right: 20px;}"
                  "QPushButton:hover{"
                               "background-color: white;"
                               "color: #005975;" 
                               "border: 2px solid #b84e41;}"
                  "QPushButton:pressed{"
                               "background-color: #006b75;"
                               "color: #343a40;" 
                               "border: 1px solid #005975;}");
  }
  else
  {
    setStyleSheet("QPushButton{"
                             "background-color: gray;"
                             "border: 1px solid black;"
                             "color: white;" 
                             "font-weight: bold;"
                             "border-radius: 8px;"
                             "padding-top: 3px;"
                             "padding-bottom: 3px;"
                             "padding-left: 20px;"
                             "padding-right: 20px;}");
  }
}