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

#ifndef UPDATABLELABEL_H
#define UPDATABLELABEL_H

//Qt includes
#include <QLineEdit>
#include <QDoubleValidator>

class UpdatableLabel : public QLineEdit
{
  Q_OBJECT

public:
  explicit UpdatableLabel(QWidget* parent = nullptr);

  /**
   * Override setText() to update style
   **/
  //void setText(const QString& a_text);


  /**
   * Before setting text, make sure it is a valid double
   **/
  void set_validated_double_text(const QString& a_text);

  /**
   * Highlight in red
   **/
  void highlight();

  /**
   * return to normal color
   **/
  void unhighlight();

  /**
   * Update style based on whether or not the user provided input is valid.
   * What constitutes a valid entry can vary and will be determined by
   * setting m_isValid
   **/
  void update_field_style();

  /**
   * Getter for m_isValid
   **/
  bool get_isValid();

  /**
   * Setter for m_isValid
   **/
  void set_isValid(bool a_state);

private:

  bool m_isValid;
};



#endif