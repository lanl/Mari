#include "nonupdatablelabel.h"

NonUpdatableLabel::NonUpdatableLabel(QWidget *parent)
    : QLabel(parent)
{
  setText("");
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  setFixedHeight(23);
  setStyleSheet("border: 1px solid black; background-color:#f8f9fa;");
}

// Override setText so that changing text does NOT change the widget's "desired" size
void NonUpdatableLabel::setText(const QString &text)
{
  m_fullText = text;
  QFontMetrics metrics(font());
  // Use current width (if 0, just set full text; it'll be elided on first resize)
  int w = width();
  QString elided = (w > 0)
                   ? metrics.elidedText(m_fullText, Qt::ElideRight, w)
                   : m_fullText;
  QLabel::setText(elided);
  if(elided.isEmpty())
  {
    setStyleSheet("border: 1px solid black; background-color:#f8f9fa;");
  }
  else
  {
    setStyleSheet("border: 1px solid black; background-color:white;");
  }
  // Let layouts know geometry may have changed, but sizeHint stays constant
  updateGeometry();
}

QSize NonUpdatableLabel::sizeHint() const
{
  // Start from QLabel’s sizeHint so height/etc are OK
  QSize size = QLabel::sizeHint();
  // Do NOT let the text dictate width.
  size.setWidth(0);
  return size;
}

QSize NonUpdatableLabel::minimumSizeHint() const
{
  // Same trick for minimum size
  QSize size = QLabel::minimumSizeHint();
  size.setWidth(0);
  return size;
}

void NonUpdatableLabel::resizeEvent(QResizeEvent* event)
{
  QFontMetrics metrics(font());
  //elide from m_fullText, not from QLabel::text()
  QString elided = metrics.elidedText(m_fullText, Qt::ElideRight, event->size().width());
  QLabel::setText(elided);
  QLabel::resizeEvent(event);
}