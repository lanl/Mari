#include "updatablelabel.h"
#include <iostream>
UpdatableLabel::UpdatableLabel(QWidget* parent)
    : QLineEdit(parent)
{
  setText("");
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setFixedHeight(23);
  setStyleSheet("border-top: 2px solid gray;"
                "border-left: 2px solid gray;"
                "border-bottom: 1px solid lightgray;"
                "border-right: 1px solid lightgray;"
                "background-color: white;");
}

//void UpdatableLabel::setText(const QString& a_text)
//{
//  QLineEdit::setText(a_text);
//  if(a_text.isEmpty())
//  {
//    setStyleSheet("border: 1px solid black; border-radius: 0px; background-color:#f8f9fa;");
//  }
//  else
//  {
//    setStyleSheet("border: 1px solid black; border-radius: 0px; background-color:white;");
//  }
//  // Let layouts know geometry may have changed, but sizeHint stays constant
//  updateGeometry();
//}

void UpdatableLabel::set_validated_double_text(const QString& a_text) 
{
  if (validator()) 
  {
    int pos = 0;
    QString tempText = a_text; // validate() requires a modifiable copy
    
    if (validator()->validate(tempText, pos) == QValidator::Acceptable) 
    {
      setText(a_text);
    } else 
    {
      std::cout << "fail validation " << a_text.toStdString() << std::endl;
      clear(); 
    }
  } 
  else 
  {
    // Fallback if no validator is attached
    setText(a_text);
  }
}

void UpdatableLabel::highlight()
{
  setStyleSheet("border-top: 2px solid red;"
                "border-left: 2px solid red;"
                "border-bottom: 1px solid red;"
                "border-right: 1px solid red;"
                "background-color: white;");
}

void UpdatableLabel::unhighlight()
{
  setStyleSheet("border-top: 2px solid gray;"
                "border-left: 2px solid gray;"
                "border-bottom: 1px solid lightgray;"
                "border-right: 1px solid lightgray;"
                "background-color: white;");
}

void UpdatableLabel::update_field_style()
{
  if (m_isValid) unhighlight();
  else highlight();
}

bool UpdatableLabel::get_isValid()
{
  return m_isValid;
}

void UpdatableLabel::set_isValid(bool a_state)
{
  m_isValid = a_state;
}
