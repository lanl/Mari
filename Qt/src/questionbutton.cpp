#include "questionbutton.h"

QuestionButton::QuestionButton(QWidget *parent)
    : QPushButton(parent)
{
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setFixedSize(18, 18);
    setText("?");
    setStyleSheet("QPushButton{"
                               "background-color: white;" 
                               "border: 1px solid gray;"
                               "border-radius: 4px;"
                               "padding-top: 3px;"
                               "padding-bottom: 3px;"
                               "padding-left: 3px;"
                               "padding-right: 3px;}"
                  "QPushButton:hover{"
                               "border: 1px solid red;}"
                  "QPushButton:pressed{"
                               "background-color: white;"
                               "color: #343a40;" 
                               "border: 1px solid #005975;}");
}