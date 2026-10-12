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

#ifndef NONUPDATABLELABEL_H
#define NONUPDATABLELABEL_H

//Qt includes
#include <QLabel>
#include <QFontMetrics>
#include <QResizeEvent>


/**
 * This class is a custom QLabel.
 * It is for cases where info is displayed but the user cannot
 * interact with the label directly (i.e. not QLineEdit).
 * It has specific 2D styleSheet to indicate non-interactive.
 * Has methods to change background color based on if text is empty or not.
 * Additionaly, overrides various QLabel methods to ensure that the
 * label always fills the window space (QSizePolicy::Expanding) but
 * long text strings do not cause it to grow. Too long of text
 * will be elided with an elipsis.
 * Growong the wondpwe will allow more tet to show.
 **/
class NonUpdatableLabel : public QLabel
{
    Q_OBJECT

public:
    explicit NonUpdatableLabel(QWidget* parent = nullptr);

  /**
   * Override setText() so that text size does not change the 
   * widget's "desired" size
   **/
  void setText(const QString& text);

protected:
  /**
   * override sizeHint() so that text length doesn't influence the sizeHint
   **/
  QSize sizeHint() const override;

  /**
   * override minimumSizeHint() so that text length doesn't influence 
   * the minimumSizeHint
   **/
  QSize minimumSizeHint() const override;

  /**
   * Override resizeEvent() so that when the window grows or shrirks and the 
   * label grows or shrinks accordingly, the displayed text will then be 
   * displayed and elided correctly
   * */
  void resizeEvent(QResizeEvent* event) override;

private:
  QString m_fullText;

};


#endif