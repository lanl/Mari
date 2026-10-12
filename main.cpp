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

//Project includes
#include "mainwindow.h"
#include "krunmanager.h"
#include "kresultsmanager.h"
#include "kdevlogger.h"
//Qt includes
#include <QApplication>
#include <QStyleFactory>
#include <QSplashScreen>
#include <QPixmap>
#include <QTimer>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif
/*
  UMAREKAWARI (Mari)
  Updated Method for Analysis of Reprocessing Effluent
  using K-edge Assay Without Additional Reference Information
*/

int main(int argc, char *argv[])
{
  //Set up the Qt instance
  QApplication a(argc, argv);

  //Allocate a console for the application if it doesn't have one
  #ifdef _WIN32
  if (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole())
  {
    FILE* pCout;
    freopen_s(&pCout, "CONOUT$", "w", stdout);
    FILE* pCin;
    freopen_s(&pCin, "CONIN$", "r", stdin);
    FILE* pCerr;
    freopen_s(&pCerr, "CONOUT$", "w", stderr);
  }
  #endif

  //instantiate Singleton managers
  KDevLogger& devLogger = KDevLogger::getInstance();
  KRunManager& runManager = KRunManager::getInstance();
  
  //Build the CLI
  //xxx will want to hide some options from the user in help menu xxx
  QCommandLineParser parser;
  parser.setApplicationDescription("UMAREKAWARI (Mari) v1.0");
  parser.addHelpOption();
  parser.addVersionOption();
  QCommandLineOption hiddenOption("a-hidden-option","Runs a hidden option","param");
  parser.addOption(hiddenOption);
  parser.process(a);
  if (parser.isSet(hiddenOption)) 
  {
    std::cout<< "Running a hidden option" <<std::endl;
  }

  //setup splash screen
  QScreen* screen = QGuiApplication::primaryScreen();
  QRect screenGeometry = screen->geometry();
  int screenWidth = screenGeometry.width()/2;
  int screenHeight = screenGeometry.height()/3;
  //QPixmap pixmap(":/images/Kanji.png");
  QPixmap pixmap(":/images/Mari.png");
  QPixmap scaledPixmap = pixmap.scaled(screenWidth, screenHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  QSplashScreen* splash = new QSplashScreen(scaledPixmap);
  splash->show();
  // Process events to ensure the splash screen is displayed immediately
  a.processEvents();

  //Registers std::vector<double> with the Qt Meta-Object system at runtime. 
  //Needed when using custom types in queued signal-slot connections across threads
  qRegisterMetaType<std::vector<double>>("std::vector<double>");
  //#ifdef _WIN32
  //  // Allocate a console for the application if it doesn't have one
  //  if (AttachConsole(ATTACH_PARENT_PROCESS) || AllocConsole()) 
  //  {
  //    FILE* pCout;
  //    freopen_s(&pCout, "CONOUT$", "w", stdout);
  //    FILE* pCin;
  //    freopen_s(&pCin, "CONIN$", "r", stdin);
  //    FILE* pCerr;
  //    freopen_s(&pCerr, "CONOUT$", "w", stderr);
  //  }
  //#endif
  
  QStyle* chosenStyle = QStyleFactory::create("cleanlooks");
  if (chosenStyle) 
  {
    a.setStyle(chosenStyle);
  }

  //set org and app names for QSettings
  QCoreApplication::setOrganizationName("LANL");
  QCoreApplication::setApplicationName("UMAREKAWARI");

  MainWindow w;
  w.setObjectName("mainwindow");
  w.setWindowIcon(QIcon(":/images/Kanji.png")); //program logo
  w.resize(1200, 700);
  w.setStyleSheet("QMainWindow {background-color: white}");

  QTimer::singleShot(1000, [splash, &w]() 
  {
    splash->finish(&w); //Closes the splash screen and shows the main window
    splash->deleteLater();
    w.show(); //execute the Qt application
  });
  return a.exec();
}


/*
  -------------Style Guide-----------------

  Classes: PascalCase
    Class names start with K unless part of GUI
    Manager class names end in 'Manager'

  Methods: snake_case
    Obvious exception is constructor/destructor

  vaiables: camelCase
    Member variables start with 'm_'
    Method arguments start with 'a_'

  Filenames: alllowercase.h

  FRY is used a search term to find TODO comments

*/