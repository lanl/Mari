#include "Test_MainWindow.h"

//use QTEST_MAIN instead of QTEST_APPLESS_MAIN since I need a QApplication instance
QTEST_MAIN(Test_MainWindow)

void Test_MainWindow::topLevelWidgets_data()
{
  MainWindow mw;
  mw.setObjectName("mainwindow");
  QList<QWidget*> widgets = mw.getWidgetList();
  QTest::addColumn<QString>("actualValue");
  QTest::addColumn<QList<QString>>("expectedOptions");
  int i=0;
  for (auto* w : widgets) 
  { 
    QTest::addRow("row%d", i) << w->metaObject()->className() << 
    (QList<QString>() << "MainWindow" << "QMenu" << "QComboBoxPrivateContainer");
    i++;
  }
}

void Test_MainWindow::topLevelWidgets()
{
  QFETCH(QString, actualValue);
  QFETCH(QList<QString>, expectedOptions);
  QVERIFY(expectedOptions.contains(actualValue));
}