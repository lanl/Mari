#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) 
: QMainWindow(parent),
  m_cd109Line(nullptr),
  m_puKEdgeLine(nullptr),
  m_uKEdgeLine(nullptr),
  m_puKAlpha1Line(nullptr),
  m_puKAlpha2Line(nullptr),
  m_puKAlpha3Line(nullptr),
  m_puKBeta1Line(nullptr),
  m_puKBeta3Line(nullptr),
  m_puKBeta2ILine(nullptr),
  m_puKBeta2IILine(nullptr),
  m_puKBeta4ILine(nullptr),
  m_uKAlpha1Line(nullptr),
  m_uKAlpha2Line(nullptr),
  m_uKBeta1Line(nullptr),
  m_uKBeta3Line(nullptr),
  m_uKBeta2Line(nullptr),
  m_uKBeta4Line(nullptr),
  m_eu154_123Line(nullptr),
  m_eu155_105Line(nullptr)
{
  setWindowTitle("UMAREKAWARI v1.0     Hybrid K-Edge Densitometry Software");
  //load settings from previous session
  load_settings();
  //Make the  central widget.
  m_mainTabWidget = new QTabWidget();
  // Add tabs to the QTabWidget
  m_mainTabWidget->addTab(layout_spectrum_tab(), "Spectrum View");
  m_mainTabWidget->addTab(layout_results_tab(), "Results");
  m_mainTabWidget->addTab(layout_compare_tab(), "Compare");
  //#C4C4C3
  QMenuBar* menuBar = this->menuBar(); // Get the QMenuBar instance
  if (menuBar) 
  {
    menuBar->setStyleSheet("QMenuBar {background-color: #f8f9fa; color: black;}"
                           "QMenuBar::item:selected {background-color: #005975; color: white; border-bottom: 2px solid #b84e41;}"
                           "QMenu::item:selected {background-color: #005975; color: white;}");
  }
  m_mainTabWidget->setStyleSheet(
    "QTabWidget::pane\
    {\
      background-color: white; \
      border: 1px solid #006b75;\
      top: -1px; /* Corrects the double border issue */\
      border-top-right-radius: 8px;\
      border-bottom-left-radius: 8px;\
      border-bottom-right-radius: 8px;\
    }\
    QTabBar::tab \
    {\
      background: #006b75;\
      color: white;\
      font-weight: bold;\
      font-size: 12px;\
      border: 1px solid #005975;\
      /*border-left: 1px solid #005975;*/\
      border-bottom-color: #005975;\
      border-top-left-radius: 8px;\
      border-top-right-radius: 8px;\
      min-width: 8ex;\
      padding: 5px;\
    }\
    QTabBar::tab:selected \
    { \
      background-color: white; \
      color: #005975; \
      border-left: 1px solid #006b75;\
      border-bottom: -1px;\
    }"
  );

  //Set the m_mainTabWidget as the central widget of the main window.
  //This automatically sets the QMainWindow as the parent of m_mainTabWidget
  setCentralWidget(m_mainTabWidget);

  create_actions(); // Create actions for menus
  create_menus(); // Create the menu bar and menus

  connect(m_kedPlot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(on_KED_mouse_move(QMouseEvent*)));
  connect(m_xrfPlot, SIGNAL(mouseMove(QMouseEvent*)), this, SLOT(on_XRF_mouse_move(QMouseEvent*)));
  connect(&KFileManager::getInstance(), &KFileManager::errorMessage, this, &MainWindow::print_error_message);
  connect(&KResultsManager::getInstance(), &KResultsManager::send_KED_results, this, &MainWindow::print_KED_results);
  connect(&KResultsManager::getInstance(), &KResultsManager::send_XRF_results, this, &MainWindow::print_XRF_results);
  //connect(&KResultsManager::getInstance(), &KResultsManager::send_KED_calibration_results, this, &MainWindow::display_KED_cal_results);
  //connect(&KResultsManager::getInstance(), &KResultsManager::send_XRF_calibration_results, this, &MainWindow::display_XRF_cal_results);
  connect(&KResultsManager::getInstance(), &KResultsManager::send_results_summary, this, &MainWindow::print_results_summary);
  connect(m_kedPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(on_ked_mouse_pressed(QMouseEvent*)));
  connect(m_kedPlot, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(on_ked_mouse_released(QMouseEvent*)));
  connect(m_xrfPlot, SIGNAL(mousePress(QMouseEvent*)), this, SLOT(on_xrf_mouse_pressed(QMouseEvent*)));
  connect(m_xrfPlot, SIGNAL(mouseRelease(QMouseEvent*)), this, SLOT(on_xrf_mouse_released(QMouseEvent*)));
  connect(this, &MainWindow::KED_widget_status, &KKEDPlotManager::getInstance(), &KKEDPlotManager::set_widget_status);
  connect(this, &MainWindow::XRF_widget_status, &KXRFPlotManager::getInstance(), &KXRFPlotManager::set_widget_status);
  connect(&KKEDPlotManager::getInstance(), &KKEDPlotManager::plot_fit_too_signal, this, &MainWindow::plot_KED_fit_curve);
    //connect(&KResultsManager::getInstance(), &KResultsManager::send_KED_calibration_results, this, &MainWindow::display_KED_cal_results);
  connect(&KRunManager::getInstance(), &KRunManager::send_cal_XRF_success, this, &MainWindow::display_XRF_cal_results);
}


MainWindow::~MainWindow() 
{
  //No explicit Qt widget memory management needed for child widgets 
  //as they are deleted with parent
}

QWidget* MainWindow::findUltimateParent(QWidget* a_widget)
{
  if (!a_widget) 
  {
    return nullptr;
  }
  QWidget* currentParent = a_widget;
  while (currentParent->parentWidget()) 
  {
    currentParent = currentParent->parentWidget();
  }
  return currentParent;
}

QList<QWidget*> MainWindow::getWidgetList() const
{
  // Get all top-level widgets
  return QApplication::topLevelWidgets();
}

QWidget* MainWindow::layout_spectrum_tab()
{
  QWidget* spectrumPage = new QWidget(m_mainTabWidget);
  //Create the main vertical layout
  //Top is KED area
  //Bottom is XRF area
  QVBoxLayout* specPgVLayout = new QVBoxLayout(spectrumPage);
  m_specPgUpperWidget = new QWidget;
  m_specPgLowerWidget = new QWidget;

  //Create the ked horizontal layout
  //Left side is plotting tabs
  //Right side is ked plot
  QHBoxLayout* kedAreaHLayout = new QHBoxLayout(m_specPgUpperWidget);
  //make the tab widget for the ked plotting info, tools, etc.
  QTabWidget* kedTabWidget = new QTabWidget();
  kedTabWidget->setStyleSheet("QTabWidget::pane {background-color: #f8f9fa;}"
                              "QTabBar::tab:selected {background-color: #f8f9fa;}");
  kedTabWidget->setMaximumWidth(500);
  kedTabWidget->setMaximumHeight(400);

  //Create the xrf horizontal layout
  //Left side is plotting tabs
  //Right side is xrf plot
  QHBoxLayout* xrfAreaHLayout = new QHBoxLayout(m_specPgLowerWidget);
  //make the tab widget for the xrf plotting info, tools, etc.
  QTabWidget* xrfTabWidget = new QTabWidget();
  xrfTabWidget->setStyleSheet("QTabWidget::pane {background-color: #f8f9fa;}"
                              "QTabBar::tab:selected {background-color: #f8f9fa;}");
  xrfTabWidget->setMaximumWidth(500);
  xrfTabWidget->setMaximumHeight(400);

  //Create a horizontal line to separate KED and XRF areas
  QFrame* kedXrfLine = new QFrame();
  kedXrfLine->setFrameShape(QFrame::HLine);
  kedXrfLine->setStyleSheet("color: #006b75;");
  kedXrfLine->setLineWidth(2);

  //Add the upper and lower widgets to the main vertical layout
  specPgVLayout->addWidget(m_specPgUpperWidget);
  specPgVLayout->addWidget(kedXrfLine);
  specPgVLayout->addWidget(m_specPgLowerWidget);

  //Add the spec info page object to ked Tab widget
  kedTabWidget->addTab(layout_KED_specInfo_tab(), "Spectrum Info");
  //Add the plot tools page object to ked Tab widget
  kedTabWidget->addTab(layout_KED_plotTools_tab(), "Plot Tools");

  //Add the ked tab widget to the ked area H layout
  kedAreaHLayout->addWidget(kedTabWidget,1); //set a 1:2 column width ratio
  //Add the ked plot widget to the ked area H layout
  kedAreaHLayout->addWidget(layout_KED_plotting_area(),2); //set a 1:2 column width ratio
  //Set the ked tab widget size policy
  kedTabWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  //Add the spec info page object to xrf Tab widget
  xrfTabWidget->addTab(layout_XRF_specInfo_tab(), "Spectrum Info");
  //Add the plot tools page object to xrf Tab widget
  xrfTabWidget->addTab(layout_XRF_plotTools_tab(), "Plot Tools");

  //Add the xrf tab widget to the xrf area H layout
  xrfAreaHLayout->addWidget(xrfTabWidget,1);
  //Add the xrf plot widget to the xrf area H layout
  xrfAreaHLayout->addWidget(layout_XRF_plotting_area(),2);
  //Set the xrf tab widget size policy
  xrfTabWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

  return spectrumPage;
}

QWidget* MainWindow::layout_KED_specInfo_tab()
{
  //Spec info page object
  QWidget* kedSpecInfoPage = new QWidget();
  //Vertical layout will hold all the spectrum info horizontal layouts
  QVBoxLayout* kedSpecInfoPgVLayout = new QVBoxLayout(kedSpecInfoPage);
  
  //Horizontal layout for ked sample title.
  QHBoxLayout* kedSampTitleHLayout = new QHBoxLayout();
  QLabel* kedSampTitleLabel = new QLabel;
  kedSampTitleLabel->setText("KED Sample");
  kedSampTitleLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  kedSampTitleHLayout->addWidget(kedSampTitleLabel);
  kedSampTitleHLayout->setAlignment(kedSampTitleLabel, Qt::AlignCenter);

  //Horizontal layout for ked sample filename.
  QHBoxLayout* kedSampFileHLayout = new QHBoxLayout();
  QLabel* kedSampFilenameLabel = new QLabel;
  kedSampFilenameLabel->setText("File Name:");
  m_kedSampFilenameField = new NonUpdatableLabel;
  kedSampFileHLayout->addWidget(kedSampFilenameLabel);
  kedSampFileHLayout->addItem(new QSpacerItem(13, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedSampFileHLayout->addWidget(m_kedSampFilenameField);
  
  //Horizontal layout for ked sample date and channels.
  QHBoxLayout* kedSampDateHLayout = new QHBoxLayout();
  QLabel* kedSampDateLabel = new QLabel;
  kedSampDateLabel->setText("Acq. Date:");
  m_kedSampDateField = new NonUpdatableLabel;
  QLabel* kedSampChannelLabel = new QLabel;
  kedSampChannelLabel->setText("Channels:");
  m_kedSampChannelField = new NonUpdatableLabel;
  kedSampDateHLayout->addWidget(kedSampDateLabel);
  kedSampDateHLayout->addItem(new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedSampDateHLayout->addWidget(m_kedSampDateField);
  kedSampDateHLayout->addItem(new QSpacerItem(18, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedSampDateHLayout->addWidget(kedSampChannelLabel);
  kedSampDateHLayout->addItem(new QSpacerItem(17, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedSampDateHLayout->addWidget(m_kedSampChannelField);
  
  //Horizontal layout for ked sample RT and LT.
  QHBoxLayout* kedSampLTHLayout = new QHBoxLayout();
  QLabel* kedSampLTLabel = new QLabel;
  kedSampLTLabel->setText("Live Time (s):");
  m_kedSampLTField = new NonUpdatableLabel;
  QLabel* kedSampRTLabel = new QLabel;
  kedSampRTLabel->setText("Real Time (s):");
  m_kedSampRTField = new NonUpdatableLabel;
  kedSampLTHLayout->addWidget(kedSampLTLabel);
  kedSampLTHLayout->addWidget(m_kedSampLTField);
  kedSampLTHLayout->addItem(new QSpacerItem(18, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedSampLTHLayout->addWidget(kedSampRTLabel);
  kedSampLTHLayout->addWidget(m_kedSampRTField);

  //Horizontal layout for ked bkgd title.
  QHBoxLayout* kedBkgdTitleHLayout = new QHBoxLayout();
  QLabel* kedBkgdTitleLabel = new QLabel;
  kedBkgdTitleLabel->setText("KED Background");
  kedBkgdTitleLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  kedBkgdTitleHLayout->addWidget(kedBkgdTitleLabel);
  kedBkgdTitleHLayout->setAlignment(kedBkgdTitleLabel, Qt::AlignCenter);

  //Horizontal layout for ked Bkg filename.
  QHBoxLayout* kedBkgdFileHLayout = new QHBoxLayout();
  QLabel* kedBkgdFilenameLabel = new QLabel;
  kedBkgdFilenameLabel->setText("File Name:");
  m_kedBkgdFilenameField = new NonUpdatableLabel;
  kedBkgdFileHLayout->addWidget(kedBkgdFilenameLabel);
  kedBkgdFileHLayout->addItem(new QSpacerItem(13, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedBkgdFileHLayout->addWidget(m_kedBkgdFilenameField);
  
  //Horizontal layout for ked Bkgd date and channels.
  QHBoxLayout* kedBkgdDateHLayout = new QHBoxLayout();
  QLabel* kedBkgdDateLabel = new QLabel;
  kedBkgdDateLabel->setText("Acq. Date:");
  m_kedBkgdDateField = new NonUpdatableLabel;
  QLabel* kedBkgdChannelLabel = new QLabel;
  kedBkgdChannelLabel->setText("Channels:");
  m_kedBkgdChannelField = new NonUpdatableLabel;
  kedBkgdDateHLayout->addWidget(kedBkgdDateLabel);
  kedBkgdDateHLayout->addItem(new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedBkgdDateHLayout->addWidget(m_kedBkgdDateField);
  kedBkgdDateHLayout->addItem(new QSpacerItem(18, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedBkgdDateHLayout->addWidget(kedBkgdChannelLabel);
  kedBkgdDateHLayout->addItem(new QSpacerItem(17, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedBkgdDateHLayout->addWidget(m_kedBkgdChannelField);
  
  //Horizontal layout for ked Bkgd RT and LT.
  QHBoxLayout* kedBkgdLTHLayout = new QHBoxLayout();
  QLabel* kedBkgdLTLabel = new QLabel;
  kedBkgdLTLabel->setText("Live Time (s):");
  m_kedBkgdLTField = new NonUpdatableLabel;
  QLabel* kedBkgdRTLabel = new QLabel;
  kedBkgdRTLabel->setText("Real Time (s):");
  m_kedBkgdRTField = new NonUpdatableLabel;
  kedBkgdLTHLayout->addWidget(kedBkgdLTLabel);
  kedBkgdLTHLayout->addWidget(m_kedBkgdLTField);
  kedBkgdLTHLayout->addItem(new QSpacerItem(18, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedBkgdLTHLayout->addWidget(kedBkgdRTLabel);
  kedBkgdLTHLayout->addWidget(m_kedBkgdRTField);

  //Horizontal layout for ked Ref title.
  QHBoxLayout* kedRefTitleHLayout = new QHBoxLayout();
  QLabel* kedRefTitleLabel = new QLabel;
  kedRefTitleLabel->setText("KED Reference");
  kedRefTitleLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  kedRefTitleHLayout->addWidget(kedRefTitleLabel);
  kedRefTitleHLayout->setAlignment(kedRefTitleLabel, Qt::AlignCenter);

  //Horizontal layout for ked Ref filename.
  QHBoxLayout* kedRefFileHLayout = new QHBoxLayout();
  QLabel* kedRefFilenameLabel = new QLabel;
  kedRefFilenameLabel->setText("File Name:");
  m_kedRefFilenameField = new NonUpdatableLabel;
  kedRefFileHLayout->addWidget(kedRefFilenameLabel);
  kedRefFileHLayout->addItem(new QSpacerItem(13, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedRefFileHLayout->addWidget(m_kedRefFilenameField);
  
  //Horizontal layout for ked Ref date and channels.
  QHBoxLayout* kedRefDateHLayout = new QHBoxLayout();
  QLabel* kedRefDateLabel = new QLabel;
  kedRefDateLabel->setText("Acq. Date:");
  m_kedRefDateField = new NonUpdatableLabel;
  QLabel* kedRefChannelLabel = new QLabel;
  kedRefChannelLabel->setText("Channels:");
  m_kedRefChannelField = new NonUpdatableLabel;
  kedRefDateHLayout->addWidget(kedRefDateLabel);
  kedRefDateHLayout->addItem(new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedRefDateHLayout->addWidget(m_kedRefDateField);
  kedRefDateHLayout->addItem(new QSpacerItem(18, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedRefDateHLayout->addWidget(kedRefChannelLabel);
  kedRefDateHLayout->addItem(new QSpacerItem(17, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedRefDateHLayout->addWidget(m_kedRefChannelField);
  
  //Horizontal layout for ked Ref RT and LT.
  QHBoxLayout* kedRefLTHLayout = new QHBoxLayout();
  QLabel* kedRefLTLabel = new QLabel;
  kedRefLTLabel->setText("Live Time (s):");
  m_kedRefLTField = new NonUpdatableLabel;
  QLabel* kedRefRTLabel = new QLabel;
  kedRefRTLabel->setText("Real Time (s):");
  m_kedRefRTField = new NonUpdatableLabel;
  kedRefLTHLayout->addWidget(kedRefLTLabel);
  kedRefLTHLayout->addWidget(m_kedRefLTField);
  kedRefLTHLayout->addItem(new QSpacerItem(18, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedRefLTHLayout->addWidget(kedRefRTLabel);
  kedRefLTHLayout->addWidget(m_kedRefRTField);

  //Add the ked spec info H layouts to the V layout
  kedSpecInfoPgVLayout->addLayout(kedSampTitleHLayout);
  kedSpecInfoPgVLayout->addLayout(kedSampFileHLayout);
  kedSpecInfoPgVLayout->addLayout(kedSampDateHLayout);
  kedSpecInfoPgVLayout->addLayout(kedSampLTHLayout);
  kedSpecInfoPgVLayout->addLayout(kedBkgdTitleHLayout);
  kedSpecInfoPgVLayout->addLayout(kedBkgdFileHLayout);
  kedSpecInfoPgVLayout->addLayout(kedBkgdDateHLayout);
  kedSpecInfoPgVLayout->addLayout(kedBkgdLTHLayout);
  kedSpecInfoPgVLayout->addLayout(kedRefTitleHLayout);
  kedSpecInfoPgVLayout->addLayout(kedRefFileHLayout);
  kedSpecInfoPgVLayout->addLayout(kedRefDateHLayout);
  kedSpecInfoPgVLayout->addLayout(kedRefLTHLayout);

  return kedSpecInfoPage;
}

QWidget* MainWindow::layout_XRF_specInfo_tab()
{
  //Spec info page object
  QWidget* xrfSpecInfoPage = new QWidget();
  //Vertical layout will hold all the spectrum info horizontal layouts
  QVBoxLayout* xrfSpecInfoPgVLayout = new QVBoxLayout(xrfSpecInfoPage);

  //Horizontal layout for xrf sample title.
  QHBoxLayout* xrfSampTitleHLayout = new QHBoxLayout();
  QLabel* xrfSampTitleLabel = new QLabel;
  xrfSampTitleLabel->setText("XRF Sample");
  xrfSampTitleLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  xrfSampTitleHLayout->addWidget(xrfSampTitleLabel);
  xrfSampTitleHLayout->setAlignment(xrfSampTitleLabel, Qt::AlignCenter);

  //Horizontal layout for xrf sample filename.
  QHBoxLayout* xrfSampFileHLayout = new QHBoxLayout();
  QLabel* xrfSampFilenameLabel = new QLabel;
  xrfSampFilenameLabel->setText("File Name:");
  m_xrfSampFilenameField = new NonUpdatableLabel;
  xrfSampFileHLayout->addWidget(xrfSampFilenameLabel);
  xrfSampFileHLayout->addItem(new QSpacerItem(13, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  xrfSampFileHLayout->addWidget(m_xrfSampFilenameField);
  
  //Horizontal layout for xrf sample date and channels.
  QHBoxLayout* xrfSampDateHLayout = new QHBoxLayout();
  QLabel* xrfSampDateLabel = new QLabel;
  xrfSampDateLabel->setText("Acq. Date:");
  m_xrfSampDateField = new NonUpdatableLabel;
  QLabel* xrfSampChannelLabel = new QLabel;
  xrfSampChannelLabel->setText("Channels:");
  m_xrfSampChannelField = new NonUpdatableLabel;
  xrfSampDateHLayout->addWidget(xrfSampDateLabel);
  xrfSampDateHLayout->addItem(new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  xrfSampDateHLayout->addWidget(m_xrfSampDateField);
  xrfSampDateHLayout->addItem(new QSpacerItem(18, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  xrfSampDateHLayout->addWidget(xrfSampChannelLabel);
  xrfSampDateHLayout->addItem(new QSpacerItem(17, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  xrfSampDateHLayout->addWidget(m_xrfSampChannelField);
  
  //Horizontal layout for xrf sample RT and LT.
  QHBoxLayout* xrfSampLTHLayout = new QHBoxLayout();
  QLabel* xrfSampLTLabel = new QLabel;
  xrfSampLTLabel->setText("Live Time (s):");
  m_xrfSampLTField = new NonUpdatableLabel;
  QLabel* xrfSampRTLabel = new QLabel;
  xrfSampRTLabel->setText("Real Time (s):");
  m_xrfSampRTField = new NonUpdatableLabel;
  xrfSampLTHLayout->addWidget(xrfSampLTLabel);
  xrfSampLTHLayout->addWidget(m_xrfSampLTField);
  xrfSampLTHLayout->addItem(new QSpacerItem(18, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  xrfSampLTHLayout->addWidget(xrfSampRTLabel);
  xrfSampLTHLayout->addWidget(m_xrfSampRTField);

  //Horizontal layout for xrf bkgd title.
  QHBoxLayout* xrfBkgdTitleHLayout = new QHBoxLayout();
  QLabel* xrfBkgdTitleLabel = new QLabel;
  xrfBkgdTitleLabel->setText("XRF Background");
  xrfBkgdTitleLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  xrfBkgdTitleHLayout->addWidget(xrfBkgdTitleLabel);
  xrfBkgdTitleHLayout->setAlignment(xrfBkgdTitleLabel, Qt::AlignCenter);

  //Horizontal layout for xrf Bkg filename.
  QHBoxLayout* xrfBkgdFileHLayout = new QHBoxLayout();
  QLabel* xrfBkgdFilenameLabel = new QLabel;
  xrfBkgdFilenameLabel->setText("File Name:");
  m_xrfBkgdFilenameField = new NonUpdatableLabel;
  xrfBkgdFileHLayout->addWidget(xrfBkgdFilenameLabel);
  xrfBkgdFileHLayout->addItem(new QSpacerItem(13, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  xrfBkgdFileHLayout->addWidget(m_xrfBkgdFilenameField);
  
  //Horizontal layout for xrf Bkgd date and channels.
  QHBoxLayout* xrfBkgdDateHLayout = new QHBoxLayout();
  QLabel* xrfBkgdDateLabel = new QLabel;
  xrfBkgdDateLabel->setText("Acq. Date:");
  m_xrfBkgdDateField = new NonUpdatableLabel;
  QLabel* xrfBkgdChannelLabel = new QLabel;
  xrfBkgdChannelLabel->setText("Channels:");
  m_xrfBkgdChannelField = new NonUpdatableLabel;
  xrfBkgdDateHLayout->addWidget(xrfBkgdDateLabel);
  xrfBkgdDateHLayout->addItem(new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  xrfBkgdDateHLayout->addWidget(m_xrfBkgdDateField);
  xrfBkgdDateHLayout->addItem(new QSpacerItem(18, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  xrfBkgdDateHLayout->addWidget(xrfBkgdChannelLabel);
  xrfBkgdDateHLayout->addItem(new QSpacerItem(17, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  xrfBkgdDateHLayout->addWidget(m_xrfBkgdChannelField);
  
  //Horizontal layout for xrf Bkgd RT and LT.
  QHBoxLayout* xrfBkgdLTHLayout = new QHBoxLayout();
  QLabel* xrfBkgdLTLabel = new QLabel;
  xrfBkgdLTLabel->setText("Live Time (s):");
  m_xrfBkgdLTField = new NonUpdatableLabel;
  QLabel* xrfBkgdRTLabel = new QLabel;
  xrfBkgdRTLabel->setText("Real Time (s):");
  m_xrfBkgdRTField = new NonUpdatableLabel;
  xrfBkgdLTHLayout->addWidget(xrfBkgdLTLabel);
  xrfBkgdLTHLayout->addWidget(m_xrfBkgdLTField);
  xrfBkgdLTHLayout->addItem(new QSpacerItem(18, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  xrfBkgdLTHLayout->addWidget(xrfBkgdRTLabel);
  xrfBkgdLTHLayout->addWidget(m_xrfBkgdRTField);

  //Horizontal layout for xrf Ref title.
  QHBoxLayout* xrfRefTitleHLayout = new QHBoxLayout();
  QLabel* xrfRefTitleLabel = new QLabel;
  xrfRefTitleLabel->setText("XRF Reference");
  xrfRefTitleLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  xrfRefTitleHLayout->addWidget(xrfRefTitleLabel);
  xrfRefTitleHLayout->setAlignment(xrfRefTitleLabel, Qt::AlignCenter);

  //Horizontal layout for xrf Ref filename.
  QHBoxLayout* xrfRefFileHLayout = new QHBoxLayout();
  QLabel* xrfRefFilenameLabel = new QLabel;
  xrfRefFilenameLabel->setText("File Name:");
  m_xrfRefFilenameField = new NonUpdatableLabel;
  xrfRefFileHLayout->addWidget(xrfRefFilenameLabel);
  xrfRefFileHLayout->addItem(new QSpacerItem(13, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  xrfRefFileHLayout->addWidget(m_xrfRefFilenameField);
  
  //Horizontal layout for xrf Ref date and channels.
  QHBoxLayout* xrfRefDateHLayout = new QHBoxLayout();
  QLabel* xrfRefDateLabel = new QLabel;
  xrfRefDateLabel->setText("Acq. Date:");
  m_xrfRefDateField = new NonUpdatableLabel;
  QLabel* xrfRefChannelLabel = new QLabel;
  xrfRefChannelLabel->setText("Channels:");
  m_xrfRefChannelField = new NonUpdatableLabel;
  xrfRefDateHLayout->addWidget(xrfRefDateLabel);
  xrfRefDateHLayout->addItem(new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  xrfRefDateHLayout->addWidget(m_xrfRefDateField);
  xrfRefDateHLayout->addItem(new QSpacerItem(18, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  xrfRefDateHLayout->addWidget(xrfRefChannelLabel);
  xrfRefDateHLayout->addItem(new QSpacerItem(17, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  xrfRefDateHLayout->addWidget(m_xrfRefChannelField);
  
  //Horizontal layout for xrf Ref RT and LT.
  QHBoxLayout* xrfRefLTHLayout = new QHBoxLayout();
  QLabel* xrfRefLTLabel = new QLabel;
  xrfRefLTLabel->setText("Live Time (s):");
  m_xrfRefLTField = new NonUpdatableLabel;
  QLabel* xrfRefRTLabel = new QLabel;
  xrfRefRTLabel->setText("Real Time (s):");
  m_xrfRefRTField = new NonUpdatableLabel;
  xrfRefLTHLayout->addWidget(xrfRefLTLabel);
  xrfRefLTHLayout->addWidget(m_xrfRefLTField);
  xrfRefLTHLayout->addItem(new QSpacerItem(18, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  xrfRefLTHLayout->addWidget(xrfRefRTLabel);
  xrfRefLTHLayout->addWidget(m_xrfRefRTField);

  //Add the xrf spec info H layouts to the V layout
  xrfSpecInfoPgVLayout->addLayout(xrfSampTitleHLayout);
  xrfSpecInfoPgVLayout->addLayout(xrfSampFileHLayout);
  xrfSpecInfoPgVLayout->addLayout(xrfSampDateHLayout);
  xrfSpecInfoPgVLayout->addLayout(xrfSampLTHLayout);
  xrfSpecInfoPgVLayout->addLayout(xrfBkgdTitleHLayout);
  xrfSpecInfoPgVLayout->addLayout(xrfBkgdFileHLayout);
  xrfSpecInfoPgVLayout->addLayout(xrfBkgdDateHLayout);
  xrfSpecInfoPgVLayout->addLayout(xrfBkgdLTHLayout);
  xrfSpecInfoPgVLayout->addLayout(xrfRefTitleHLayout);
  xrfSpecInfoPgVLayout->addLayout(xrfRefFileHLayout);
  xrfSpecInfoPgVLayout->addLayout(xrfRefDateHLayout);
  xrfSpecInfoPgVLayout->addLayout(xrfRefLTHLayout);

  return xrfSpecInfoPage;
}

QWidget* MainWindow::layout_KED_plotTools_tab()
{
  //Plot tools page object
  QWidget* kedPlotToolsPage = new QWidget();
  //Vertical layout will hold all the plot tools horizontal layouts
  QVBoxLayout* m_kedPlotToolsPgVLayout = new QVBoxLayout(kedPlotToolsPage);

  //Horizontal layout for Graphics header.
  QHBoxLayout* kedGraphicsHeaderHLayout = new QHBoxLayout();
  //Graphics header.
  QLabel* kedGraphicsLabel = new QLabel;
  kedGraphicsLabel->setText("Graphics Options");
  kedGraphicsLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  //add label to the H layout
  kedGraphicsHeaderHLayout->addWidget(kedGraphicsLabel);

  //Create a horizontal line to separate header from widgets
  QFrame* kedGraphicsLine = new QFrame();
  kedGraphicsLine->setFrameShape(QFrame::HLine);

  //Horizontal layout for lin/log radio buttons.
  QHBoxLayout* kedLogLinHLayout = new QHBoxLayout();
  //lin/log widgets
  QButtonGroup* kedLinLogGroup = new QButtonGroup(this);
  m_kedLogButton = new QRadioButton("Log");
  m_kedLinButton = new QRadioButton("Lin");
  kedLinLogGroup->addButton(m_kedLogButton);
  kedLinLogGroup->addButton(m_kedLinButton);
  CancelButton* kedRescaleButton = new CancelButton();//using cancel button color scheme
  kedRescaleButton->setText("Reset Plot");
  kedRescaleButton->setFixedWidth(100);
  //add buttons to their H layout
  kedLogLinHLayout->addWidget(m_kedLogButton);
  kedLogLinHLayout->addWidget(m_kedLinButton);
  kedLogLinHLayout->addWidget(kedRescaleButton);
  //set radio button default states
  m_kedLogButton->setChecked(false);
  m_kedLinButton->setChecked(true);
  //Connect ked log/lin buttons to slots
  connect(m_kedLogButton, &QRadioButton::toggled, this, &MainWindow::on_KED_log_button_toggled);
  connect(m_kedLinButton, &QRadioButton::toggled, this, &MainWindow::on_KED_lin_button_toggled);
  connect(kedRescaleButton, &QPushButton::clicked, this, &MainWindow::on_KED_reset_button_clicked);

  //Horizontal layout for sample/bkgd/ref check boxes.
  QHBoxLayout* kedFileBoxHLayout = new QHBoxLayout();
  //sample/bkgd/ref check boxes.
  m_kedSampCheckBox = new QCheckBox("Sample");
  m_kedSampCheckBox->setChecked(true);
  m_kedSampCheckBox->setDisabled(true);
  m_kedBkgdCheckBox = new QCheckBox("Background");
  m_kedBkgdCheckBox->setChecked(true);
  m_kedBkgdCheckBox->setDisabled(true);
  m_kedRefCheckBox = new QCheckBox("Reference");
  m_kedRefCheckBox->setChecked(true);
  m_kedRefCheckBox->setDisabled(true);
  //add check boxes to their H layout
  kedFileBoxHLayout->addWidget(m_kedSampCheckBox);
  kedFileBoxHLayout->addWidget(m_kedBkgdCheckBox);
  kedFileBoxHLayout->addWidget(m_kedRefCheckBox);
  //Connect ked samp check box to slot
  connect(m_kedSampCheckBox, &QCheckBox::toggled, this, &MainWindow::on_KED_samp_checkBox_toggled);
  connect(m_kedBkgdCheckBox, &QCheckBox::toggled, this, &MainWindow::on_KED_bkgd_checkBox_toggled);
  connect(m_kedRefCheckBox, &QCheckBox::toggled, this, &MainWindow::on_KED_ref_checkBox_toggled);

  //Horizontal layout for sample/bkgd/ref color drop downs.
  QHBoxLayout* kedFileColorHLayout = new QHBoxLayout();
  //sample/bkgd/ref color drop downs
  m_kedSampComboBox = new QComboBox();
  m_kedSampComboBox->addItem("Blue");
  m_kedSampComboBox->addItem("Black");
  m_kedSampComboBox->addItem("Red");
  m_kedSampComboBox->addItem("Green");
  m_kedSampComboBox->addItem("Cyan");
  m_kedSampComboBox->setCurrentIndex(0);
  m_kedSampComboBox->setDisabled(true);
  m_kedBkgdComboBox = new QComboBox();
  m_kedBkgdComboBox->addItem("Blue");
  m_kedBkgdComboBox->addItem("Black");
  m_kedBkgdComboBox->addItem("Red");
  m_kedBkgdComboBox->addItem("Green");
  m_kedBkgdComboBox->addItem("Cyan");
  m_kedBkgdComboBox->setCurrentIndex(1);
  m_kedBkgdComboBox->setDisabled(true);
  m_kedRefComboBox = new QComboBox();
  m_kedRefComboBox->addItem("Blue");
  m_kedRefComboBox->addItem("Black");
  m_kedRefComboBox->addItem("Red");
  m_kedRefComboBox->addItem("Green");
  m_kedRefComboBox->addItem("Cyan");
  m_kedRefComboBox->setCurrentIndex(2);
  m_kedRefComboBox->setDisabled(true);
  //Add combo boxes to their H layout
  kedFileColorHLayout->addWidget(m_kedSampComboBox);
  kedFileColorHLayout->addWidget(m_kedBkgdComboBox);
  kedFileColorHLayout->addWidget(m_kedRefComboBox);
  connect(m_kedSampComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_KED_samp_comboBox_toggled);
  connect(m_kedBkgdComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_KED_bkgd_comboBox_toggled);
  connect(m_kedRefComboBox,  QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_KED_ref_comboBox_toggled);

  //Horizontal layout for gridlines check boxes.
  QHBoxLayout* kedGridlinesHLayout = new QHBoxLayout();
  //x&y gridlines check boxes.
  m_kedXGridCheckBox = new QCheckBox("X-axis Gridlines");
  m_kedYGridCheckBox = new QCheckBox("Y-axis Gridlines");
  //add check boxes to their H layout
  kedGridlinesHLayout->addWidget(m_kedXGridCheckBox);
  kedGridlinesHLayout->addWidget(m_kedYGridCheckBox);
  //set check box default states
  m_kedXGridCheckBox->setChecked(true);
  m_kedYGridCheckBox->setChecked(true);
  //Connect ked log/lin buttons to slots
  connect(m_kedXGridCheckBox, &QCheckBox::toggled, this, &MainWindow::on_KED_x_grid_checkBox_toggled);
  connect(m_kedYGridCheckBox, &QCheckBox::toggled, this, &MainWindow::on_KED_y_grid_checkBox_toggled);

  //Horizontal layout for k-edge check boxes.
  QHBoxLayout* kedKEdgeLinesHLayout = new QHBoxLayout();
  //U&Pu K-Edge lines check boxes.
  m_kedCd109CheckBox = new QCheckBox("Cd-109");
  m_kedPuKEdgeCheckBox = new QCheckBox("Pu K-Edge");
  m_kedUKEdgeCheckBox = new QCheckBox("U K-Edge");
  //add check boxes to their H layout
  kedKEdgeLinesHLayout->addWidget(m_kedCd109CheckBox);
  kedKEdgeLinesHLayout->addWidget(m_kedPuKEdgeCheckBox);
  kedKEdgeLinesHLayout->addWidget(m_kedUKEdgeCheckBox);
  //set check box default states
  m_kedCd109CheckBox->setChecked(false);
  m_kedPuKEdgeCheckBox->setChecked(false);
  m_kedUKEdgeCheckBox->setChecked(false);
  connect(m_kedCd109CheckBox, &QCheckBox::toggled, this, &MainWindow::on_KED_cd109_checkBox_toggled);
  connect(m_kedPuKEdgeCheckBox, &QCheckBox::toggled, this, &MainWindow::on_KED_Pu_KEdge_checkBox_toggled);
  connect(m_kedUKEdgeCheckBox, &QCheckBox::toggled, this, &MainWindow::on_KED_U_KEdge_checkBox_toggled);

  //Horizontal layout for hidden fit curve check box
  QHBoxLayout* kedFitCheckBoxHLayout = new QHBoxLayout();
  m_kedFitCheckBox = new QCheckBox("Fitted Curve");
  m_kedFitCheckBox->setChecked(false);
  m_kedFitCheckBox->setVisible(false);
  //add buttons to layout
  kedFitCheckBoxHLayout->addWidget(m_kedFitCheckBox);
  connect(m_kedFitCheckBox, &QCheckBox::toggled, this, &MainWindow::on_KED_fit_curve_button_toggled);

  //Horizontal layout for Energy Cal header.
  QHBoxLayout* kedEnergyCalHeaderHLayout = new QHBoxLayout();
  //Energy Cal header.
  QLabel* kedEnergyCalLabel = new QLabel("Energy Calibration");
  kedEnergyCalLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  //add label to the H layout
  kedEnergyCalHeaderHLayout->addWidget(kedEnergyCalLabel);

  //horizontal layout for E cal radio buttons
  QHBoxLayout* kedEnergyCalButtonsHLayout = new QHBoxLayout();
  m_kedEnergyCalGroup = new QButtonGroup(this);
  m_kedSampEcalButton = new QRadioButton("Sample");
  m_kedSampEcalButton->setDisabled(true);
  m_kedBkgdEcalButton = new QRadioButton("Background");
  m_kedBkgdEcalButton->setDisabled(true);
  m_kedRefEcalButton = new QRadioButton("Reference");
  m_kedRefEcalButton->setDisabled(true);
  m_kedEnergyCalGroup->addButton(m_kedSampEcalButton,0);
  m_kedEnergyCalGroup->addButton(m_kedBkgdEcalButton,1);
  m_kedEnergyCalGroup->addButton(m_kedRefEcalButton,2);
  //add buttons to layout
  kedEnergyCalButtonsHLayout->addWidget(m_kedSampEcalButton);
  kedEnergyCalButtonsHLayout->addWidget(m_kedBkgdEcalButton);
  kedEnergyCalButtonsHLayout->addWidget(m_kedRefEcalButton);
  //connect(m_kedSampEcalButton, &QRadioButton::toggled, this, &MainWindow::on_KED_samp_Ecal_button_toggled);
  //connect(m_kedBkgdEcalButton, &QRadioButton::toggled, this, &MainWindow::on_KED_bkgd_Ecal_button_toggled);
  //connect(m_kedRefEcalButton, &QRadioButton::toggled, this, &MainWindow::on_KED_ref_Ecal_button_toggled);
  connect(m_kedEnergyCalGroup, &QButtonGroup::idToggled,
          this, &MainWindow::on_KED_Ecal_group_toggled);

  //Create a horizontal line to separate header from widgets
  QFrame* kedECalLine = new QFrame();
  kedECalLine->setFrameShape(QFrame::HLine);

  //Horizontal layout for ked E energy cal.
  QHBoxLayout* kedECalHLayout = new QHBoxLayout();
  QLabel* kedOffsetLabel = new QLabel;
  kedOffsetLabel->setText("E (keV) = ");
  m_kedOffsetField = new UpdatableLabel;
  QLabel* kedLinearLabel = new QLabel;
  kedLinearLabel->setText(" + ");
  m_kedLinearField = new UpdatableLabel;
  QLabel* kedQuadraticLabel = new QLabel;
  kedQuadraticLabel->setText("*ch  + ");
  m_kedQuadraticField = new UpdatableLabel;
  QLabel* kedLastLabel = new QLabel;
  kedLastLabel->setText("*ch<sup>2</sup>");
  kedECalHLayout->addWidget(kedOffsetLabel);
  kedECalHLayout->addWidget(m_kedOffsetField);
  kedECalHLayout->addWidget(kedLinearLabel);
  kedECalHLayout->addWidget(m_kedLinearField);
  kedECalHLayout->addWidget(kedQuadraticLabel);
  kedECalHLayout->addWidget(m_kedQuadraticField);
  kedECalHLayout->addWidget(kedLastLabel);
  connect(m_kedOffsetField, &QLineEdit::returnPressed, this, &MainWindow::on_KED_offsetField_return);
  connect(m_kedLinearField, &QLineEdit::returnPressed, this, &MainWindow::on_KED_linearField_return);
  connect(m_kedQuadraticField, &QLineEdit::returnPressed, this, &MainWindow::on_KED_quadraticField_return);

  //Add the plot tools H layouts to the V layout
  m_kedPlotToolsPgVLayout->addLayout(kedGraphicsHeaderHLayout);
  m_kedPlotToolsPgVLayout->addWidget(kedGraphicsLine);
  m_kedPlotToolsPgVLayout->addItem(new QSpacerItem(1, 13, QSizePolicy::Minimum, QSizePolicy::Fixed));
  m_kedPlotToolsPgVLayout->addLayout(kedLogLinHLayout);
  m_kedPlotToolsPgVLayout->addLayout(kedFileBoxHLayout);
  m_kedPlotToolsPgVLayout->addLayout(kedFileColorHLayout);
  m_kedPlotToolsPgVLayout->addLayout(kedGridlinesHLayout);
  m_kedPlotToolsPgVLayout->addLayout(kedKEdgeLinesHLayout);
  m_kedPlotToolsPgVLayout->addLayout(kedFitCheckBoxHLayout);
  m_kedPlotToolsPgVLayout->addItem(new QSpacerItem(1, 18, QSizePolicy::Minimum, QSizePolicy::Fixed));
  m_kedPlotToolsPgVLayout->addLayout(kedEnergyCalHeaderHLayout);
  m_kedPlotToolsPgVLayout->addWidget(kedECalLine);
  m_kedPlotToolsPgVLayout->addLayout(kedEnergyCalButtonsHLayout);
  m_kedPlotToolsPgVLayout->addItem(new QSpacerItem(1, 13, QSizePolicy::Minimum, QSizePolicy::Fixed));
  m_kedPlotToolsPgVLayout->addLayout(kedECalHLayout);
  return kedPlotToolsPage;
}

QWidget* MainWindow::layout_XRF_plotTools_tab()
{
  //Plot tools page object
  QWidget* xrfPlotToolsPage = new QWidget();
  //Vertical layout will hold all the plot tools horizontal layouts
  QVBoxLayout* xrfPlotToolsPgVLayout = new QVBoxLayout(xrfPlotToolsPage);

  //Horizontal layout for Graphics header.
  QHBoxLayout* xrfGraphicsHeaderHLayout = new QHBoxLayout();
  //Graphics header.
  QLabel* xrfGraphicsLabel = new QLabel;
  xrfGraphicsLabel->setText("Graphics Options");
  xrfGraphicsLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  //add label to the H layout
  xrfGraphicsHeaderHLayout->addWidget(xrfGraphicsLabel);

  //Create a horizontal line to separate header from widgets
  QFrame* xrfGraphicsLine = new QFrame();
  xrfGraphicsLine->setFrameShape(QFrame::HLine);

  //Horizontal layout for lin/log radio buttons.
  QHBoxLayout* xrfLogLinHLayout = new QHBoxLayout();
  //lin/log widgets
  QButtonGroup* xrfLinLogGroup = new QButtonGroup(this);
  m_xrfLogButton = new QRadioButton("Log");
  m_xrfLinButton = new QRadioButton("Lin");
  xrfLinLogGroup->addButton(m_xrfLogButton);
  xrfLinLogGroup->addButton(m_xrfLinButton);
  CancelButton* xrfRescaleButton = new CancelButton();//using cancel button color scheme
  xrfRescaleButton->setText("Reset Plot");
  xrfRescaleButton->setFixedWidth(100);
  //add radio buttons to their H layout
  xrfLogLinHLayout->addWidget(m_xrfLogButton);
  xrfLogLinHLayout->addWidget(m_xrfLinButton);
  xrfLogLinHLayout->addWidget(xrfRescaleButton);
  //set radio button default states
  m_xrfLogButton->setChecked(false);
  m_xrfLinButton->setChecked(true);
  //Connect xrf log/lin buttons to slots
  connect(m_xrfLogButton, &QRadioButton::toggled, this, &MainWindow::on_XRF_log_button_toggled);
  connect(m_xrfLinButton, &QRadioButton::toggled, this, &MainWindow::on_XRF_lin_button_toggled);
  connect(xrfRescaleButton, &QPushButton::clicked, this, &MainWindow::on_XRF_reset_button_clicked);

  //Horizontal layout for sample/bkgd/ref check boxes.
  QHBoxLayout* xrfFileBoxHLayout = new QHBoxLayout();
  //sample/bkgd/ref check boxes.
  m_xrfSampCheckBox = new QCheckBox("Sample");
  m_xrfSampCheckBox->setChecked(true);
  m_xrfSampCheckBox->setDisabled(true);
  m_xrfBkgdCheckBox = new QCheckBox("Background");
  m_xrfBkgdCheckBox->setChecked(true);
  m_xrfBkgdCheckBox->setDisabled(true);
  m_xrfRefCheckBox = new QCheckBox("Reference");
  m_xrfRefCheckBox->setChecked(true);
  m_xrfRefCheckBox->setDisabled(true);
  //add check boxes to their H layout
  xrfFileBoxHLayout->addWidget(m_xrfSampCheckBox);
  xrfFileBoxHLayout->addWidget(m_xrfBkgdCheckBox);
  xrfFileBoxHLayout->addWidget(m_xrfRefCheckBox);
  //Connect ked samp check box to slot
  connect(m_xrfSampCheckBox, &QCheckBox::toggled, this, &MainWindow::on_XRF_samp_checkBox_toggled);
  connect(m_xrfBkgdCheckBox, &QCheckBox::toggled, this, &MainWindow::on_XRF_bkgd_checkBox_toggled);
  connect(m_xrfRefCheckBox,  &QCheckBox::toggled, this, &MainWindow::on_XRF_ref_checkBox_toggled);

  //Horizontal layout for sample/bkgd/ref color drop downs.
  QHBoxLayout* xrfFileColorHLayout = new QHBoxLayout();
  //sample/bkgd/ref color drop downs
  m_xrfSampComboBox = new QComboBox();
  m_xrfSampComboBox->addItem("Blue");
  m_xrfSampComboBox->addItem("Black");
  m_xrfSampComboBox->addItem("Red");
  m_xrfSampComboBox->addItem("Green");
  m_xrfSampComboBox->addItem("Cyan");
  m_xrfSampComboBox->setCurrentIndex(0);
  m_xrfSampComboBox->setDisabled(true);
  m_xrfBkgdComboBox = new QComboBox();
  m_xrfBkgdComboBox->addItem("Blue");
  m_xrfBkgdComboBox->addItem("Black");
  m_xrfBkgdComboBox->addItem("Red");
  m_xrfBkgdComboBox->addItem("Green");
  m_xrfBkgdComboBox->addItem("Cyan");
  m_xrfBkgdComboBox->setCurrentIndex(1);
  m_xrfBkgdComboBox->setDisabled(true);
  m_xrfRefComboBox = new QComboBox();
  m_xrfRefComboBox->addItem("Blue");
  m_xrfRefComboBox->addItem("Black");
  m_xrfRefComboBox->addItem("Red");
  m_xrfRefComboBox->addItem("Green");
  m_xrfRefComboBox->addItem("Cyan");
  m_xrfRefComboBox->setCurrentIndex(2);
  m_xrfRefComboBox->setDisabled(true);
  //add combo boxes to their H layout
  xrfFileColorHLayout->addWidget(m_xrfSampComboBox);
  xrfFileColorHLayout->addWidget(m_xrfBkgdComboBox);
  xrfFileColorHLayout->addWidget(m_xrfRefComboBox);
  connect(m_xrfSampComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_XRF_samp_comboBox_toggled);
  connect(m_xrfBkgdComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_XRF_bkgd_comboBox_toggled);
  connect(m_xrfRefComboBox,  QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_XRF_ref_comboBox_toggled);

  //Horizontal layout for gridlines check boxes.
  QHBoxLayout* xrfGridlinesHLayout = new QHBoxLayout();
  //x&y gridlines check boxes.
  m_xrfXGridCheckBox = new QCheckBox("X-axis Gridlines");
  m_xrfYGridCheckBox = new QCheckBox("Y-axis Gridlines");
  //add check boxes to their H layout
  xrfGridlinesHLayout->addWidget(m_xrfXGridCheckBox);
  xrfGridlinesHLayout->addWidget(m_xrfYGridCheckBox);
  //set check box default states
  m_xrfXGridCheckBox->setChecked(true);
  m_xrfYGridCheckBox->setChecked(true);
  //Connect ked log/lin buttons to slots
  connect(m_xrfXGridCheckBox, &QCheckBox::toggled, this, &MainWindow::on_XRF_x_grid_checkBox_toggled);
  connect(m_xrfYGridCheckBox, &QCheckBox::toggled, this, &MainWindow::on_XRF_y_grid_checkBox_toggled);

  //Horizontal layout for x-ray reference lines check boxes.
  QHBoxLayout* xrfCalLinesHLayout = new QHBoxLayout();
  //U&Pu x-ray lines check boxes.
  m_xrfPuXrayCheckBox = new QCheckBox("Pu x-rays");
  m_xrfUXrayCheckBox = new QCheckBox("U x-rays");
  m_xrfFissionCheckBox = new QCheckBox("Fission Products");
  //add check boxes to their H layout
  xrfCalLinesHLayout->addWidget(m_xrfPuXrayCheckBox);
  xrfCalLinesHLayout->addWidget(m_xrfUXrayCheckBox);
  xrfCalLinesHLayout->addWidget(m_xrfFissionCheckBox);
  //set check box default states
  m_xrfPuXrayCheckBox->setChecked(false);
  m_xrfUXrayCheckBox->setChecked(false);
  connect(m_xrfPuXrayCheckBox, &QCheckBox::toggled, this, &MainWindow::on_XRF_Pu_xray_checkBox_toggled);
  connect(m_xrfUXrayCheckBox, &QCheckBox::toggled, this, &MainWindow::on_XRF_U_xray_checkBox_toggled);
  connect(m_xrfFissionCheckBox, &QCheckBox::toggled, this, &MainWindow::on_XRF_fission_checkBox_toggled);

  //Horizontal layout for Energy Cal header.
  QHBoxLayout* xrfEnergyCalHeaderHLayout = new QHBoxLayout();
  //Energy Cal header.
  QLabel* xrfEnergyCalLabel = new QLabel("Energy Calibration");
  xrfEnergyCalLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  //add label to the H layout
  xrfEnergyCalHeaderHLayout->addWidget(xrfEnergyCalLabel);

  QHBoxLayout* xrfEnergyCalButtonsHLayout = new QHBoxLayout();
  m_xrfEnergyCalGroup = new QButtonGroup(this);
  m_xrfSampEcalButton = new QRadioButton("Sample");
  m_xrfSampEcalButton->setDisabled(true);
  m_xrfBkgdEcalButton = new QRadioButton("Background");
  m_xrfBkgdEcalButton->setDisabled(true);
  m_xrfRefEcalButton = new QRadioButton("Reference");
  m_xrfRefEcalButton->setDisabled(true);
  m_xrfEnergyCalGroup->addButton(m_xrfSampEcalButton,0);
  m_xrfEnergyCalGroup->addButton(m_xrfBkgdEcalButton,1);
  m_xrfEnergyCalGroup->addButton(m_xrfRefEcalButton,2);
  //add buttons to layout
  xrfEnergyCalButtonsHLayout->addWidget(m_xrfSampEcalButton);
  xrfEnergyCalButtonsHLayout->addWidget(m_xrfBkgdEcalButton);
  xrfEnergyCalButtonsHLayout->addWidget(m_xrfRefEcalButton);
  //connect(m_xrfSampEcalButton, &QRadioButton::toggled, this, &MainWindow::on_XRF_samp_Ecal_button_toggled);
  //connect(m_xrfBkgdEcalButton, &QRadioButton::toggled, this, &MainWindow::on_XRF_bkgd_Ecal_button_toggled);
  //connect(m_xrfRefEcalButton, &QRadioButton::toggled, this,  &MainWindow::on_XRF_ref_Ecal_button_toggled);
  connect(m_xrfEnergyCalGroup, &QButtonGroup::idToggled,
          this, &MainWindow::on_XRF_Ecal_group_toggled);
  //Create a horizontal line to separate header from widgets
  QFrame* xrfECalLine = new QFrame();
  xrfECalLine->setFrameShape(QFrame::HLine);

  //Horizontal layout for xrf E energy cal.
  QHBoxLayout* xrfECalHLayout = new QHBoxLayout();
  QLabel* xrfOffsetLabel = new QLabel;
  xrfOffsetLabel->setText("E (keV): ");
  m_xrfOffsetField = new UpdatableLabel;
  QLabel* xrfLinearLabel = new QLabel;
  xrfLinearLabel->setText(" + ");
  m_xrfLinearField = new UpdatableLabel;
  QLabel* xrfQuadraticLabel = new QLabel;
  xrfQuadraticLabel->setText("*ch + ");
  m_xrfQuadraticField = new UpdatableLabel;
  QLabel* xrfLastLabel = new QLabel;
  xrfLastLabel->setText("*ch<sup>2</sup>");
  xrfECalHLayout->addWidget(xrfOffsetLabel);
  xrfECalHLayout->addWidget(m_xrfOffsetField);
  xrfECalHLayout->addWidget(xrfLinearLabel);
  xrfECalHLayout->addWidget(m_xrfLinearField);
  xrfECalHLayout->addWidget(xrfQuadraticLabel);
  xrfECalHLayout->addWidget(m_xrfQuadraticField);
  xrfECalHLayout->addWidget(xrfLastLabel);
  connect(m_xrfOffsetField, &QLineEdit::returnPressed, this, &MainWindow::on_XRF_offsetField_return);
  connect(m_xrfLinearField, &QLineEdit::returnPressed, this, &MainWindow::on_XRF_linearField_return);
  connect(m_xrfQuadraticField, &QLineEdit::returnPressed, this, &MainWindow::on_XRF_quadraticField_return);

  //Add the plot tools H layouts to the V layout
  xrfPlotToolsPgVLayout->addLayout(xrfGraphicsHeaderHLayout);
  xrfPlotToolsPgVLayout->addWidget(xrfGraphicsLine);
  xrfPlotToolsPgVLayout->addItem(new QSpacerItem(1, 13, QSizePolicy::Minimum, QSizePolicy::Fixed));
  xrfPlotToolsPgVLayout->addLayout(xrfLogLinHLayout);
  xrfPlotToolsPgVLayout->addLayout(xrfFileBoxHLayout);
  xrfPlotToolsPgVLayout->addLayout(xrfFileColorHLayout);
  xrfPlotToolsPgVLayout->addLayout(xrfGridlinesHLayout);
  xrfPlotToolsPgVLayout->addLayout(xrfCalLinesHLayout);
  xrfPlotToolsPgVLayout->addItem(new QSpacerItem(1, 30, QSizePolicy::Minimum, QSizePolicy::Fixed));
  xrfPlotToolsPgVLayout->addLayout(xrfEnergyCalHeaderHLayout);
  xrfPlotToolsPgVLayout->addWidget(xrfECalLine);
  xrfPlotToolsPgVLayout->addLayout(xrfEnergyCalButtonsHLayout);
  xrfPlotToolsPgVLayout->addItem(new QSpacerItem(1, 13, QSizePolicy::Minimum, QSizePolicy::Fixed));
  xrfPlotToolsPgVLayout->addLayout(xrfECalHLayout);
  
  return xrfPlotToolsPage;
}

QWidget* MainWindow::layout_KED_plotting_area()
{
  //Plotting area object
  QWidget* kedPlottingArea = new QWidget();
  kedPlottingArea->setStyleSheet("border: 1px solid #006b75; border-radius: 8px;");
  //main vertical layout
  QVBoxLayout* kedPlottingAreaVLayout = new QVBoxLayout(kedPlottingArea);

  //horizontal layout for the plot and slider
  QHBoxLayout* kedPlottingAreaHLayout = new QHBoxLayout();
  QVBoxLayout* kedPlotVLayout = new QVBoxLayout(); //for plot itself
  QVBoxLayout* kedSliderVLayout = new QVBoxLayout();//for slider and scale box
  m_kedPlot = new QCustomPlot(this);
  QFont labelFontX = m_kedPlot->xAxis->labelFont();
  labelFontX.setPointSize(11);
  QFont labelFontY = m_kedPlot->yAxis->labelFont();
  labelFontY.setPointSize(11);
  m_kedPlot->xAxis->setLabelFont(labelFontX);
  m_kedPlot->yAxis->setLabelFont(labelFontY);
  m_kedPlot->xAxis->setLabel("Energy (keV)");
  m_kedPlot->yAxis->setLabel("Counts");
  m_kedPlot->setInteractions(QCP::iSelectItems | QCP::iRangeDrag | QCP::iRangeZoom);
  connect(m_kedPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), m_kedPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(m_kedPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), m_kedPlot->yAxis2, SLOT(setRange(QCPRange)));
  connect(m_kedPlot, SIGNAL(itemClick(QCPAbstractItem*, QMouseEvent*)),
             this, SLOT(on_KED_item_clicked(QCPAbstractItem*, QMouseEvent*)));
  // Create a vertical slider
  m_kedSlider = new QSlider(Qt::Vertical, this);
  m_kedSlider->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  m_kedSlider->setDisabled(true);
  m_kedSlider->setRange(0, 1000);
  m_kedSlider->setValue(500);
  m_kedSlider->setStyleSheet(
  "QSlider:vertical \
  {\
    border: none;\
  }\
  QSlider::handle:vertical \
  {\
    background: lightgray;\
    border: 1px solid lightgray;\
    width: 18px;\
    height: 9px;\
    margin: -5px 0;\
    border-radius: 9px;\
  }");
  connect(m_kedSlider, &QSlider::valueChanged, this, &MainWindow::on_KED_slider_changed);
  //create box to hold scale value
  m_kedScaleField = new UpdatableLabel;
  m_kedScaleField->setText("1.0");
  m_kedScaleField->setFixedWidth(35);
  m_kedScaleField->setFixedHeight(20);
  connect(m_kedScaleField, &QLineEdit::returnPressed, this, &MainWindow::on_KED_scaler_return);

  //add widgets to V layouts
  kedPlotVLayout->addWidget(m_kedPlot);
  kedSliderVLayout->addWidget(m_kedSlider, 0, Qt::AlignHCenter);
  kedSliderVLayout->addWidget(m_kedScaleField, 0, Qt::AlignHCenter);

  //add V layouts to H layout
  kedPlottingAreaHLayout->addLayout(kedPlotVLayout,1);
  kedPlottingAreaHLayout->addLayout(kedSliderVLayout,0);

  //Add the H layout to main V layout
  kedPlottingAreaVLayout->addLayout(kedPlottingAreaHLayout);
  kedPlottingAreaVLayout->addItem(new QSpacerItem(1, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));
  
  return kedPlottingArea;
}

QWidget* MainWindow::layout_XRF_plotting_area()
{
  //Plotting area object
  QWidget* xrfPlottingArea = new QWidget();
  xrfPlottingArea->setStyleSheet("border: 1px solid #006b75; border-radius: 8px;");
  //main vertical layout
  QVBoxLayout* xrfPlottingAreaVLayout = new QVBoxLayout(xrfPlottingArea);

  //horizontal layout for the plot and slider
  QHBoxLayout* xrfPlottingAreaHLayout = new QHBoxLayout();
  QVBoxLayout* xrfPlotVLayout = new QVBoxLayout(); //for plot itself
  QVBoxLayout* xrfSliderVLayout = new QVBoxLayout();//for slider and scale box
  m_xrfPlot = new QCustomPlot(this);
  QFont labelFontX = m_xrfPlot->xAxis->labelFont();
  labelFontX.setPointSize(11);
  QFont labelFontY = m_xrfPlot->yAxis->labelFont();
  labelFontY.setPointSize(11);
  m_xrfPlot->xAxis->setLabelFont(labelFontX);
  m_xrfPlot->yAxis->setLabelFont(labelFontY);
  m_xrfPlot->xAxis->setLabel("Energy (keV)");
  m_xrfPlot->yAxis->setLabel("Counts");
  m_xrfPlot->setInteractions(QCP::iSelectItems | QCP::iRangeDrag | QCP::iRangeZoom);
  connect(m_xrfPlot->xAxis, SIGNAL(rangeChanged(QCPRange)), m_xrfPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(m_xrfPlot->yAxis, SIGNAL(rangeChanged(QCPRange)), m_xrfPlot->yAxis2, SLOT(setRange(QCPRange)));
  connect(m_xrfPlot, SIGNAL(itemClick(QCPAbstractItem*, QMouseEvent*)),
             this, SLOT(on_XRF_item_clicked(QCPAbstractItem*, QMouseEvent*)));
  // Create a vertical slider
  m_xrfSlider = new QSlider(Qt::Vertical, this);
  m_xrfSlider->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
  m_xrfSlider->setDisabled(true);
  m_xrfSlider->setRange(0, 1000);
  m_xrfSlider->setValue(500);
  m_xrfSlider->setStyleSheet(
  "QSlider:vertical \
  {\
    border: none;\
  }\
  QSlider::handle:vertical \
  {\
    background: lightgray;\
    border: 1px solid lightgray;\
    width: 18px;\
    height: 9px;\
    margin: -5px 0;\
    border-radius: 9px;\
  }");
  connect(m_xrfSlider, &QSlider::valueChanged, this, &MainWindow::on_XRF_slider_changed);
  //create box to hold scale value
  m_xrfScaleField = new UpdatableLabel;
  m_xrfScaleField->setText("1.0");
  m_xrfScaleField->setFixedWidth(35);
  m_xrfScaleField->setFixedHeight(20);
  connect(m_xrfScaleField, &QLineEdit::returnPressed, this, &MainWindow::on_XRF_scaler_return);

  //add widgets to V layouts
  xrfPlotVLayout->addWidget(m_xrfPlot);
  xrfSliderVLayout->addWidget(m_xrfSlider, 0, Qt::AlignHCenter);
  xrfSliderVLayout->addWidget(m_xrfScaleField, 0, Qt::AlignHCenter);

  //add V layouts to H layout
  xrfPlottingAreaHLayout->addLayout(xrfPlotVLayout,1);
  xrfPlottingAreaHLayout->addLayout(xrfSliderVLayout,0);

  //Add the H layout to main V layout
  xrfPlottingAreaVLayout->addLayout(xrfPlottingAreaHLayout);
  xrfPlottingAreaVLayout->addItem(new QSpacerItem(1, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));
  
  return xrfPlottingArea;
}

QWidget* MainWindow::layout_results_tab()
{
  QWidget* resultsPage = new QWidget();
  QVBoxLayout* resultsPageVLayout = new QVBoxLayout(resultsPage);
  QTabWidget* resultsTabWidget = new QTabWidget();
  resultsTabWidget->addTab(layout_results_summary_tab(), "Results Summary");
  resultsTabWidget->addTab(layout_KED_results_tab(), "KED Analysis");
  resultsTabWidget->addTab(layout_XRF_results_tab(), "XRF Analysis");
  resultsPageVLayout->addWidget(resultsTabWidget);
  return resultsPage;
}

QWidget* MainWindow::layout_results_summary_tab()
{
  QWidget* resultsSummaryPage = new QWidget(m_mainTabWidget);

  //Vertical layout will hold all the horizontal layouts
  QVBoxLayout* resultsSummaryVLayout = new QVBoxLayout(resultsSummaryPage);

  //Horizontal layout for the title.
  QHBoxLayout* resultsSummaryTitleHLayout = new QHBoxLayout();
  QLabel* resultsSummaryTitleLabel = new QLabel;
  resultsSummaryTitleLabel->setText("UMAREKAWARI (Mari) v1.0");
  resultsSummaryTitleLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 15px;");
  resultsSummaryTitleHLayout->addWidget(resultsSummaryTitleLabel);
  resultsSummaryTitleHLayout->setAlignment(resultsSummaryTitleLabel, Qt::AlignCenter);

  //Horizontal layout for the subtitle.
  QHBoxLayout* resultsSummarySubtitleHLayout = new QHBoxLayout();
  QLabel* resultsSummarySubtitleLabel = new QLabel;
  resultsSummarySubtitleLabel->setText("Hybrid K-Edge Densitometry Software");
  resultsSummarySubtitleLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  resultsSummarySubtitleHLayout->addWidget(resultsSummarySubtitleLabel);
  resultsSummarySubtitleHLayout->setAlignment(resultsSummarySubtitleLabel, Qt::AlignCenter);

  //Horizontal layout for the subsubtitle.
  QHBoxLayout* resultsSummarySubsubtitleHLayout = new QHBoxLayout();
  QLabel* resultsSummarySubsubtitleLabel = new QLabel;
  resultsSummarySubsubtitleLabel->setText("Analysis Results Summary");
  resultsSummarySubsubtitleLabel->setStyleSheet("font-weight: bold; font-size: 13px;");
  resultsSummarySubsubtitleHLayout->addWidget(resultsSummarySubsubtitleLabel);
  resultsSummarySubsubtitleHLayout->setAlignment(resultsSummarySubsubtitleLabel, Qt::AlignCenter);

  //Horizontal layout for Date line.
  QHBoxLayout* resultsSummaryDateHLayout = new QHBoxLayout();
  QLabel* resultsSummaryDateLabel = new QLabel;
  resultsSummaryDateLabel->setText("Analysis Date:");
  m_resultsSummaryDateField = new NonUpdatableLabel;
  QLabel* resultsSummaryFacilityLabel = new QLabel;
  resultsSummaryFacilityLabel->setText("Facility:");
  m_resultsSummaryFacilityField = new NonUpdatableLabel;
  QLabel* resultsSummaryOperatorLabel = new QLabel;
  resultsSummaryOperatorLabel->setText("Operator:");
  m_resultsSummaryOperatorField = new NonUpdatableLabel;
  resultsSummaryDateHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryDateHLayout->addWidget(resultsSummaryDateLabel);
  resultsSummaryDateHLayout->addWidget(m_resultsSummaryDateField);
  resultsSummaryDateHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryDateHLayout->addWidget(resultsSummaryFacilityLabel);
  resultsSummaryDateHLayout->addWidget(m_resultsSummaryFacilityField);
  resultsSummaryDateHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryDateHLayout->addWidget(resultsSummaryOperatorLabel);
  resultsSummaryDateHLayout->addWidget(m_resultsSummaryOperatorField);
  resultsSummaryDateHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  
  //Horizontal layout for Comment line.
  QHBoxLayout* resultsSummaryCommentHLayout = new QHBoxLayout();
  QLabel* resultsSummaryCommentLabel = new QLabel;
  resultsSummaryCommentLabel->setText("Comment:");
  m_resultsSummaryCommentField = new NonUpdatableLabel;
  resultsSummaryCommentHLayout->addItem(new QSpacerItem(200, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryCommentHLayout->addWidget(resultsSummaryCommentLabel);
  resultsSummaryCommentHLayout->addItem(new QSpacerItem(16, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryCommentHLayout->addWidget(m_resultsSummaryCommentField);
  resultsSummaryCommentHLayout->addItem(new QSpacerItem(200, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Create a horizontal line to separate Header and Spectrum parts
  QFrame* commSpecLine = new QFrame();
  commSpecLine->setFrameShape(QFrame::HLine);
  commSpecLine->setStyleSheet("color: #006b75;");

 //Horizontal layout for KED XRF spec labels line.
  QHBoxLayout* kedXrfLabelsHLayout = new QHBoxLayout();
  QLabel* resultsSummaryKEDLabel = new QLabel;
  resultsSummaryKEDLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  resultsSummaryKEDLabel->setText("KED Spectra");
  QLabel* resultsSummaryXRFLabel = new QLabel;
  resultsSummaryXRFLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  resultsSummaryXRFLabel->setText("XRF Spectra");
  kedXrfLabelsHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedXrfLabelsHLayout->addWidget(resultsSummaryKEDLabel);
  kedXrfLabelsHLayout->addItem(new QSpacerItem(50, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedXrfLabelsHLayout->addWidget(resultsSummaryXRFLabel);
  kedXrfLabelsHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for Sample Spectrum line.
  QHBoxLayout* resultsSummarySampleHLayout = new QHBoxLayout();
  QLabel* resultsSummaryKEDSampleLabel = new QLabel;
  resultsSummaryKEDSampleLabel->setText("Sample Spectrum:");
  m_resultsSummaryKEDSampleField = new NonUpdatableLabel;
  QLabel* resultsSummaryXRFSampleLabel = new QLabel;
  resultsSummaryXRFSampleLabel->setText("Sample Spectrum:");
  m_resultsSummaryXRFSampleField = new NonUpdatableLabel;
  resultsSummarySampleHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummarySampleHLayout->addWidget(resultsSummaryKEDSampleLabel);
  resultsSummarySampleHLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummarySampleHLayout->addWidget(m_resultsSummaryKEDSampleField);
  resultsSummarySampleHLayout->addItem(new QSpacerItem(50, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummarySampleHLayout->addWidget(resultsSummaryXRFSampleLabel);
  resultsSummarySampleHLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummarySampleHLayout->addWidget(m_resultsSummaryXRFSampleField);
  resultsSummarySampleHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for Background Spectrum line.
  QHBoxLayout* resultsSummaryBkgdHLayout = new QHBoxLayout();
  QLabel* resultsSummaryKEDBkgdLabel = new QLabel;
  resultsSummaryKEDBkgdLabel->setText("Background Spectrum:");
  m_resultsSummaryKEDBkgdField = new NonUpdatableLabel;
  QLabel* resultsSummaryXRFBkgdLabel = new QLabel;
  resultsSummaryXRFBkgdLabel->setText("Background Spectrum:");
  m_resultsSummaryXRFBkgdField = new NonUpdatableLabel;
  resultsSummaryBkgdHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryBkgdHLayout->addWidget(resultsSummaryKEDBkgdLabel);
  resultsSummaryBkgdHLayout->addWidget(m_resultsSummaryKEDBkgdField);
  resultsSummaryBkgdHLayout->addItem(new QSpacerItem(50, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryBkgdHLayout->addWidget(resultsSummaryXRFBkgdLabel);
  resultsSummaryBkgdHLayout->addWidget(m_resultsSummaryXRFBkgdField);
  resultsSummaryBkgdHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for Reference Spectrum line.
  QHBoxLayout* resultsSummaryRefHLayout = new QHBoxLayout();
  QLabel* resultsSummaryKEDRefLabel = new QLabel;
  resultsSummaryKEDRefLabel->setText("Reference Spectrum:");
  m_resultsSummaryKEDRefField = new NonUpdatableLabel;
  QLabel* resultsSummaryXRFRefLabel = new QLabel;
  resultsSummaryXRFRefLabel->setText("Reference Spectrum:");
  m_resultsSummaryXRFRefField = new NonUpdatableLabel;
  resultsSummaryRefHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryRefHLayout->addWidget(resultsSummaryKEDRefLabel);
  resultsSummaryRefHLayout->addItem(new QSpacerItem(12, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryRefHLayout->addWidget(m_resultsSummaryKEDRefField);
  resultsSummaryRefHLayout->addItem(new QSpacerItem(50, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryRefHLayout->addWidget(resultsSummaryXRFRefLabel);
  resultsSummaryRefHLayout->addItem(new QSpacerItem(12, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryRefHLayout->addWidget(m_resultsSummaryXRFRefField);
  resultsSummaryRefHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for Parameter/Temp line.
  QHBoxLayout* resultsSummaryParamHLayout = new QHBoxLayout();
  QLabel* resultsSummaryParamLabel = new QLabel;
  resultsSummaryParamLabel->setText("Parameter Set:");
  m_resultsSummaryParamField = new NonUpdatableLabel;
  m_resultsSummaryParamField->setMinimumWidth(300);
  QLabel* resultsSummaryTempLabel = new QLabel;
  resultsSummaryTempLabel->setText("Sample Temp. (C):");
  m_resultsSummaryTempField = new NonUpdatableLabel;
  resultsSummaryParamHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryParamHLayout->addWidget(resultsSummaryParamLabel);
  resultsSummaryParamHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryParamHLayout->addWidget(m_resultsSummaryParamField);
  resultsSummaryParamHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryParamHLayout->addWidget(resultsSummaryTempLabel);
  resultsSummaryParamHLayout->addWidget(m_resultsSummaryTempField);
  resultsSummaryParamHLayout->addItem(new QSpacerItem(400, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for Reference line.
  QHBoxLayout* resultsSummaryReferenceHLayout = new QHBoxLayout();
  QLabel* resultsSummaryReferenceLabel = new QLabel;
  resultsSummaryReferenceLabel->setText("Reference Spectrum:");
  m_resultsSummaryReferenceField = new NonUpdatableLabel;
  resultsSummaryReferenceHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryReferenceHLayout->addWidget(resultsSummaryReferenceLabel);
  resultsSummaryReferenceHLayout->addItem(new QSpacerItem(11, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryReferenceHLayout->addWidget(m_resultsSummaryReferenceField);
  resultsSummaryReferenceHLayout->addItem(new QSpacerItem(597, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Create a horizontal line to separate Spectrum and Analysis parts
  QFrame* specAnaLine = new QFrame();
  specAnaLine->setFrameShape(QFrame::HLine);
  specAnaLine->setStyleSheet("color: #006b75;");

  //Horizontal layout for Analysis Type line.
  QHBoxLayout* resultsSummaryAnaTypeHLayout = new QHBoxLayout();
  QLabel* resultsSummaryAnaTypeLabel = new QLabel;
  resultsSummaryAnaTypeLabel->setText("Analysis Mode:");
  m_resultsSummaryAnaTypeField = new NonUpdatableLabel;
  QLabel* resultsSummaryKEDModeLabel = new QLabel;
  resultsSummaryKEDModeLabel->setText("KED Mode:");
  m_resultsSummaryKEDModeField = new NonUpdatableLabel;
  QLabel* resultsSummaryXRFModeLabel = new QLabel;
  resultsSummaryXRFModeLabel->setText("XRF Mode:");
  m_resultsSummaryXRFModeField = new NonUpdatableLabel;
  resultsSummaryAnaTypeHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryAnaTypeHLayout->addWidget(resultsSummaryAnaTypeLabel);
  resultsSummaryAnaTypeHLayout->addWidget(m_resultsSummaryAnaTypeField);
  resultsSummaryAnaTypeHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryAnaTypeHLayout->addWidget(resultsSummaryKEDModeLabel);
  resultsSummaryAnaTypeHLayout->addWidget(m_resultsSummaryKEDModeField);
  resultsSummaryAnaTypeHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryAnaTypeHLayout->addWidget(resultsSummaryXRFModeLabel);
  resultsSummaryAnaTypeHLayout->addWidget(m_resultsSummaryXRFModeField);
  resultsSummaryAnaTypeHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for Element Name line.
  QHBoxLayout* resultsSummaryElementHLayout = new QHBoxLayout();
  QLabel* resultsSummaryULabel = new QLabel;
  resultsSummaryULabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  resultsSummaryULabel->setText("U");
  QLabel* resultsSummaryNpLabel = new QLabel;
  resultsSummaryNpLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  resultsSummaryNpLabel->setText("Np");
  QLabel* resultsSummaryPuLabel = new QLabel;
  resultsSummaryPuLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  resultsSummaryPuLabel->setText("Pu");
  QLabel* resultsSummaryAmLabel = new QLabel;
  resultsSummaryAmLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  resultsSummaryAmLabel->setText("Am");
  QLabel* resultsSummaryCmLabel = new QLabel;
  resultsSummaryCmLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  resultsSummaryCmLabel->setText("Cm");
  resultsSummaryElementHLayout->addItem(new QSpacerItem(425, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryElementHLayout->addWidget(resultsSummaryULabel);
  resultsSummaryElementHLayout->addItem(new QSpacerItem(35, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryElementHLayout->addWidget(resultsSummaryNpLabel);
  resultsSummaryElementHLayout->addItem(new QSpacerItem(35, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryElementHLayout->addWidget(resultsSummaryPuLabel);
  resultsSummaryElementHLayout->addItem(new QSpacerItem(35, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryElementHLayout->addWidget(resultsSummaryAmLabel);
  resultsSummaryElementHLayout->addItem(new QSpacerItem(35, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryElementHLayout->addWidget(resultsSummaryCmLabel);
  resultsSummaryElementHLayout->addItem(new QSpacerItem(250, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for Concentration line.
  QHBoxLayout* resultsSummaryConcenHLayout = new QHBoxLayout();
  QLabel* resultsSummaryConcenLabel = new QLabel;
  resultsSummaryConcenLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  resultsSummaryConcenLabel->setText("Concentration (g/L):");
  m_resultsSummaryUConcenField  = new NonUpdatableLabel;
  m_resultsSummaryNpConcenField = new NonUpdatableLabel;
  m_resultsSummaryPuConcenField = new NonUpdatableLabel;
  m_resultsSummaryAmConcenField = new NonUpdatableLabel;
  m_resultsSummaryCmConcenField = new NonUpdatableLabel;
  resultsSummaryConcenHLayout->addItem(new QSpacerItem(250, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryConcenHLayout->addWidget(resultsSummaryConcenLabel);
  resultsSummaryConcenHLayout->addWidget(m_resultsSummaryUConcenField);
  resultsSummaryConcenHLayout->addItem(new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryConcenHLayout->addWidget(m_resultsSummaryNpConcenField);
  resultsSummaryConcenHLayout->addItem(new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryConcenHLayout->addWidget(m_resultsSummaryPuConcenField);
  resultsSummaryConcenHLayout->addItem(new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryConcenHLayout->addWidget(m_resultsSummaryAmConcenField);
  resultsSummaryConcenHLayout->addItem(new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryConcenHLayout->addWidget(m_resultsSummaryCmConcenField);
  resultsSummaryConcenHLayout->addItem(new QSpacerItem(250, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for Sigma line.
  QHBoxLayout* resultsSummarySigmaHLayout = new QHBoxLayout();
  QLabel* resultsSummarySigmaLabel = new QLabel;
  resultsSummarySigmaLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  resultsSummarySigmaLabel->setText("Sigma (g/L):");
  m_resultsSummaryUSigmaField  = new NonUpdatableLabel;
  m_resultsSummaryNpSigmaField = new NonUpdatableLabel;
  m_resultsSummaryPuSigmaField = new NonUpdatableLabel;
  m_resultsSummaryAmSigmaField = new NonUpdatableLabel;
  m_resultsSummaryCmSigmaField = new NonUpdatableLabel;
  resultsSummarySigmaHLayout->addItem(new QSpacerItem(250, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummarySigmaHLayout->addWidget(resultsSummarySigmaLabel);
  resultsSummarySigmaHLayout->addItem(new QSpacerItem(49, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummarySigmaHLayout->addWidget(m_resultsSummaryUSigmaField);
  resultsSummarySigmaHLayout->addItem(new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummarySigmaHLayout->addWidget(m_resultsSummaryNpSigmaField);
  resultsSummarySigmaHLayout->addItem(new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummarySigmaHLayout->addWidget(m_resultsSummaryPuSigmaField);
  resultsSummarySigmaHLayout->addItem(new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummarySigmaHLayout->addWidget(m_resultsSummaryAmSigmaField);
  resultsSummarySigmaHLayout->addItem(new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummarySigmaHLayout->addWidget(m_resultsSummaryCmSigmaField);
  resultsSummarySigmaHLayout->addItem(new QSpacerItem(250, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for RSD line.
  QHBoxLayout* resultsSummaryRSDHLayout = new QHBoxLayout();
  QLabel* resultsSummaryRSDLabel = new QLabel;
  resultsSummaryRSDLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  resultsSummaryRSDLabel->setText("RSD (%):");
  m_resultsSummaryURSDField  = new NonUpdatableLabel;
  m_resultsSummaryNpRSDField = new NonUpdatableLabel;
  m_resultsSummaryPuRSDField = new NonUpdatableLabel;
  m_resultsSummaryAmRSDField = new NonUpdatableLabel;
  m_resultsSummaryCmRSDField = new NonUpdatableLabel;
  resultsSummaryRSDHLayout->addItem(new QSpacerItem(250, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryRSDHLayout->addWidget(resultsSummaryRSDLabel);
  resultsSummaryRSDHLayout->addItem(new QSpacerItem(72, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryRSDHLayout->addWidget(m_resultsSummaryURSDField);
  resultsSummaryRSDHLayout->addItem(new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryRSDHLayout->addWidget(m_resultsSummaryNpRSDField);
  resultsSummaryRSDHLayout->addItem(new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryRSDHLayout->addWidget(m_resultsSummaryPuRSDField);
  resultsSummaryRSDHLayout->addItem(new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryRSDHLayout->addWidget(m_resultsSummaryAmRSDField);
  resultsSummaryRSDHLayout->addItem(new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryRSDHLayout->addWidget(m_resultsSummaryCmRSDField);
  resultsSummaryRSDHLayout->addItem(new QSpacerItem(250, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Add the H layouts to the V layout
  resultsSummaryVLayout->addItem(new QSpacerItem(1, 15, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryVLayout->addLayout(resultsSummaryTitleHLayout);
  resultsSummaryVLayout->addLayout(resultsSummarySubtitleHLayout);
  resultsSummaryVLayout->addLayout(resultsSummarySubsubtitleHLayout);
  resultsSummaryVLayout->addItem(new QSpacerItem(1, 50, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryVLayout->addLayout(resultsSummaryDateHLayout);
  resultsSummaryVLayout->addItem(new QSpacerItem(1, 10, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryVLayout->addLayout(resultsSummaryCommentHLayout);
  resultsSummaryVLayout->addItem(new QSpacerItem(1, 25, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryVLayout->addWidget(commSpecLine);
  resultsSummaryVLayout->addItem(new QSpacerItem(1, 10, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryVLayout->addLayout(kedXrfLabelsHLayout);
  resultsSummaryVLayout->addLayout(resultsSummarySampleHLayout);
  resultsSummaryVLayout->addLayout(resultsSummaryBkgdHLayout);
  resultsSummaryVLayout->addLayout(resultsSummaryRefHLayout);
  resultsSummaryVLayout->addItem(new QSpacerItem(1, 25, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryVLayout->addWidget(specAnaLine);
  resultsSummaryVLayout->addItem(new QSpacerItem(1, 25, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryVLayout->addLayout(resultsSummaryParamHLayout);
  resultsSummaryVLayout->addItem(new QSpacerItem(1, 15, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryVLayout->addLayout(resultsSummaryAnaTypeHLayout);
  resultsSummaryVLayout->addItem(new QSpacerItem(1, 25, QSizePolicy::Fixed, QSizePolicy::Minimum));
  resultsSummaryVLayout->addLayout(resultsSummaryElementHLayout);
  resultsSummaryVLayout->addLayout(resultsSummaryConcenHLayout);
  resultsSummaryVLayout->addLayout(resultsSummarySigmaHLayout);
  resultsSummaryVLayout->addLayout(resultsSummaryRSDHLayout);
  resultsSummaryVLayout->addItem(new QSpacerItem(1, 100, QSizePolicy::Fixed, QSizePolicy::Minimum));

  return resultsSummaryPage;
}

QWidget* MainWindow::layout_KED_results_tab()
{
  QWidget* kedResultsPage = new QWidget();
  QVBoxLayout* kedResultsPageVLayout = new QVBoxLayout(kedResultsPage);
  QTabWidget* kedResultsTabWidget = new QTabWidget();
  kedResultsTabWidget->addTab(layout_KED_results_plot_tab(), "Plot");
  kedResultsTabWidget->addTab(layout_KED_results_ceres_tab(), "Solver");
  kedResultsPageVLayout->addWidget(kedResultsTabWidget);
  
  return kedResultsPage;
}

QWidget* MainWindow::layout_KED_results_plot_tab()
{
  QWidget* KEDResultsPlotPage = new QWidget(m_mainTabWidget);

  /*
           KEDResultsWidgetsHLayout
           |--kedResultsPlotArea
           |   |--KEDResultsPlotsVLayout
           |       |--KEDResultsPlotHLayout
           |       |   |--m_kedResultsPlot
           |       |--KEDResidualsPlotHLayout
           |           |--m_kedResidualsPlot
           |--kedResultsWidgetArea
              |--KEDResultsButtonsVLayout
  */

  //Main vertical layout will hold all the horizontal layouts
  QVBoxLayout* KEDResultsVLayout = new QVBoxLayout(KEDResultsPlotPage);

  //Plot area object
  QWidget* kedResultsPlotArea = new QWidget();
  kedResultsPlotArea->setObjectName("kedResultsPlotArea");
  kedResultsPlotArea->setStyleSheet(
    "#kedResultsPlotArea { border: 1px solid #006b75; border-radius: 8px;}");

  //Widget area object
  QWidget* kedResultsWidgetArea = new QWidget();
  kedResultsWidgetArea->setObjectName("kedResultsWidgetArea");
  kedResultsWidgetArea->setStyleSheet(
    "#kedResultsWidgetArea { border: 1px solid #006b75; border-radius: 8px; background-color: #f8f9fa}");

  //Vertical layout for the plots and buttons areas
  QVBoxLayout* KEDResultsWidgetsVLayout = new QVBoxLayout();

  // Vertical layout inside the bordered plot area
  QVBoxLayout* KEDResultsPlotsVLayout = new QVBoxLayout(kedResultsPlotArea);

  // Vertical layout inside the bordered widget/button area
  QVBoxLayout* KEDResultsButtonsVLayout = new QVBoxLayout(kedResultsWidgetArea);

  //Horizontal layout for the title.
  QHBoxLayout* KEDResultsTitleHLayout = new QHBoxLayout();
  QLabel* KEDResultsTitleLabel = new QLabel;
  KEDResultsTitleLabel->setText("UMAREKAWARI (Mari) v1.0");
  KEDResultsTitleLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 15px;");
  KEDResultsTitleHLayout->addWidget(KEDResultsTitleLabel);
  KEDResultsTitleHLayout->setAlignment(KEDResultsTitleLabel, Qt::AlignCenter);

  //Horizontal layout for the subtitle.
  QHBoxLayout* KEDResultsSubtitleHLayout = new QHBoxLayout();
  QLabel* KEDResultsSubtitleLabel = new QLabel;
  KEDResultsSubtitleLabel->setText("Hybrid K-Edge Densitometry Software");
  KEDResultsSubtitleLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  KEDResultsSubtitleHLayout->addWidget(KEDResultsSubtitleLabel);
  KEDResultsSubtitleHLayout->setAlignment(KEDResultsSubtitleLabel, Qt::AlignCenter);

  //Horizontal layout for the subsubtitle.
  QHBoxLayout* KEDResultsSubsubtitleHLayout = new QHBoxLayout();
  QLabel* KEDResultsSubsubtitleLabel = new QLabel;
  KEDResultsSubsubtitleLabel->setText("KED Analysis Results");
  KEDResultsSubsubtitleLabel->setStyleSheet("font-weight: bold; font-size: 13px;");
  KEDResultsSubsubtitleHLayout->addWidget(KEDResultsSubsubtitleLabel);
  KEDResultsSubsubtitleHLayout->setAlignment(KEDResultsSubsubtitleLabel, Qt::AlignCenter);

  //Horizontal layout for results plot.
  QHBoxLayout* KEDResultsPlotHLayout = new QHBoxLayout();
  m_kedResultsPlot = new QCustomPlot(this);
  QFont labelFontX = m_kedResultsPlot->xAxis->labelFont();
  labelFontX.setPointSize(11);
  QFont labelFontY = m_kedResultsPlot->yAxis->labelFont();
  labelFontY.setPointSize(11);
  m_kedResultsPlot->yAxis->setLabelFont(labelFontY);
  m_kedResultsPlot->xAxis->setTickLabels(false);
  m_kedResultsPlot->xAxis->setLabel("");
  m_kedResultsPlot->yAxis->setLabel("Counts");
  m_kedResultsPlot->setInteractions(QCP::iSelectItems | QCP::iRangeDrag | QCP::iRangeZoom);
  KEDResultsPlotHLayout->addWidget(m_kedResultsPlot);

  //Horizontal layout for residuals plot.
  QHBoxLayout* KEDResidualsPlotHLayout = new QHBoxLayout();
  m_kedResidualsPlot = new QCustomPlot(this);
  m_kedResidualsPlot->xAxis->setLabelFont(labelFontX);
  m_kedResidualsPlot->yAxis->setLabelFont(labelFontY);
  m_kedResidualsPlot->xAxis->setLabel("Energy (keV)");
  m_kedResidualsPlot->yAxis->setLabel("Residual");
  m_kedResidualsPlot->setInteractions(QCP::iSelectItems | QCP::iRangeDrag | QCP::iRangeZoom);
  KEDResidualsPlotHLayout->addWidget(m_kedResidualsPlot);

  //Horizontal layout for Reset/lin/log plot button
  QHBoxLayout* kedResultsRescaleHLayout = new QHBoxLayout();
  CancelButton* kedResultsRescaleButton = new CancelButton();//using cancel button color scheme
  kedResultsRescaleButton->setText("Reset Plot");
  kedResultsRescaleButton->setFixedWidth(100);
  QButtonGroup* kedResultsLinLogGroup = new QButtonGroup(this);
  m_kedResultsLogButton = new QRadioButton("Log");
  m_kedResultsLinButton = new QRadioButton("Lin");
  kedResultsLinLogGroup->addButton(m_kedResultsLogButton);
  kedResultsLinLogGroup->addButton(m_kedResultsLinButton);
  //add buttons to their H layout
  kedResultsRescaleHLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
  kedResultsRescaleHLayout->addWidget(kedResultsRescaleButton);
  kedResultsRescaleHLayout->addItem(new QSpacerItem(150, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedResultsRescaleHLayout->addWidget(m_kedResultsLogButton);
  kedResultsRescaleHLayout->addItem(new QSpacerItem(50, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedResultsRescaleHLayout->addWidget(m_kedResultsLinButton);
  kedResultsRescaleHLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
  m_kedResultsLogButton->setChecked(false);
  m_kedResultsLinButton->setChecked(true);
  //Connect ked log/lin buttons to slots
  connect(kedResultsRescaleButton, &QPushButton::clicked, this, &MainWindow::on_KED_results_reset_button_clicked);
  connect(m_kedResultsLogButton, &QRadioButton::toggled, this, &MainWindow::on_KED_results_log_button_toggled);
  connect(m_kedResultsLinButton, &QRadioButton::toggled, this, &MainWindow::on_KED_results_lin_button_toggled);

  //Horizontal layout for check boxes.
  QHBoxLayout* kedResultsFitHLayout = new QHBoxLayout();
  m_kedResultsFitCheckBox = new QCheckBox("Total Fit");
  m_kedResultsContCheckBox = new QCheckBox("Step Continuum");
  m_kedResultsFitCheckBox->setChecked(true);
  m_kedResultsContCheckBox->setChecked(true);
  connect(m_kedResultsFitCheckBox, &QCheckBox::toggled, this, &MainWindow::on_KED_results_fit_checkBox_toggled);
  connect(m_kedResultsContCheckBox, &QCheckBox::toggled, this, &MainWindow::on_KED_results_continuum_checkBox_toggled);
  m_kedResultsLASCheckBox = new QCheckBox("Low Angle Scattering");
  m_kedResultsBSCheckBox = new QCheckBox("Backscattering");
  m_kedResultsLASCheckBox->setChecked(true);
  m_kedResultsBSCheckBox->setChecked(true);
  //connect(m_kedResultsLASCheckBox, &QCheckBox::toggled, this, &MainWindow::on_XRF_Pu_xray_checkBox_toggled);
  //connect(m_kedResultsBSCheckBox, &QCheckBox::toggled, this, &MainWindow::on_XRF_U_xray_checkBox_toggled);
  m_kedResultsTailCheckBox = new QCheckBox("Tailing");
  m_kedResultsCoincCheckBox = new QCheckBox("Coincidence Summing");
  m_kedResultsTailCheckBox->setChecked(true);
  m_kedResultsCoincCheckBox->setChecked(true);
  //connect(m_kedResultsTailCheckBox, &QCheckBox::toggled, this, &MainWindow::on_XRF_Pu_xray_checkBox_toggled);
  //connect(m_kedResultsCoincCheckBox, &QCheckBox::toggled, this, &MainWindow::on_XRF_U_xray_checkBox_toggled);
  //add check boxes to their H layout
  kedResultsFitHLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
  kedResultsFitHLayout->addWidget(m_kedResultsFitCheckBox);
  kedResultsFitHLayout->addItem(new QSpacerItem(30, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedResultsFitHLayout->addWidget(m_kedResultsContCheckBox);
  kedResultsFitHLayout->addItem(new QSpacerItem(30, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedResultsFitHLayout->addWidget(m_kedResultsLASCheckBox);
  kedResultsFitHLayout->addItem(new QSpacerItem(30, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedResultsFitHLayout->addWidget(m_kedResultsBSCheckBox);
  kedResultsFitHLayout->addItem(new QSpacerItem(30, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedResultsFitHLayout->addWidget(m_kedResultsTailCheckBox);
  kedResultsFitHLayout->addItem(new QSpacerItem(30, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedResultsFitHLayout->addWidget(m_kedResultsCoincCheckBox);
  kedResultsFitHLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

  // Make plot areas align
  auto* marginGroup = new QCPMarginGroup(m_kedResultsPlot);
  m_kedResultsPlot->axisRect()->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);
  m_kedResidualsPlot->axisRect()->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);

  // Sync top/bottom secondary axes too
  connect(m_kedResultsPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),
          m_kedResultsPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(m_kedResidualsPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),
          m_kedResidualsPlot->xAxis2, SLOT(setRange(QCPRange)));

  // Sync main and residual x ranges
  connect(m_kedResultsPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),
          m_kedResidualsPlot->xAxis, SLOT(setRange(QCPRange)));
  connect(m_kedResidualsPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),
          m_kedResultsPlot->xAxis, SLOT(setRange(QCPRange)));

  // Replot the other widget when one range changes
  connect(m_kedResultsPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),
          m_kedResidualsPlot, SLOT(replot()));

  connect(m_kedResidualsPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),
          m_kedResultsPlot, SLOT(replot()));

  m_kedResultsPlot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  m_kedResidualsPlot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  //Add the plot layouts inside the bordered plot-area widget
  KEDResultsPlotsVLayout->addLayout(KEDResultsPlotHLayout, 3);
  KEDResultsPlotsVLayout->addLayout(KEDResidualsPlotHLayout, 1);

  //Add the button widgets layouts to button V layout to layout
  KEDResultsButtonsVLayout->addLayout(kedResultsRescaleHLayout);
  KEDResultsButtonsVLayout->addLayout(kedResultsFitHLayout);

  //Add the bordered widgets to the outer horizontal layout
  KEDResultsWidgetsVLayout->addWidget(kedResultsWidgetArea, 1);
  KEDResultsWidgetsVLayout->addWidget(kedResultsPlotArea, 5);
  
  //Add the titles and widget area layouts to the main V layout
  KEDResultsVLayout->addItem(new QSpacerItem(1, 10, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDResultsVLayout->addLayout(KEDResultsTitleHLayout);
  KEDResultsVLayout->addLayout(KEDResultsSubtitleHLayout);
  KEDResultsVLayout->addLayout(KEDResultsSubsubtitleHLayout);
  KEDResultsVLayout->addItem(new QSpacerItem(1, 30, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDResultsVLayout->addLayout(KEDResultsWidgetsVLayout);
  KEDResultsVLayout->addItem(new QSpacerItem(1, 10, QSizePolicy::Fixed, QSizePolicy::Minimum));

  return KEDResultsPlotPage;
}

QWidget* MainWindow::layout_KED_results_ceres_tab()
{
  QWidget* KEDResultsCeresPage = new QWidget(m_mainTabWidget);

  //Vertical layout will hold all the horizontal layouts
  QVBoxLayout* KEDFitDetailsVLayout = new QVBoxLayout(KEDResultsCeresPage);

  //Horizontal layout for the title.
  QHBoxLayout* KEDFitDetailsTitleHLayout = new QHBoxLayout();
  QLabel* KEDFitDetailsTitleLabel = new QLabel;
  KEDFitDetailsTitleLabel->setText("UMAREKAWARI (Mari) v1.0");
  KEDFitDetailsTitleLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 15px;");
  KEDFitDetailsTitleHLayout->addWidget(KEDFitDetailsTitleLabel);
  KEDFitDetailsTitleHLayout->setAlignment(KEDFitDetailsTitleLabel, Qt::AlignCenter);

  //Horizontal layout for the subtitle.
  QHBoxLayout* KEDFitDetailsSubtitleHLayout = new QHBoxLayout();
  QLabel* KEDFitDetailsSubtitleLabel = new QLabel;
  KEDFitDetailsSubtitleLabel->setText("Hybrid K-Edge Densitometry Software");
  KEDFitDetailsSubtitleLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  KEDFitDetailsSubtitleHLayout->addWidget(KEDFitDetailsSubtitleLabel);
  KEDFitDetailsSubtitleHLayout->setAlignment(KEDFitDetailsSubtitleLabel, Qt::AlignCenter);

  //Horizontal layout for the subsubtitle.
  QHBoxLayout* KEDFitDetailsSubsubtitleHLayout = new QHBoxLayout();
  QLabel* KEDFitDetailsSubsubtitleLabel = new QLabel;
  KEDFitDetailsSubsubtitleLabel->setText("Linear Solver Performance and Fit Details");
  KEDFitDetailsSubsubtitleLabel->setStyleSheet("font-weight: bold; font-size: 13px;");
  KEDFitDetailsSubsubtitleHLayout->addWidget(KEDFitDetailsSubsubtitleLabel);
  KEDFitDetailsSubsubtitleHLayout->setAlignment(KEDFitDetailsSubsubtitleLabel, Qt::AlignCenter);

  //Horizontal layout for Algorithm & Convergence title.
  QHBoxLayout* KEDFitDetailsConvTitleHLayout = new QHBoxLayout();
  QLabel* KEDFitDetailsConvTitleLabel = new QLabel;
  KEDFitDetailsConvTitleLabel->setText("Algorithm & Convergence");
  KEDFitDetailsConvTitleLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  KEDFitDetailsConvTitleHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsConvTitleHLayout->addWidget(KEDFitDetailsConvTitleLabel);
  KEDFitDetailsConvTitleHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for termination line.
  QHBoxLayout* KEDFitDetailsTermHLayout = new QHBoxLayout();
  QLabel* KEDFitDetailsAlgoLabel = new QLabel;
  KEDFitDetailsAlgoLabel->setText("Algorithm Type:");
  m_KEDFitDetailsAlgoField = new NonUpdatableLabel;
  QLabel* KEDFitDetailsTermLabel = new QLabel;
  KEDFitDetailsTermLabel->setText("Termination Type:");
  m_KEDFitDetailsTermField = new NonUpdatableLabel;
  QLabel* KEDFitDetailsMsgLabel = new QLabel;
  KEDFitDetailsMsgLabel->setText("Message:");
  m_KEDFitDetailsMsgField = new NonUpdatableLabel;
  KEDFitDetailsTermHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsTermHLayout->addWidget(KEDFitDetailsAlgoLabel);
  KEDFitDetailsTermHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsTermHLayout->addWidget(m_KEDFitDetailsAlgoField);
  KEDFitDetailsTermHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsTermHLayout->addWidget(KEDFitDetailsTermLabel);
  KEDFitDetailsTermHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsTermHLayout->addWidget(m_KEDFitDetailsTermField);
  KEDFitDetailsTermHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsTermHLayout->addWidget(KEDFitDetailsMsgLabel);
  KEDFitDetailsTermHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsTermHLayout->addWidget(m_KEDFitDetailsMsgField);
  KEDFitDetailsTermHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for Cost and Residuals title.
  QHBoxLayout* KEDFitDetailsCostTitleHLayout = new QHBoxLayout();
  QLabel* KEDFitDetailsCostTitleLabel = new QLabel;
  KEDFitDetailsCostTitleLabel->setText("Cost and Residuals");
  KEDFitDetailsCostTitleLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  KEDFitDetailsCostTitleHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsCostTitleHLayout->addWidget(KEDFitDetailsCostTitleLabel);
  KEDFitDetailsCostTitleHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for cost line.
  QHBoxLayout* KEDFitDetailsCostHLayout = new QHBoxLayout();
  QLabel* KEDFitDetailsInitCostLabel = new QLabel;
  KEDFitDetailsInitCostLabel->setText("Initial Cost:");
  m_KEDFitDetailsInitCostField = new NonUpdatableLabel;
  QLabel* KEDFitDetailsFinalCostLabel = new QLabel;
  KEDFitDetailsFinalCostLabel->setText("Final Cost:");
  m_KEDFitDetailsFinalCostField = new NonUpdatableLabel;
  QLabel* KEDFitDetailsCostDropLabel = new QLabel;
  KEDFitDetailsCostDropLabel->setText("Cost Drop:");
  m_KEDFitDetailsCostDropField = new NonUpdatableLabel;
  KEDFitDetailsCostHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsCostHLayout->addWidget(KEDFitDetailsInitCostLabel);
  KEDFitDetailsCostHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsCostHLayout->addWidget(m_KEDFitDetailsInitCostField);
  KEDFitDetailsCostHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsCostHLayout->addWidget(KEDFitDetailsFinalCostLabel);
  KEDFitDetailsCostHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsCostHLayout->addWidget(m_KEDFitDetailsFinalCostField);
  KEDFitDetailsCostHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsCostHLayout->addWidget(KEDFitDetailsCostDropLabel);
  KEDFitDetailsCostHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsCostHLayout->addWidget(m_KEDFitDetailsCostDropField);
  KEDFitDetailsCostHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for residuals line.
  QHBoxLayout* KEDFitDetailsResidualHLayout = new QHBoxLayout();
  QLabel* KEDFitDetailsResidualLabel = new QLabel;
  KEDFitDetailsResidualLabel->setText("Number of Residuals Evals:");
  m_KEDFitDetailsResidualField = new NonUpdatableLabel;
  QLabel* KEDFitDetailsJacobianLabel = new QLabel;
  KEDFitDetailsJacobianLabel->setText("Number of Jacobian Evals:");
  m_KEDFitDetailsJacobianField = new NonUpdatableLabel;
  KEDFitDetailsResidualHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsResidualHLayout->addWidget(KEDFitDetailsResidualLabel);
  KEDFitDetailsResidualHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsResidualHLayout->addWidget(m_KEDFitDetailsResidualField);
  KEDFitDetailsResidualHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsResidualHLayout->addWidget(KEDFitDetailsJacobianLabel);
  KEDFitDetailsResidualHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsResidualHLayout->addWidget(m_KEDFitDetailsJacobianField);
  KEDFitDetailsResidualHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for Execution Performance & Iterations title.
  QHBoxLayout* KEDFitDetailsPerformTitleHLayout = new QHBoxLayout();
  QLabel* KEDFitDetailsPerformTitleLabel = new QLabel;
  KEDFitDetailsPerformTitleLabel->setText("Execution Performance & Iterations");
  KEDFitDetailsPerformTitleLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  KEDFitDetailsPerformTitleHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsPerformTitleHLayout->addWidget(KEDFitDetailsPerformTitleLabel);
  KEDFitDetailsPerformTitleHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for performance line.
  QHBoxLayout* KEDFitDetailsPerformHLayout = new QHBoxLayout();
  QLabel* KEDFitDetailsTotalItLabel = new QLabel;
  KEDFitDetailsTotalItLabel->setText("Total Iterations:");
  m_KEDFitDetailsTotalItField = new NonUpdatableLabel;
  QLabel* KEDFitDetailsSuccItLabel = new QLabel;
  KEDFitDetailsSuccItLabel->setText("Successful Iterations:");
  m_KEDFitDetailsSuccItField = new NonUpdatableLabel;
  QLabel* KEDFitDetailsUnsuccItLabel = new QLabel;
  KEDFitDetailsUnsuccItLabel->setText("Unsuccessful Iterations:");
  m_KEDFitDetailsUnsuccItField = new NonUpdatableLabel;
  KEDFitDetailsPerformHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsPerformHLayout->addWidget(KEDFitDetailsTotalItLabel);
  KEDFitDetailsPerformHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsPerformHLayout->addWidget(m_KEDFitDetailsTotalItField);
  KEDFitDetailsPerformHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsPerformHLayout->addWidget(KEDFitDetailsSuccItLabel);
  KEDFitDetailsPerformHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsPerformHLayout->addWidget(m_KEDFitDetailsSuccItField);
  KEDFitDetailsPerformHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsPerformHLayout->addWidget(KEDFitDetailsUnsuccItLabel);
  KEDFitDetailsPerformHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsPerformHLayout->addWidget(m_KEDFitDetailsUnsuccItField);
  KEDFitDetailsPerformHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for time line.
  QHBoxLayout* KEDFitDetailsTimeHLayout = new QHBoxLayout();
  QLabel* KEDFitDetailsTotalTimeLabel = new QLabel;
  KEDFitDetailsTotalTimeLabel->setText("Total Time (s):");
  m_KEDFitDetailsTotalTimeField = new NonUpdatableLabel;
  QLabel* KEDFitDetailsJacobTimeLabel = new QLabel;
  KEDFitDetailsJacobTimeLabel->setText("Jacobian Eval. Time (s):");
  m_KEDFitDetailsJacobTimeField = new NonUpdatableLabel;
  QLabel* KEDFitDetailsLinSolveTimeLabel = new QLabel;
  KEDFitDetailsLinSolveTimeLabel->setText("Linear Solver Time (s):");
  m_KEDFitDetailsLinSolveTimeField = new NonUpdatableLabel;
  KEDFitDetailsTimeHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsTimeHLayout->addWidget(KEDFitDetailsTotalTimeLabel);
  KEDFitDetailsTimeHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsTimeHLayout->addWidget(m_KEDFitDetailsTotalTimeField);
  KEDFitDetailsTimeHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsTimeHLayout->addWidget(KEDFitDetailsJacobTimeLabel);
  KEDFitDetailsTimeHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsTimeHLayout->addWidget(m_KEDFitDetailsJacobTimeField);
  KEDFitDetailsTimeHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsTimeHLayout->addWidget(KEDFitDetailsLinSolveTimeLabel);
  KEDFitDetailsTimeHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsTimeHLayout->addWidget(m_KEDFitDetailsLinSolveTimeField);
  KEDFitDetailsTimeHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for free parameter table.
  QHBoxLayout* KEDFitDetailsFreeTableHLayout = new QHBoxLayout();
  // Create a table with 4 rows and 4 columns
  QTableWidget *tableWidget = new QTableWidget(4, 4, this);
  //Hide the default built-in horizontal header
  tableWidget->horizontalHeader()->setVisible(false);
  //Merge the top row (Row 0) into two separate dual-column headers
  //Arguments: setSpan(targetRow, targetColumn, rowSpan, columnSpan)
  tableWidget->setSpan(0, 0, 1, 2); // Merges Col 0 and Col 1
  tableWidget->setSpan(0, 2, 1, 2); // Merges Col 2 and Col 3
  //Create and style Header 1 (Personal Info)
  QTableWidgetItem *header1 = new QTableWidgetItem("Bremsstrahlung Shape");
  header1->setTextAlignment(Qt::AlignCenter);
  header1->setFlags(header1->flags() & ~Qt::ItemIsEditable); // Read-only
  header1->setBackground(Qt::lightGray); 
  tableWidget->setItem(0, 0, header1); // Must place in top-left cell of the span
  //Create and style Header 2 (Contact Info)
  QTableWidgetItem *header2 = new QTableWidgetItem("Sample Densities");
  header2->setTextAlignment(Qt::AlignCenter);
  header2->setFlags(header2->flags() & ~Qt::ItemIsEditable); // Read-only
  header2->setBackground(Qt::lightGray); 
  tableWidget->setItem(0, 2, header2); // Must place in top-left cell of the span
  //Populate Row 1 (Sub-headers for the 4 individual columns)
  tableWidget->setItem(1, 0, new QTableWidgetItem("Scaling Constant (C)"));
  tableWidget->setItem(1, 2, new QTableWidgetItem("Rho U"));
  //Populate Row 2 (Example Data)
  tableWidget->setItem(2, 0, new QTableWidgetItem("Shape Constatnt (X)"));
  tableWidget->setItem(2, 2, new QTableWidgetItem("Rho Pu"));
  tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
  tableWidget->resizeRowsToContents();
  int h = tableWidget->horizontalHeader()->height() +
          tableWidget->verticalHeader()->length() +
          2 * tableWidget->frameWidth();
  tableWidget->setFixedHeight(h);
  tableWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

  KEDFitDetailsFreeTableHLayout->addWidget(tableWidget);

  //Add the H layouts to the V layout
  KEDFitDetailsVLayout->addLayout(KEDFitDetailsTitleHLayout);
  KEDFitDetailsVLayout->addLayout(KEDFitDetailsSubtitleHLayout);
  KEDFitDetailsVLayout->addLayout(KEDFitDetailsSubsubtitleHLayout);
  KEDFitDetailsVLayout->addItem(new QSpacerItem(1, 30, QSizePolicy::Fixed, QSizePolicy::Minimum));
  KEDFitDetailsVLayout->addLayout(KEDFitDetailsConvTitleHLayout);
  KEDFitDetailsVLayout->addLayout(KEDFitDetailsTermHLayout);
  KEDFitDetailsVLayout->addLayout(KEDFitDetailsCostTitleHLayout);
  KEDFitDetailsVLayout->addLayout(KEDFitDetailsCostHLayout);
  KEDFitDetailsVLayout->addLayout(KEDFitDetailsResidualHLayout);
  KEDFitDetailsVLayout->addLayout(KEDFitDetailsPerformTitleHLayout);
  KEDFitDetailsVLayout->addLayout(KEDFitDetailsPerformHLayout);
  KEDFitDetailsVLayout->addLayout(KEDFitDetailsTimeHLayout);
  KEDFitDetailsVLayout->addLayout(KEDFitDetailsFreeTableHLayout);
  KEDFitDetailsVLayout->addStretch(1);

  return KEDResultsCeresPage;
}

QWidget* MainWindow::layout_XRF_results_tab()
{
  QWidget* xrfResultsPage = new QWidget();
  QVBoxLayout* xrfResultsPageVLayout = new QVBoxLayout(xrfResultsPage);
  QTabWidget* xrfResultsTabWidget = new QTabWidget();
  xrfResultsTabWidget->addTab(layout_XRF_results_plot_tab(), "Plot");
  xrfResultsTabWidget->addTab(layout_XRF_results_ceres_tab(), "Solver");
  xrfResultsPageVLayout->addWidget(xrfResultsTabWidget);
  
  return xrfResultsPage;
}

QWidget* MainWindow::layout_XRF_results_plot_tab()
{
  QWidget* XRFResultsPlotPage = new QWidget(m_mainTabWidget);

  /*
           XRFResultsWidgetsHLayout
           |--xrfResultsPlotArea
           |   |--XRFResultsPlotsVLayout
           |       |--XRFResultsPlotHLayout
           |       |   |--m_xrfResultsPlot
           |       |--XRFResidualsPlotHLayout
           |           |--m_xrfResidualsPlot
           |--xrfResultsWidgetArea
              |--XRFResultsButtonsVLayout
  */

  //Main vertical layout will hold all the horizontal layouts
  QVBoxLayout* XRFResultsVLayout = new QVBoxLayout(XRFResultsPlotPage);

  //Plot area object
  QWidget* xrfResultsPlotArea = new QWidget();
  xrfResultsPlotArea->setObjectName("xrfResultsPlotArea");
  xrfResultsPlotArea->setStyleSheet(
    "#xrfResultsPlotArea { border: 1px solid #006b75; border-radius: 8px;}");

  //Widget area object
  QWidget* xrfResultsWidgetArea = new QWidget();
  xrfResultsWidgetArea->setObjectName("xrfResultsWidgetArea");
  xrfResultsWidgetArea->setStyleSheet(
    "#xrfResultsWidgetArea { border: 1px solid #006b75; border-radius: 8px; background-color: #f8f9fa}");

  //Vertical layout for the plots and buttons areas
  QVBoxLayout* XRFResultsWidgetsVLayout = new QVBoxLayout();

  // Vertical layout inside the bordered plot area
  QVBoxLayout* XRFResultsPlotsVLayout = new QVBoxLayout(xrfResultsPlotArea);

  // Vertical layout inside the bordered widget/button area
  QVBoxLayout* XRFResultsButtonsVLayout = new QVBoxLayout(xrfResultsWidgetArea);

  //Horizontal layout for the title.
  QHBoxLayout* XRFResultsTitleHLayout = new QHBoxLayout();
  QLabel* XRFResultsTitleLabel = new QLabel;
  XRFResultsTitleLabel->setText("UMAREKAWARI (Mari) v1.0");
  XRFResultsTitleLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 15px;");
  XRFResultsTitleHLayout->addWidget(XRFResultsTitleLabel);
  XRFResultsTitleHLayout->setAlignment(XRFResultsTitleLabel, Qt::AlignCenter);

  //Horizontal layout for the subtitle.
  QHBoxLayout* XRFResultsSubtitleHLayout = new QHBoxLayout();
  QLabel* XRFResultsSubtitleLabel = new QLabel;
  XRFResultsSubtitleLabel->setText("Hybrid K-Edge Densitometry Software");
  XRFResultsSubtitleLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  XRFResultsSubtitleHLayout->addWidget(XRFResultsSubtitleLabel);
  XRFResultsSubtitleHLayout->setAlignment(XRFResultsSubtitleLabel, Qt::AlignCenter);

  //Horizontal layout for the subsubtitle.
  QHBoxLayout* XRFResultsSubsubtitleHLayout = new QHBoxLayout();
  QLabel* XRFResultsSubsubtitleLabel = new QLabel;
  XRFResultsSubsubtitleLabel->setText("XRF Analysis Results");
  XRFResultsSubsubtitleLabel->setStyleSheet("font-weight: bold; font-size: 13px;");
  XRFResultsSubsubtitleHLayout->addWidget(XRFResultsSubsubtitleLabel);
  XRFResultsSubsubtitleHLayout->setAlignment(XRFResultsSubsubtitleLabel, Qt::AlignCenter);

  //Horizontal layout for results plot.
  QHBoxLayout* XRFResultsPlotHLayout = new QHBoxLayout();
  m_xrfResultsPlot = new QCustomPlot(this);
  QFont labelFontX = m_xrfResultsPlot->xAxis->labelFont();
  labelFontX.setPointSize(11);
  QFont labelFontY = m_xrfResultsPlot->yAxis->labelFont();
  labelFontY.setPointSize(11);
  m_xrfResultsPlot->yAxis->setLabelFont(labelFontY);
  m_xrfResultsPlot->xAxis->setTickLabels(false);
  m_xrfResultsPlot->xAxis->setLabel("");
  m_xrfResultsPlot->yAxis->setLabel("Counts");
  m_xrfResultsPlot->setInteractions(QCP::iSelectItems | QCP::iRangeDrag | QCP::iRangeZoom);
  XRFResultsPlotHLayout->addWidget(m_xrfResultsPlot);

  //Horizontal layout for residuals plot.
  QHBoxLayout* XRFResidualsPlotHLayout = new QHBoxLayout();
  m_xrfResidualsPlot = new QCustomPlot(this);
  m_xrfResidualsPlot->xAxis->setLabelFont(labelFontX);
  m_xrfResidualsPlot->yAxis->setLabelFont(labelFontY);
  m_xrfResidualsPlot->xAxis->setLabel("Energy (keV)");
  m_xrfResidualsPlot->yAxis->setLabel("Residual");
  m_xrfResidualsPlot->setInteractions(QCP::iSelectItems | QCP::iRangeDrag | QCP::iRangeZoom);
  XRFResidualsPlotHLayout->addWidget(m_xrfResidualsPlot);

  //Horizontal layout for Reset/lin/log plot button
  QHBoxLayout* xrfResultsRescaleHLayout = new QHBoxLayout();
  CancelButton* xrfResultsRescaleButton = new CancelButton();//using cancel button color scheme
  xrfResultsRescaleButton->setText("Reset Plot");
  xrfResultsRescaleButton->setFixedWidth(100);
  QButtonGroup* xrfResultsLinLogGroup = new QButtonGroup(this);
  m_xrfResultsLogButton = new QRadioButton("Log");
  m_xrfResultsLinButton = new QRadioButton("Lin");
  xrfResultsLinLogGroup->addButton(m_xrfResultsLogButton);
  xrfResultsLinLogGroup->addButton(m_xrfResultsLinButton);
  //add buttons to their H layout
  xrfResultsRescaleHLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
  xrfResultsRescaleHLayout->addWidget(xrfResultsRescaleButton);
  xrfResultsRescaleHLayout->addItem(new QSpacerItem(150, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  xrfResultsRescaleHLayout->addWidget(m_xrfResultsLogButton);
  xrfResultsRescaleHLayout->addItem(new QSpacerItem(50, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  xrfResultsRescaleHLayout->addWidget(m_xrfResultsLinButton);
  xrfResultsRescaleHLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
  m_xrfResultsLogButton->setChecked(false);
  m_xrfResultsLinButton->setChecked(true);
  //Connect xrf log/lin buttons to slots
  connect(xrfResultsRescaleButton, &QPushButton::clicked, this, &MainWindow::on_XRF_results_reset_button_clicked);
  connect(m_xrfResultsLogButton, &QRadioButton::toggled, this, &MainWindow::on_XRF_results_log_button_toggled);
  connect(m_xrfResultsLinButton, &QRadioButton::toggled, this, &MainWindow::on_XRF_results_lin_button_toggled);

  //Horizontal layout for check boxes.
  QHBoxLayout* xrfResultsFitHLayout = new QHBoxLayout();
  m_xrfResultsFitCheckBox = new QCheckBox("Total Fit");
  m_xrfResultsContCheckBox = new QCheckBox("Step Continuum");
  m_xrfResultsFitCheckBox->setChecked(true);
  m_xrfResultsContCheckBox->setChecked(true);
  connect(m_xrfResultsFitCheckBox, &QCheckBox::toggled, this, &MainWindow::on_XRF_results_fit_checkBox_toggled);
  connect(m_xrfResultsContCheckBox, &QCheckBox::toggled, this, &MainWindow::on_XRF_results_continuum_checkBox_toggled);
  m_xrfResultsLASCheckBox = new QCheckBox("Low Angle Scattering");
  m_xrfResultsBSCheckBox = new QCheckBox("Backscattering");
  m_xrfResultsLASCheckBox->setChecked(true);
  m_xrfResultsBSCheckBox->setChecked(true);
  //connect(m_xrfResultsLASCheckBox, &QCheckBox::toggled, this, &MainWindow::on_XRF_Pu_xray_checkBox_toggled);
  //connect(m_xrfResultsBSCheckBox, &QCheckBox::toggled, this, &MainWindow::on_XRF_U_xray_checkBox_toggled);
  m_xrfResultsTailCheckBox = new QCheckBox("Tailing");
  m_xrfResultsCoincCheckBox = new QCheckBox("Coincidence Summing");
  m_xrfResultsTailCheckBox->setChecked(true);
  m_xrfResultsCoincCheckBox->setChecked(true);
  //connect(m_xrfResultsTailCheckBox, &QCheckBox::toggled, this, &MainWindow::on_XRF_Pu_xray_checkBox_toggled);
  //connect(m_xrfResultsCoincCheckBox, &QCheckBox::toggled, this, &MainWindow::on_XRF_U_xray_checkBox_toggled);
  //add check boxes to their H layout
  xrfResultsFitHLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
  xrfResultsFitHLayout->addWidget(m_xrfResultsFitCheckBox);
  xrfResultsFitHLayout->addItem(new QSpacerItem(30, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  xrfResultsFitHLayout->addWidget(m_xrfResultsContCheckBox);
  xrfResultsFitHLayout->addItem(new QSpacerItem(30, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  xrfResultsFitHLayout->addWidget(m_xrfResultsLASCheckBox);
  xrfResultsFitHLayout->addItem(new QSpacerItem(30, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  xrfResultsFitHLayout->addWidget(m_xrfResultsBSCheckBox);
  xrfResultsFitHLayout->addItem(new QSpacerItem(30, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  xrfResultsFitHLayout->addWidget(m_xrfResultsTailCheckBox);
  xrfResultsFitHLayout->addItem(new QSpacerItem(30, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  xrfResultsFitHLayout->addWidget(m_xrfResultsCoincCheckBox);
  xrfResultsFitHLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

  // Make plot areas align
  auto* marginGroup = new QCPMarginGroup(m_xrfResultsPlot);
  m_xrfResultsPlot->axisRect()->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);
  m_xrfResidualsPlot->axisRect()->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);

  // Sync top/bottom secondary axes too
  connect(m_xrfResultsPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),
          m_xrfResultsPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(m_xrfResidualsPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),
          m_xrfResidualsPlot->xAxis2, SLOT(setRange(QCPRange)));

  // Sync main and residual x ranges
  connect(m_xrfResultsPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),
          m_xrfResidualsPlot->xAxis, SLOT(setRange(QCPRange)));
  connect(m_xrfResidualsPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),
          m_xrfResultsPlot->xAxis, SLOT(setRange(QCPRange)));

  // Replot the other widget when one range changes
  connect(m_xrfResultsPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),
          m_xrfResidualsPlot, SLOT(replot()));

  connect(m_xrfResidualsPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),
          m_xrfResultsPlot, SLOT(replot()));

  m_xrfResultsPlot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  m_xrfResidualsPlot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  //Add the plot layouts inside the bordered plot-area widget
  XRFResultsPlotsVLayout->addLayout(XRFResultsPlotHLayout, 3);
  XRFResultsPlotsVLayout->addLayout(XRFResidualsPlotHLayout, 1);

  //Add the button widgets layouts to button V layout to layout
  XRFResultsButtonsVLayout->addLayout(xrfResultsRescaleHLayout);
  XRFResultsButtonsVLayout->addLayout(xrfResultsFitHLayout);

  //Add the bordered widgets to the outer horizontal layout
  XRFResultsWidgetsVLayout->addWidget(xrfResultsWidgetArea, 1);
  XRFResultsWidgetsVLayout->addWidget(xrfResultsPlotArea, 5);
  
  //Add the titles and widget area layouts to the main V layout
  XRFResultsVLayout->addItem(new QSpacerItem(1, 10, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFResultsVLayout->addLayout(XRFResultsTitleHLayout);
  XRFResultsVLayout->addLayout(XRFResultsSubtitleHLayout);
  XRFResultsVLayout->addLayout(XRFResultsSubsubtitleHLayout);
  XRFResultsVLayout->addItem(new QSpacerItem(1, 30, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFResultsVLayout->addLayout(XRFResultsWidgetsVLayout);
  XRFResultsVLayout->addItem(new QSpacerItem(1, 10, QSizePolicy::Fixed, QSizePolicy::Minimum));

  return XRFResultsPlotPage;
}

QWidget* MainWindow::layout_XRF_results_ceres_tab()
{
  QWidget* XRFResultsCeresPage = new QWidget(m_mainTabWidget);

  //Vertical layout will hold all the horizontal layouts
  QVBoxLayout* XRFFitDetailsVLayout = new QVBoxLayout(XRFResultsCeresPage);

  //Horizontal layout for the title.
  QHBoxLayout* XRFFitDetailsTitleHLayout = new QHBoxLayout();
  QLabel* XRFFitDetailsTitleLabel = new QLabel;
  XRFFitDetailsTitleLabel->setText("UMAREKAWARI (Mari) v1.0");
  XRFFitDetailsTitleLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 15px;");
  XRFFitDetailsTitleHLayout->addWidget(XRFFitDetailsTitleLabel);
  XRFFitDetailsTitleHLayout->setAlignment(XRFFitDetailsTitleLabel, Qt::AlignCenter);

  //Horizontal layout for the subtitle.
  QHBoxLayout* XRFFitDetailsSubtitleHLayout = new QHBoxLayout();
  QLabel* XRFFitDetailsSubtitleLabel = new QLabel;
  XRFFitDetailsSubtitleLabel->setText("Hybrid K-Edge Densitometry Software");
  XRFFitDetailsSubtitleLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  XRFFitDetailsSubtitleHLayout->addWidget(XRFFitDetailsSubtitleLabel);
  XRFFitDetailsSubtitleHLayout->setAlignment(XRFFitDetailsSubtitleLabel, Qt::AlignCenter);

  //Horizontal layout for the subsubtitle.
  QHBoxLayout* XRFFitDetailsSubsubtitleHLayout = new QHBoxLayout();
  QLabel* XRFFitDetailsSubsubtitleLabel = new QLabel;
  XRFFitDetailsSubsubtitleLabel->setText("Linear Solver Performance and Fit Details");
  XRFFitDetailsSubsubtitleLabel->setStyleSheet("font-weight: bold; font-size: 13px;");
  XRFFitDetailsSubsubtitleHLayout->addWidget(XRFFitDetailsSubsubtitleLabel);
  XRFFitDetailsSubsubtitleHLayout->setAlignment(XRFFitDetailsSubsubtitleLabel, Qt::AlignCenter);

  //Horizontal layout for Algorithm & Convergence title.
  QHBoxLayout* XRFFitDetailsConvTitleHLayout = new QHBoxLayout();
  QLabel* XRFFitDetailsConvTitleLabel = new QLabel;
  XRFFitDetailsConvTitleLabel->setText("Algorithm & Convergence");
  XRFFitDetailsConvTitleLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  XRFFitDetailsConvTitleHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsConvTitleHLayout->addWidget(XRFFitDetailsConvTitleLabel);
  XRFFitDetailsConvTitleHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for termination line.
  QHBoxLayout* XRFFitDetailsTermHLayout = new QHBoxLayout();
  QLabel* XRFFitDetailsAlgoLabel = new QLabel;
  XRFFitDetailsAlgoLabel->setText("Algorithm Type:");
  m_XRFFitDetailsAlgoField = new NonUpdatableLabel;
  QLabel* XRFFitDetailsTermLabel = new QLabel;
  XRFFitDetailsTermLabel->setText("Termination Type:");
  m_XRFFitDetailsTermField = new NonUpdatableLabel;
  QLabel* XRFFitDetailsMsgLabel = new QLabel;
  XRFFitDetailsMsgLabel->setText("Message:");
  m_XRFFitDetailsMsgField = new NonUpdatableLabel;
  XRFFitDetailsTermHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsTermHLayout->addWidget(XRFFitDetailsAlgoLabel);
  XRFFitDetailsTermHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsTermHLayout->addWidget(m_XRFFitDetailsAlgoField);
  XRFFitDetailsTermHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsTermHLayout->addWidget(XRFFitDetailsTermLabel);
  XRFFitDetailsTermHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsTermHLayout->addWidget(m_XRFFitDetailsTermField);
  XRFFitDetailsTermHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsTermHLayout->addWidget(XRFFitDetailsMsgLabel);
  XRFFitDetailsTermHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsTermHLayout->addWidget(m_XRFFitDetailsMsgField);
  XRFFitDetailsTermHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for Cost and Residuals title.
  QHBoxLayout* XRFFitDetailsCostTitleHLayout = new QHBoxLayout();
  QLabel* XRFFitDetailsCostTitleLabel = new QLabel;
  XRFFitDetailsCostTitleLabel->setText("Cost and Residuals");
  XRFFitDetailsCostTitleLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  XRFFitDetailsCostTitleHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsCostTitleHLayout->addWidget(XRFFitDetailsCostTitleLabel);
  XRFFitDetailsCostTitleHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for cost line.
  QHBoxLayout* XRFFitDetailsCostHLayout = new QHBoxLayout();
  QLabel* XRFFitDetailsInitCostLabel = new QLabel;
  XRFFitDetailsInitCostLabel->setText("Initial Cost:");
  m_XRFFitDetailsInitCostField = new NonUpdatableLabel;
  QLabel* XRFFitDetailsFinalCostLabel = new QLabel;
  XRFFitDetailsFinalCostLabel->setText("Final Cost:");
  m_XRFFitDetailsFinalCostField = new NonUpdatableLabel;
  QLabel* XRFFitDetailsCostDropLabel = new QLabel;
  XRFFitDetailsCostDropLabel->setText("Cost Drop:");
  m_XRFFitDetailsCostDropField = new NonUpdatableLabel;
  XRFFitDetailsCostHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsCostHLayout->addWidget(XRFFitDetailsInitCostLabel);
  XRFFitDetailsCostHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsCostHLayout->addWidget(m_XRFFitDetailsInitCostField);
  XRFFitDetailsCostHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsCostHLayout->addWidget(XRFFitDetailsFinalCostLabel);
  XRFFitDetailsCostHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsCostHLayout->addWidget(m_XRFFitDetailsFinalCostField);
  XRFFitDetailsCostHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsCostHLayout->addWidget(XRFFitDetailsCostDropLabel);
  XRFFitDetailsCostHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsCostHLayout->addWidget(m_XRFFitDetailsCostDropField);
  XRFFitDetailsCostHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for residuals line.
  QHBoxLayout* XRFFitDetailsResidualHLayout = new QHBoxLayout();
  QLabel* XRFFitDetailsResidualLabel = new QLabel;
  XRFFitDetailsResidualLabel->setText("Number of Residuals Evals:");
  m_XRFFitDetailsResidualField = new NonUpdatableLabel;
  QLabel* XRFFitDetailsJacobianLabel = new QLabel;
  XRFFitDetailsJacobianLabel->setText("Number of Jacobian Evals:");
  m_XRFFitDetailsJacobianField = new NonUpdatableLabel;
  XRFFitDetailsResidualHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsResidualHLayout->addWidget(XRFFitDetailsResidualLabel);
  XRFFitDetailsResidualHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsResidualHLayout->addWidget(m_XRFFitDetailsResidualField);
  XRFFitDetailsResidualHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsResidualHLayout->addWidget(XRFFitDetailsJacobianLabel);
  XRFFitDetailsResidualHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsResidualHLayout->addWidget(m_XRFFitDetailsJacobianField);
  XRFFitDetailsResidualHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for Execution Performance & Iterations title.
  QHBoxLayout* XRFFitDetailsPerformTitleHLayout = new QHBoxLayout();
  QLabel* XRFFitDetailsPerformTitleLabel = new QLabel;
  XRFFitDetailsPerformTitleLabel->setText("Execution Performance & Iterations");
  XRFFitDetailsPerformTitleLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  XRFFitDetailsPerformTitleHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsPerformTitleHLayout->addWidget(XRFFitDetailsPerformTitleLabel);
  XRFFitDetailsPerformTitleHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for performance line.
  QHBoxLayout* XRFFitDetailsPerformHLayout = new QHBoxLayout();
  QLabel* XRFFitDetailsTotalItLabel = new QLabel;
  XRFFitDetailsTotalItLabel->setText("Total Iterations:");
  m_XRFFitDetailsTotalItField = new NonUpdatableLabel;
  QLabel* XRFFitDetailsSuccItLabel = new QLabel;
  XRFFitDetailsSuccItLabel->setText("Successful Iterations:");
  m_XRFFitDetailsSuccItField = new NonUpdatableLabel;
  QLabel* XRFFitDetailsUnsuccItLabel = new QLabel;
  XRFFitDetailsUnsuccItLabel->setText("Unsuccessful Iterations:");
  m_XRFFitDetailsUnsuccItField = new NonUpdatableLabel;
  XRFFitDetailsPerformHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsPerformHLayout->addWidget(XRFFitDetailsTotalItLabel);
  XRFFitDetailsPerformHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsPerformHLayout->addWidget(m_XRFFitDetailsTotalItField);
  XRFFitDetailsPerformHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsPerformHLayout->addWidget(XRFFitDetailsSuccItLabel);
  XRFFitDetailsPerformHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsPerformHLayout->addWidget(m_XRFFitDetailsSuccItField);
  XRFFitDetailsPerformHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsPerformHLayout->addWidget(XRFFitDetailsUnsuccItLabel);
  XRFFitDetailsPerformHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsPerformHLayout->addWidget(m_XRFFitDetailsUnsuccItField);
  XRFFitDetailsPerformHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for time line.
  QHBoxLayout* XRFFitDetailsTimeHLayout = new QHBoxLayout();
  QLabel* XRFFitDetailsTotalTimeLabel = new QLabel;
  XRFFitDetailsTotalTimeLabel->setText("Total Time (s):");
  m_XRFFitDetailsTotalTimeField = new NonUpdatableLabel;
  QLabel* XRFFitDetailsJacobTimeLabel = new QLabel;
  XRFFitDetailsJacobTimeLabel->setText("Jacobian Eval. Time (s):");
  m_XRFFitDetailsJacobTimeField = new NonUpdatableLabel;
  QLabel* XRFFitDetailsLinSolveTimeLabel = new QLabel;
  XRFFitDetailsLinSolveTimeLabel->setText("Linear Solver Time (s):");
  m_XRFFitDetailsLinSolveTimeField = new NonUpdatableLabel;
  XRFFitDetailsTimeHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsTimeHLayout->addWidget(XRFFitDetailsTotalTimeLabel);
  XRFFitDetailsTimeHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsTimeHLayout->addWidget(m_XRFFitDetailsTotalTimeField);
  XRFFitDetailsTimeHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsTimeHLayout->addWidget(XRFFitDetailsJacobTimeLabel);
  XRFFitDetailsTimeHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsTimeHLayout->addWidget(m_XRFFitDetailsJacobTimeField);
  XRFFitDetailsTimeHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsTimeHLayout->addWidget(XRFFitDetailsLinSolveTimeLabel);
  XRFFitDetailsTimeHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsTimeHLayout->addWidget(m_XRFFitDetailsLinSolveTimeField);
  XRFFitDetailsTimeHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  ////Horizontal layout for free parameter table.
  //QHBoxLayout* XRFFitDetailsFreeTableHLayout = new QHBoxLayout();
  //// Create a table with 4 rows and 4 columns
  //QTableWidget *tableWidget = new QTableWidget(4, 4, this);
  ////Hide the default built-in horizontal header
  //tableWidget->horizontalHeader()->setVisible(false);
  ////Merge the top row (Row 0) into two separate dual-column headers
  ////Arguments: setSpan(targetRow, targetColumn, rowSpan, columnSpan)
  //tableWidget->setSpan(0, 0, 1, 2); // Merges Col 0 and Col 1
  //tableWidget->setSpan(0, 2, 1, 2); // Merges Col 2 and Col 3
  ////Create and style Header 1 (Personal Info)
  //QTableWidgetItem *header1 = new QTableWidgetItem("Bremsstrahlung Shape");
  //header1->setTextAlignment(Qt::AlignCenter);
  //header1->setFlags(header1->flags() & ~Qt::ItemIsEditable); // Read-only
  //header1->setBackground(Qt::lightGray); 
  //tableWidget->setItem(0, 0, header1); // Must place in top-left cell of the span
  ////Create and style Header 2 (Contact Info)
  //QTableWidgetItem *header2 = new QTableWidgetItem("Sample Densities");
  //header2->setTextAlignment(Qt::AlignCenter);
  //header2->setFlags(header2->flags() & ~Qt::ItemIsEditable); // Read-only
  //header2->setBackground(Qt::lightGray); 
  //tableWidget->setItem(0, 2, header2); // Must place in top-left cell of the span
  ////Populate Row 1 (Sub-headers for the 4 individual columns)
  //tableWidget->setItem(1, 0, new QTableWidgetItem("Scaling Constant (C)"));
  //tableWidget->setItem(1, 2, new QTableWidgetItem("Rho U"));
  ////Populate Row 2 (Example Data)
  //tableWidget->setItem(2, 0, new QTableWidgetItem("Shape Constatnt (X)"));
  //tableWidget->setItem(2, 2, new QTableWidgetItem("Rho Pu"));
  //tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
  //tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
  //tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
  //tableWidget->resizeRowsToContents();
  //int h = tableWidget->horizontalHeader()->height() +
  //        tableWidget->verticalHeader()->length() +
  //        2 * tableWidget->frameWidth();
  //tableWidget->setFixedHeight(h);
  //tableWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
  //
  //XRFFitDetailsFreeTableHLayout->addWidget(tableWidget);

  //Add the H layouts to the V layout
  XRFFitDetailsVLayout->addLayout(XRFFitDetailsTitleHLayout);
  XRFFitDetailsVLayout->addLayout(XRFFitDetailsSubtitleHLayout);
  XRFFitDetailsVLayout->addLayout(XRFFitDetailsSubsubtitleHLayout);
  XRFFitDetailsVLayout->addItem(new QSpacerItem(1, 30, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFFitDetailsVLayout->addLayout(XRFFitDetailsConvTitleHLayout);
  XRFFitDetailsVLayout->addLayout(XRFFitDetailsTermHLayout);
  XRFFitDetailsVLayout->addLayout(XRFFitDetailsCostTitleHLayout);
  XRFFitDetailsVLayout->addLayout(XRFFitDetailsCostHLayout);
  XRFFitDetailsVLayout->addLayout(XRFFitDetailsResidualHLayout);
  XRFFitDetailsVLayout->addLayout(XRFFitDetailsPerformTitleHLayout);
  XRFFitDetailsVLayout->addLayout(XRFFitDetailsPerformHLayout);
  XRFFitDetailsVLayout->addLayout(XRFFitDetailsTimeHLayout);
  //XRFFitDetailsVLayout->addLayout(XRFFitDetailsFreeTableHLayout);
  XRFFitDetailsVLayout->addStretch(1);

  return XRFResultsCeresPage;
}

QWidget* MainWindow::layout_compare_tab()
{
  QWidget* comparePage = new QWidget();
  QVBoxLayout* layout2 = new QVBoxLayout(comparePage);
  QLabel* label2 = new QLabel("Content for comparison tab", comparePage);
  layout2->addWidget(label2);
  return comparePage;
}

void MainWindow::create_actions() 
{
  openAct = new QAction(tr("&Open Spectrum"), this);
  openAct->setShortcut(QKeySequence::Open);
  openAct->setStatusTip(tr("Open an existing file"));
  connect(openAct, &QAction::triggered, this, &MainWindow::open_file);

  analyzeAct = new QAction(tr("&Analyze"), this);
  analyzeAct->setShortcut(QKeySequence::New);
  analyzeAct->setStatusTip(tr("Analyze a spectrum"));
  connect(analyzeAct, &QAction::triggered, this, &MainWindow::analyze);

  saveAct = new QAction(tr("&Save"), this);
  saveAct->setShortcut(QKeySequence::Save);
  saveAct->setStatusTip(tr("Save the document to disk"));
  connect(saveAct, &QAction::triggered, this, &MainWindow::save_file);

  quitAct = new QAction(tr("&Quit"), this);
  quitAct->setShortcut(tr("Ctrl+Q"));
  quitAct->setStatusTip(tr("quit the app"));
  connect(quitAct, &QAction::triggered, this, &MainWindow::quit_app);

  hideKEDAct = new QAction(tr("&Hide/Show KED Window"), this);
  hideKEDAct->setShortcut(tr("Ctrl+K"));
  hideKEDAct->setStatusTip(tr("Hide KED Window"));
  connect(hideKEDAct, &QAction::triggered, this, &MainWindow::hide_KED_window);

  hideXRFAct = new QAction(tr("&Hide/Show XRF Window"), this);
  hideXRFAct->setShortcut(tr("Ctrl+F"));
  hideXRFAct->setStatusTip(tr("Hide XRF Window"));
  connect(hideXRFAct, &QAction::triggered, this, &MainWindow::hide_XRF_window);

  newParamAct = new QAction(tr("&New"), this);
  newParamAct->setStatusTip(tr("New Parameter Set"));
  connect(newParamAct, &QAction::triggered, this, &MainWindow::new_parameter_set);

  editParamAct = new QAction(tr("&Edit"), this);
  editParamAct->setStatusTip(tr("New Parameter Set"));
  connect(editParamAct, &QAction::triggered, this, &MainWindow::edit_parameter_set);

  deleteParamAct = new QAction(tr("&Delete"), this);
  deleteParamAct->setStatusTip(tr("New Parameter Set"));
  connect(deleteParamAct, &QAction::triggered, this, &MainWindow::delete_parameter_set);

  calSpectrumAct = new QAction(tr("&Calibrate Spectrum"), this);
  calSpectrumAct->setStatusTip(tr("Calibrate a spectrum"));
  connect(calSpectrumAct, &QAction::triggered, this, &MainWindow::calibrate_spectrum);

  calKEDAct = new QAction(tr("&Calibrate KED instrument"), this);
  calKEDAct->setStatusTip(tr("Calibrate the KED instrument"));
  connect(calKEDAct, &QAction::triggered, this, &MainWindow::calibrate_KED);

  calXRFAct = new QAction(tr("&Calibrate XRF instrument"), this);
  calXRFAct->setStatusTip(tr("Calibrate the XRF instrument"));
  connect(calXRFAct, &QAction::triggered, this, &MainWindow::calibrate_XRF);
}

void MainWindow::create_menus() 
{
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(openAct);
  fileMenu->addAction(analyzeAct);
  fileMenu->addAction(saveAct);
  fileMenu->addAction(quitAct);

  viewMenu = menuBar()->addMenu(tr("&View"));
  viewMenu->addAction(hideKEDAct);
  viewMenu->addAction(hideXRFAct);

  paramMenu = menuBar()->addMenu(tr("&Parameter"));
  paramMenu->addAction(newParamAct);
  paramMenu->addAction(editParamAct);
  paramMenu->addAction(deleteParamAct);

  calibrateMenu = menuBar()->addMenu(tr("&Calibrate"));
  calibrateMenu->addAction(calSpectrumAct);
  calibrateMenu->addAction(calKEDAct);
  calibrateMenu->addAction(calXRFAct);
}


void MainWindow::open_file() 
{
  KDevLogger::getInstance().logEvent("User selected File | Open Spectrum");
  LoadSpectrumDialog* dialog = new LoadSpectrumDialog(this);
  connect(dialog, &LoadSpectrumDialog::handle_graphing, this, &MainWindow::handle_graphing);
  connect(dialog, &LoadSpectrumDialog::update_state_fields, this, &MainWindow::handle_state_fields_update);
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->exec(); // Show modally
}

void MainWindow::analyze() 
{
  KDevLogger::getInstance().logEvent("User selected File | Analyze");
  AnalyzeDialog* dialog = new AnalyzeDialog(this);
  connect(dialog, &AnalyzeDialog::load_new_spectra, this, &MainWindow::handle_graphing);
  connect(dialog, &AnalyzeDialog::load_new_spectra, this, &MainWindow::handle_state_fields_update);
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->exec(); // Show modally
}

void MainWindow::calibrate_spectrum() 
{
  KDevLogger::getInstance().logEvent("User selected File | Calibrate Spectrum");
  CalibrateXRFResultsDialog* dialog = new CalibrateXRFResultsDialog(this);
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->exec(); // Show modally
}

void MainWindow::calibrate_KED() 
{
  KDevLogger::getInstance().logEvent("User selected File | Calibrate KED");
  CalibrateKEDDialog* dialog = new CalibrateKEDDialog(this);
  connect(dialog, &CalibrateKEDDialog::load_new_spectra, this, &MainWindow::handle_graphing);
  connect(dialog, &CalibrateKEDDialog::load_new_spectra, this, &MainWindow::handle_state_fields_update);
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->exec(); // Show modally
}

void MainWindow::calibrate_XRF() 
{
  KDevLogger::getInstance().logEvent("User selected File | Calibrate XRF");
  CalibrateXRFDialog* dialog = new CalibrateXRFDialog(this);
  connect(dialog, &CalibrateXRFDialog::load_new_spectra, this, &MainWindow::handle_graphing);
  connect(dialog, &CalibrateXRFDialog::load_new_spectra, this, &MainWindow::handle_state_fields_update);
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->exec(); // Show modally
}

void MainWindow::save_file() 
{
  //KDevLogger::getInstance().logEvent("User selected File | Save");
  //QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"), QString(), tr("Text Files (*.txt);;All Files (*)"));
  //if (!fileName.isEmpty()) 
  //{
  //  QMessageBox::information(this, tr("Save File"), tr("Saving file: %1").arg(fileName));
  //}

    QPrinter printer(QPrinter::HighResolution);
    QPrintDialog dialog(&printer, m_summaryTextEdit);

    if (dialog.exec() != QDialog::Accepted)
        return;

    m_summaryTextEdit->print(&printer);
}

void MainWindow::quit_app() 
{
  KDevLogger::getInstance().logEvent("User selected File | Quit");
  QApplication::quit();
}

void MainWindow::hide_KED_window()
{
  if(kedIsVisible) kedIsVisible = false;
  else kedIsVisible = true;
  if (m_specPgUpperWidget) 
  {
    m_specPgUpperWidget->setVisible(kedIsVisible);
  }
  else
  {
    QMessageBox::information(this,  tr("Information"), tr("OOPS! Something went wrong.")); 
  }
}

void MainWindow::hide_XRF_window()
{
  if(xrfIsVisible) xrfIsVisible = false;
  else xrfIsVisible = true;
  if (m_specPgLowerWidget) 
  {
    m_specPgLowerWidget->setVisible(xrfIsVisible);
  }
  else
  {
    QMessageBox::information(this,  tr("Information"), tr("OOPS! Something went wrong.")); 
  }
}

void MainWindow::new_parameter_set() 
{
  KDevLogger::getInstance().logEvent("User selected Parameter | New");
  NewParameterDialog* newDialog = new NewParameterDialog(0, this);
  newDialog->setAttribute(Qt::WA_DeleteOnClose);
  newDialog->exec(); // Show modally
}

void MainWindow::edit_parameter_set() 
{
  KDevLogger::getInstance().logEvent("User selected Parameter | Edit");
  EditParameterDialog* editDialog = new EditParameterDialog(this);
  connect(editDialog, &EditParameterDialog::open_parameter_set_for_editing,
          this, [editDialog, this]()
  {
    editDialog->hide(); // disappear immediately
    NewParameterDialog* newDialog = new NewParameterDialog(1, this);
    newDialog->setAttribute(Qt::WA_DeleteOnClose);
    newDialog->exec();
    editDialog->close();
  });
  editDialog->exec();
}

void MainWindow::delete_parameter_set() 
{
  KDevLogger::getInstance().logEvent("User selected Parameter | Delete");
  DeleteParameterDialog* deleteDialog = new DeleteParameterDialog(this);
  deleteDialog->setAttribute(Qt::WA_DeleteOnClose);
  deleteDialog->exec();
}

void MainWindow::handle_graphing()
{
  int kedStateNumber = KFileManager::getInstance().calc_KED_state_number();
  emit KED_widget_status(m_kedSampComboBox->currentIndex(),
                         m_kedBkgdComboBox->currentIndex(),
                         m_kedRefComboBox->currentIndex(),
                         m_kedSampCheckBox->isChecked(),
                         m_kedBkgdCheckBox->isChecked(),
                         m_kedRefCheckBox->isChecked());
  KKEDPlotManager::getInstance().setState(kedStateNumber);
  KKEDPlotManager::getInstance().applyState(m_kedPlot, 
                                            m_kedFitCheckBox->isChecked());

  int xrfStateNumber = KFileManager::getInstance().calc_XRF_state_number();
  emit XRF_widget_status(m_xrfSampComboBox->currentIndex(),
                         m_xrfBkgdComboBox->currentIndex(),
                         m_xrfRefComboBox->currentIndex(),
                         m_xrfSampCheckBox->isChecked(),
                         m_xrfBkgdCheckBox->isChecked(),
                         m_xrfRefCheckBox->isChecked());
  KXRFPlotManager::getInstance().setState(xrfStateNumber);
  KXRFPlotManager::getInstance().applyState(m_xrfPlot);
}

void MainWindow::handle_KED_graphing()
{
  int kedStateNumber = KFileManager::getInstance().calc_KED_state_number();
  emit KED_widget_status(m_kedSampComboBox->currentIndex(),
                         m_kedBkgdComboBox->currentIndex(),
                         m_kedRefComboBox->currentIndex(),
                         m_kedSampCheckBox->isChecked(),
                         m_kedBkgdCheckBox->isChecked(),
                         m_kedRefCheckBox->isChecked());
  KKEDPlotManager::getInstance().setState(kedStateNumber);
  KKEDPlotManager::getInstance().applyState(m_kedPlot, 
                                            m_kedFitCheckBox->isChecked());
}

void MainWindow::handle_XRF_graphing()
{
  int xrfStateNumber = KFileManager::getInstance().calc_XRF_state_number();
  emit XRF_widget_status(m_xrfSampComboBox->currentIndex(),
                         m_xrfBkgdComboBox->currentIndex(),
                         m_xrfRefComboBox->currentIndex(),
                         m_xrfSampCheckBox->isChecked(),
                         m_xrfBkgdCheckBox->isChecked(),
                         m_xrfRefCheckBox->isChecked());
  KXRFPlotManager::getInstance().setState(xrfStateNumber);
  KXRFPlotManager::getInstance().applyState(m_xrfPlot);
}

void MainWindow::handle_state_fields_update()
{
  update_state_fields(KFileManager::getInstance().m_kedSampData,
                      m_kedSampFilenameField, m_kedSampChannelField,
                      m_kedSampDateField, m_kedSampLTField, m_kedSampRTField);
  update_state_fields(KFileManager::getInstance().m_kedBkgdData,
                      m_kedBkgdFilenameField, m_kedBkgdChannelField,
                      m_kedBkgdDateField, m_kedBkgdLTField, m_kedBkgdRTField);
  update_state_fields(KFileManager::getInstance().m_kedRefData,
                      m_kedRefFilenameField, m_kedRefChannelField,
                      m_kedRefDateField, m_kedRefLTField, m_kedRefRTField);
  update_state_fields(KFileManager::getInstance().m_xrfSampData,
                      m_xrfSampFilenameField, m_xrfSampChannelField,
                      m_xrfSampDateField, m_xrfSampLTField, m_xrfSampRTField);
  update_state_fields(KFileManager::getInstance().m_xrfBkgdData,
                      m_xrfBkgdFilenameField, m_xrfBkgdChannelField,
                      m_xrfBkgdDateField, m_xrfBkgdLTField, m_xrfBkgdRTField);
  update_state_fields(KFileManager::getInstance().m_xrfRefData,
                      m_xrfRefFilenameField, m_xrfRefChannelField,
                      m_xrfRefDateField, m_xrfRefLTField, m_xrfRefRTField);
  update_widget_disabling();
}

void MainWindow::update_state_fields(const KSpectralData&     a_dataSet,
                                           NonUpdatableLabel* a_filenameField, 
                                           NonUpdatableLabel* a_channelField,
                                           NonUpdatableLabel* a_dateField, 
                                           NonUpdatableLabel* a_LTField, 
                                           NonUpdatableLabel* a_RTField)
{
  //Fill out filename field
  if (a_filenameField) 
  {
    if(a_dataSet.m_isLoaded==true)
    {
      a_filenameField->setText(QString::fromStdString(a_dataSet.m_filename));
    }
    else
    {
      a_filenameField->setText("");
    }
  }
  //Fill out channel number field
  if (a_channelField) 
  {
    if(a_dataSet.m_isLoaded==true)
    {
      a_channelField->setText(QString::number(a_dataSet.m_measurement->num_gamma_channels()));
    }
    else
    {
      a_channelField->setText("");
    }
  }
  //Fill out acqusition date time field
  if (a_dateField) 
  {
    if(a_dataSet.m_isLoaded==true)
    {
      std::chrono::time_point<std::chrono::system_clock,std::chrono::microseconds> chronoTime = 
      a_dataSet.m_measurement->start_time();
      std::string dateTime = std::format("{:%Y-%m-%d %H:%M}", chronoTime, chronoTime.time_since_epoch());
      a_dateField->setText(QString::fromStdString(dateTime));
    }
    else
    {
      a_dateField->setText("");
    }
  }
  //Fill out live time field
  if (a_LTField) 
  {
    if(a_dataSet.m_isLoaded==true)
    {
      a_LTField->setText(QString::number(a_dataSet.m_measurement->live_time()));
    }
    else
    {
      a_LTField->setText("");
    }
  }
  //Fill out real time field
  if (a_RTField) 
  {
    if(a_dataSet.m_isLoaded==true)
    {
      a_RTField->setText(QString::number(a_dataSet.m_measurement->real_time()));
    }
    else
    {
      a_RTField->setText("");
    }
  }
}

void MainWindow::update_widget_disabling()
{
  m_kedSampComboBox->setDisabled(!KFileManager::getInstance().m_kedSampData.m_isLoaded);
  m_kedSampCheckBox->setDisabled(!KFileManager::getInstance().m_kedSampData.m_isLoaded);
  m_kedSampEcalButton->setDisabled(!KFileManager::getInstance().m_kedSampData.m_isLoaded);
  m_kedSampEcalButton->setChecked(KFileManager::getInstance().m_kedSampData.m_isLoaded);

  m_kedBkgdComboBox->setDisabled(!KFileManager::getInstance().m_kedBkgdData.m_isLoaded);
  m_kedBkgdCheckBox->setDisabled(!KFileManager::getInstance().m_kedBkgdData.m_isLoaded);
  m_kedBkgdEcalButton->setDisabled(!KFileManager::getInstance().m_kedBkgdData.m_isLoaded);
  m_kedBkgdEcalButton->setChecked(KFileManager::getInstance().m_kedBkgdData.m_isLoaded);
  
  m_kedRefComboBox->setDisabled(!KFileManager::getInstance().m_kedRefData.m_isLoaded);
  m_kedRefCheckBox->setDisabled(!KFileManager::getInstance().m_kedRefData.m_isLoaded);
  m_kedRefEcalButton->setDisabled(!KFileManager::getInstance().m_kedRefData.m_isLoaded);
  m_kedRefEcalButton->setChecked(KFileManager::getInstance().m_kedRefData.m_isLoaded);
  
  m_xrfSampComboBox->setDisabled(!KFileManager::getInstance().m_xrfSampData.m_isLoaded);
  m_xrfSampCheckBox->setDisabled(!KFileManager::getInstance().m_xrfSampData.m_isLoaded);
  m_xrfSampEcalButton->setDisabled(!KFileManager::getInstance().m_xrfSampData.m_isLoaded);
  m_xrfSampEcalButton->setChecked(KFileManager::getInstance().m_xrfSampData.m_isLoaded);
  
  m_xrfBkgdComboBox->setDisabled(!KFileManager::getInstance().m_xrfBkgdData.m_isLoaded);
  m_xrfBkgdCheckBox->setDisabled(!KFileManager::getInstance().m_xrfBkgdData.m_isLoaded);
  m_xrfBkgdEcalButton->setDisabled(!KFileManager::getInstance().m_xrfBkgdData.m_isLoaded);
  m_xrfBkgdEcalButton->setChecked(KFileManager::getInstance().m_xrfBkgdData.m_isLoaded);
  
  m_xrfRefComboBox->setDisabled(!KFileManager::getInstance().m_xrfRefData.m_isLoaded);
  m_xrfRefCheckBox->setDisabled(!KFileManager::getInstance().m_xrfRefData.m_isLoaded);
  m_xrfRefEcalButton->setDisabled(!KFileManager::getInstance().m_xrfRefData.m_isLoaded);
  m_xrfRefEcalButton->setChecked(KFileManager::getInstance().m_xrfRefData.m_isLoaded);

  if(KFileManager::getInstance().m_kedSampData.m_isLoaded || 
     KFileManager::getInstance().m_kedBkgdData.m_isLoaded || 
     KFileManager::getInstance().m_kedRefData.m_isLoaded)
  {
    m_kedSlider->setDisabled(false);
    m_kedSlider->setStyleSheet(
                                "QSlider:vertical \
                                {\
                                  border: none;\
                                }\
                                QSlider::handle:vertical \
                                {\
                                  background: #006b75;\
                                  border: 1px solid #006b75;\
                                  width: 18px;\
                                  height: 9px;\
                                  margin: -5px 0;\
                                  border-radius: 9px;\
                                }\
                                QSlider::handle:vertical:hover \
                                {\
                                  background: #006b75;\
                                  border: 2px solid #b84e41;\
                                }");
  }
  else
  {
    reset_KED_slider();
  }

  if(!KFileManager::getInstance().m_kedSampData.m_isLoaded)
  {
    m_kedSampSliderPosition = 500;
  }

  if(!KFileManager::getInstance().m_kedBkgdData.m_isLoaded)
  {
    m_kedBkgdSliderPosition = 500;
  }

  if(!KFileManager::getInstance().m_kedRefData.m_isLoaded)
  {
    m_kedRefSliderPosition = 500;
  }


  if(KFileManager::getInstance().m_xrfSampData.m_isLoaded || 
     KFileManager::getInstance().m_xrfBkgdData.m_isLoaded || 
     KFileManager::getInstance().m_xrfRefData.m_isLoaded)
  {
    m_xrfSlider->setDisabled(false);
    m_xrfSlider->setStyleSheet(
                                "QSlider:vertical \
                                {\
                                  border: none;\
                                }\
                                QSlider::handle:vertical \
                                {\
                                  background: #006b75;\
                                  border: 1px solid #006b75;\
                                  width: 18px;\
                                  height: 9px;\
                                  margin: -5px 0;\
                                  border-radius: 9px;\
                                }\
                                QSlider::handle:vertical:hover \
                                {\
                                  background: #006b75;\
                                  border: 2px solid #b84e41;\
                                }");
  }
  else
  {
    reset_XRF_slider();
  }

  if(!KFileManager::getInstance().m_xrfSampData.m_isLoaded)
  {
    m_xrfSampSliderPosition = 500;
  }

  if(!KFileManager::getInstance().m_xrfBkgdData.m_isLoaded)
  {
    m_xrfBkgdSliderPosition = 500;
  }

  if(!KFileManager::getInstance().m_xrfRefData.m_isLoaded)
  {
    m_xrfRefSliderPosition = 500;
  }
}

void MainWindow::print_error_message(const QString& a_content)
{
  WarningMsgBox msgBox(a_content);
  int ret = msgBox.exec();
}

void MainWindow::print_KED_results(const KKEDResults& a_kedResults)
{
  m_kedResults = a_kedResults;
  plot_KED_fit_curve();
  m_kedFitCheckBox->setVisible(true);
  m_kedFitCheckBox->setChecked(true);
  //print the Ceres results
  m_KEDFitDetailsAlgoField->setText(QString::fromStdString(m_kedResults.m_ceresResults.m_linearSolverType));
  m_KEDFitDetailsTermField->setText(QString::fromStdString(m_kedResults.m_ceresResults.m_terminationType));
  m_KEDFitDetailsMsgField->setText(QString::fromStdString(m_kedResults.m_ceresResults.m_message));
  m_KEDFitDetailsInitCostField->setText(QString::number(m_kedResults.m_ceresResults.m_initialCost));
  m_KEDFitDetailsFinalCostField->setText(QString::number(m_kedResults.m_ceresResults.m_finalCost));
  m_KEDFitDetailsCostDropField->setText(QString::number(m_kedResults.m_ceresResults.m_costDrop));
  m_KEDFitDetailsResidualField->setText(QString::number(m_kedResults.m_ceresResults.m_numResidualEvals));
  m_KEDFitDetailsJacobianField->setText(QString::number(m_kedResults.m_ceresResults.m_numJacobianEvals));
  m_KEDFitDetailsTotalItField->setText(QString::number(m_kedResults.m_ceresResults.m_totalIterations));
  m_KEDFitDetailsSuccItField->setText(QString::number(m_kedResults.m_ceresResults.m_numSuccessfulSteps));
  m_KEDFitDetailsUnsuccItField->setText(QString::number(m_kedResults.m_ceresResults.m_numUnsuccessfulSteps));
  m_KEDFitDetailsTotalTimeField->setText(QString::number(m_kedResults.m_ceresResults.m_totalTime));
  m_KEDFitDetailsJacobTimeField->setText(QString::number(m_kedResults.m_ceresResults.m_jacobianTime));
  m_KEDFitDetailsLinSolveTimeField->setText(QString::number(m_kedResults.m_ceresResults.m_linearSolverTime));
}

void MainWindow::print_XRF_results(const KXRFResults& a_xrfResults)
{
  m_xrfResults = a_xrfResults;
  plot_XRF_fit_curve();
}

void MainWindow::print_results_summary(const KResultsSummary& a_resultsSummary)
{
  //header info
  m_resultsSummaryDateField->setText(a_resultsSummary.m_date.toString(Qt::TextDate));
  m_resultsSummaryOperatorField->setText(a_resultsSummary.m_operator);
  m_resultsSummaryFacilityField->setText(a_resultsSummary.m_facility);
  m_resultsSummaryCommentField->setText(a_resultsSummary.m_comment);
  m_resultsSummaryKEDSampleField->setText(a_resultsSummary.m_kedSampleFile);
  m_resultsSummaryKEDBkgdField->setText(a_resultsSummary.m_kedBkgdFile);
  m_resultsSummaryKEDRefField->setText(a_resultsSummary.m_kedRefFile);
  m_resultsSummaryXRFSampleField->setText(a_resultsSummary.m_xrfSampleFile);
  m_resultsSummaryXRFBkgdField->setText(a_resultsSummary.m_xrfBkgdFile);
  m_resultsSummaryXRFRefField->setText(a_resultsSummary.m_xrfRefFile);
  m_resultsSummaryParamField->setText(a_resultsSummary.m_paramSet);
  QString anaMode;
  if(a_resultsSummary.m_anaMode == 0) anaMode = "Hybrid K-Edge";
  else if(a_resultsSummary.m_anaMode == 1) anaMode = "KED Only";
  else if(a_resultsSummary.m_anaMode == 2) anaMode = "XRF Only";
  else anaMode = "";
  m_resultsSummaryAnaTypeField->setText(anaMode);

  QString kedMode;
  if(a_resultsSummary.m_KEDMode == 0) kedMode = "Spectral Fitting Analysis";
  else if(a_resultsSummary.m_KEDMode == 1) kedMode = "Two-ROI Analysis";
  else kedMode = "";
  m_resultsSummaryKEDModeField->setText(kedMode);

  //analysis options
  m_resultsSummaryTempField->setText(QString::number(a_resultsSummary.m_sampTemp));

  //Sample results
  m_resultsSummaryUConcenField->setText(QString::number(a_resultsSummary.m_uConcentration));
  m_resultsSummaryNpConcenField->setText(QString::number(a_resultsSummary.m_npConcentration));
  m_resultsSummaryPuConcenField->setText(QString::number(a_resultsSummary.m_puConcentration));
  m_resultsSummaryAmConcenField->setText(QString::number(a_resultsSummary.m_amConcentration));
  m_resultsSummaryCmConcenField->setText(QString::number(a_resultsSummary.m_cmConcentration));
  //Sigma results
  m_resultsSummaryUSigmaField->setText(QString::number(a_resultsSummary.m_uSigma));
  m_resultsSummaryNpSigmaField->setText(QString::number(a_resultsSummary.m_npSigma));
  m_resultsSummaryPuSigmaField->setText(QString::number(a_resultsSummary.m_puSigma));
  m_resultsSummaryAmSigmaField->setText(QString::number(a_resultsSummary.m_amSigma));
  m_resultsSummaryCmSigmaField->setText(QString::number(a_resultsSummary.m_cmSigma));
  //RSD results
  m_resultsSummaryURSDField->setText(QString::number(a_resultsSummary.m_uRSD));
  m_resultsSummaryNpRSDField->setText(QString::number(a_resultsSummary.m_npRSD));
  m_resultsSummaryPuRSDField->setText(QString::number(a_resultsSummary.m_puRSD));
  m_resultsSummaryAmRSDField->setText(QString::number(a_resultsSummary.m_amRSD));
  m_resultsSummaryCmRSDField->setText(QString::number(a_resultsSummary.m_cmRSD));
}

void MainWindow::plot_KED_fit_curve()
{
  QVector<double> dataX(m_kedResults.m_fitROIEnergies.begin(),     m_kedResults.m_fitROIEnergies.end());
  QVector<double> dataY(m_kedResults.m_fitROIDataCounts.begin(),   m_kedResults.m_fitROIDataCounts.end());
  QVector<double> modelY(m_kedResults.m_fitROIModelCounts.begin(), m_kedResults.m_fitROIModelCounts.end());

  QVector<double> residualsX(m_kedResults.m_residualsX.begin(), m_kedResults.m_residualsX.end());
  QVector<double> residualsY(m_kedResults.m_residualsY.begin(), m_kedResults.m_residualsY.end());

  QVector<double> continuumCounts(m_kedResults.m_continuumCounts.begin(), m_kedResults.m_continuumCounts.end());
  QVector<double> coincCounts(m_kedResults.m_coincCounts.begin(), m_kedResults.m_coincCounts.end());

  double maxYValue = KMath::maxCounts(KFileManager::getInstance().m_kedSampData.m_counts);
  double maxXValue = KMath::maxCounts(KFileManager::getInstance().m_kedSampData.m_energies);

  m_numKEDPlots = KFileManager::getInstance().calc_KED_plot_number();
  if (m_kedPlot) 
  {
    //not checking to make sure there is data since this already done
    //when the file was loaded.
    m_kedPlot->addGraph();
    //graph id number is zero-indexed so number of plots is actually
    //the id of the next plot
    m_kedPlot->graph(m_numKEDPlots)->setData(dataX, modelY);
    m_kedPlot->graph(m_numKEDPlots)->setLineStyle(QCPGraph::lsLine);
    m_kedPlot->graph(m_numKEDPlots)->setPen(QPen(Qt::red));
    m_kedPlot->replot();
  }
  else
  {
    //FRY should change this to send error back to runamangaer?
    std::cout<< "OOPS! Something went wrong." << std::endl; 
  }

  if (m_kedResultsPlot) 
  {
    //Need max counts to set plot scale.
    //not checking to make sure there is data since this already done
    //when the file was loaded.
    //plot the data
    m_kedResultsPlot->addGraph();
    m_kedResultsPlot->graph(0)->setData(dataX, dataY);
    m_kedResultsPlot->graph(0)->setLineStyle(QCPGraph::lsStepCenter);
    m_kedResultsPlot->graph(0)->setPen(QPen(Qt::blue));
    //plot the fit curve
    m_kedResultsPlot->addGraph();
    m_kedResultsPlot->graph(1)->setData(dataX, modelY);
    m_kedResultsPlot->graph(1)->setLineStyle(QCPGraph::lsLine);
    m_kedResultsPlot->graph(1)->setPen(QPen(Qt::red));
    //plot the continuum
    m_kedResultsPlot->addGraph();
    m_kedResultsPlot->graph(2)->setData(dataX, continuumCounts);
    m_kedResultsPlot->graph(2)->setLineStyle(QCPGraph::lsLine);
    m_kedResultsPlot->graph(2)->setPen(QPen(Qt::green));
    //plot the coinc summing
    m_kedResultsPlot->addGraph();
    m_kedResultsPlot->graph(3)->setData(dataX, coincCounts);
    m_kedResultsPlot->graph(3)->setLineStyle(QCPGraph::lsLine);
    m_kedResultsPlot->graph(3)->setPen(QPen(Qt::gray));
    //m_kedResultsPlot->replot();
    
  }
  else
  {
    //FRY: should change this to send error back to runamangaer? FRY//
    std::cout<< "OOPS! Something went wrong." << std::endl; 
  }

  if (m_kedResidualsPlot) 
  {
    //get the data
    QVector<double> residualsX(m_kedResults.m_residualsX.begin(), m_kedResults.m_residualsX.end());
    QVector<double> residualsY(m_kedResults.m_residualsY.begin(), m_kedResults.m_residualsY.end());
    double maxYResValue = KMath::maxCounts(m_kedResults.m_residualsY);
    auto min_iterator = std::min_element(m_kedResults.m_residualsY.begin(), 
                                         m_kedResults.m_residualsY.end());
    double minYResValue = *min_iterator;
    //plot the data
    m_kedResidualsPlot->addGraph();
    m_kedResidualsPlot->graph(0)->setData(residualsX, residualsY);
    m_kedResidualsPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    m_kedResidualsPlot->graph(0)->setPen(QPen(Qt::red));
    m_kedResidualsPlot->yAxis->setRange(minYResValue*1.1, maxYResValue*1.1);
    m_kedResidualsPlot->replot();
  }
  else
  {
    //should change this to send error back to runamangaer?
    std::cout<< "OOPS! Something went wrong." << std::endl; 
  }

  const parameter_data::KParameterData& constants =
  KFileManager::getInstance().m_workingParameterSet;
  m_kedResultsPlot->xAxis->setRange(constants.m_KEDFitLowerBound, 
                                    constants.m_KEDFitUpperBound);
  m_kedResultsPlot->yAxis->setRange(0, maxYValue);
}

void MainWindow::plot_XRF_fit_curve()
{
  QVector<double> dataX(m_xrfResults.m_fitROIEnergies.begin(),     m_xrfResults.m_fitROIEnergies.end());
  QVector<double> dataY(m_xrfResults.m_fitROIDataCounts.begin(),   m_xrfResults.m_fitROIDataCounts.end());
  QVector<double> modelY(m_xrfResults.m_fitROIModelCounts.begin(), m_xrfResults.m_fitROIModelCounts.end());

  QVector<double> residualsX(m_xrfResults.m_residualsX.begin(), m_xrfResults.m_residualsX.end());
  QVector<double> residualsY(m_xrfResults.m_residualsY.begin(), m_xrfResults.m_residualsY.end());

  QVector<double> continuumCounts(m_xrfResults.m_continuumCounts.begin(), m_xrfResults.m_continuumCounts.end());
  QVector<double> coincCounts(m_xrfResults.m_coincCounts.begin(), m_xrfResults.m_coincCounts.end());

  double maxYValue = KMath::maxCounts(KFileManager::getInstance().m_xrfSampData.m_counts);
  double maxXValue = KMath::maxCounts(KFileManager::getInstance().m_xrfSampData.m_energies);

  m_numXRFPlots = KFileManager::getInstance().calc_XRF_plot_number();
  if (m_xrfPlot) 
  {
    //not checking to make sure there is data since this already done
    //when the file was loaded.
    m_xrfPlot->addGraph();
    //graph id number is zero-indexed so number of plots is actually
    //the id of the next plot
    m_xrfPlot->graph(m_numXRFPlots)->setData(dataX, modelY);
    m_xrfPlot->graph(m_numXRFPlots)->setLineStyle(QCPGraph::lsLine);
    m_xrfPlot->graph(m_numXRFPlots)->setPen(QPen(Qt::red));
    m_xrfPlot->replot();
  }
  else
  {
    //FRY should change this to send error back to runamangaer?
    std::cout<< "OOPS! Something went wrong." << std::endl; 
  }

  if (m_xrfResultsPlot) 
  {
    //Need max counts to set plot scale.
    //not checking to make sure there is data since this already done
    //when the file was loaded.
    //plot the data
    m_xrfResultsPlot->addGraph();
    m_xrfResultsPlot->graph(0)->setData(dataX, dataY);
    m_xrfResultsPlot->graph(0)->setLineStyle(QCPGraph::lsStepCenter);
    m_xrfResultsPlot->graph(0)->setPen(QPen(Qt::blue));
    //plot the fit curve
    m_xrfResultsPlot->addGraph();
    m_xrfResultsPlot->graph(1)->setData(dataX, modelY);
    m_xrfResultsPlot->graph(1)->setLineStyle(QCPGraph::lsLine);
    m_xrfResultsPlot->graph(1)->setPen(QPen(Qt::red));
    //plot the continuum
    //m_xrfResultsPlot->addGraph();
    //m_xrfResultsPlot->graph(2)->setData(dataX, continuumCounts);
    //m_xrfResultsPlot->graph(2)->setLineStyle(QCPGraph::lsLine);
    //m_xrfResultsPlot->graph(2)->setPen(QPen(Qt::green));
    //plot the coinc summing
    m_xrfResultsPlot->addGraph();
    m_xrfResultsPlot->graph(2)->setData(dataX, coincCounts);
    m_xrfResultsPlot->graph(2)->setLineStyle(QCPGraph::lsLine);
    m_xrfResultsPlot->graph(2)->setPen(QPen(Qt::gray));
    //m_xrfResultsPlot->replot();
    
  }
  else
  {
    //FRY: should change this to send error back to runamangaer? FRY//
    std::cout<< "OOPS! Something went wrong." << std::endl; 
  }

  if (m_xrfResidualsPlot) 
  {
    //get the data
    QVector<double> residualsX(m_xrfResults.m_residualsX.begin(), m_xrfResults.m_residualsX.end());
    QVector<double> residualsY(m_xrfResults.m_residualsY.begin(), m_xrfResults.m_residualsY.end());
    double maxYResValue = KMath::maxCounts(m_xrfResults.m_residualsY);
    auto min_iterator = std::min_element(m_xrfResults.m_residualsY.begin(), 
                                         m_xrfResults.m_residualsY.end());
    double minYResValue = *min_iterator;
    //plot the data
    m_xrfResidualsPlot->addGraph();
    m_xrfResidualsPlot->graph(0)->setData(residualsX, residualsY);
    m_xrfResidualsPlot->graph(0)->setLineStyle(QCPGraph::lsLine);
    m_xrfResidualsPlot->graph(0)->setPen(QPen(Qt::red));
    m_xrfResidualsPlot->yAxis->setRange(minYResValue*1.1, maxYResValue*1.1);
    m_xrfResidualsPlot->replot();
  }
  else
  {
    //should change this to send error back to runmanager?
    std::cout<< "OOPS! Something went wrong." << std::endl; 
  }

  const parameter_data::KParameterData& constants =
  KFileManager::getInstance().m_workingParameterSet;
  m_xrfResultsPlot->xAxis->setRange(constants.m_XRFFitLowerBound, 
                                    constants.m_XRFFitUpperBound);
  m_xrfResultsPlot->yAxis->setRange(0, maxYValue);
}


//void MainWindow::display_KED_cal_results()
//{
//  CalibrateKEDResultsDialog* dialog = new CalibrateKEDResultsDialog(this);
//  dialog->setAttribute(Qt::WA_DeleteOnClose);
//  dialog->exec(); // Show modally
//}

void MainWindow::display_XRF_cal_results()
{
  CalibrateXRFResultsDialog* dialog = new CalibrateXRFResultsDialog(this);
  dialog->setAttribute(Qt::WA_DeleteOnClose);
  dialog->exec(); // Show modally
}

//void MainWindow::display_KED_calibration()
//{
//  CalibrateKEDResultsDialog* dialog = new CalibrateKEDResultsDialog(this);
//  dialog->setAttribute(Qt::WA_DeleteOnClose);
//  dialog->exec(); // Show modally
//}

//void MainWindow::display_KED_calibration()
//{
//  CalibrateKEDResultsDialog* dialog = new CalibrateKEDResultsDialog(this);
//  dialog->setAttribute(Qt::WA_DeleteOnClose);
//  dialog->exec(); // Show modally
//}

/* 
    PRIVATE SLOTS
*/
void MainWindow::on_KED_lin_button_toggled(bool isChecked)
{
  if (!isChecked) return;

  m_kedPlot->yAxis->setScaleType(QCPAxis::stLinear);
  QSharedPointer<QCPAxisTicker> linearTicker(new QCPAxisTicker);
  linearTicker->setTickCount(6);
  m_kedPlot->yAxis->setTicker(linearTicker);
  m_kedPlot->yAxis->setNumberFormat("g");
  m_kedPlot->yAxis->setNumberPrecision(6);
  m_kedPlot->replot();

  KDevLogger::getInstance().logEvent("User selected KED Lin radio button");
}

void MainWindow::on_XRF_lin_button_toggled(bool isChecked)
{
  if (!isChecked) return;

  m_xrfPlot->yAxis->setScaleType(QCPAxis::stLinear);
  QSharedPointer<QCPAxisTicker> linearTicker(new QCPAxisTicker);
  linearTicker->setTickCount(6);
  m_xrfPlot->yAxis->setTicker(linearTicker);
  m_xrfPlot->yAxis->setNumberFormat("g");
  m_xrfPlot->yAxis->setNumberPrecision(6);
  m_xrfPlot->replot();

  KDevLogger::getInstance().logEvent("User selected XRF Lin radio button");
}

void MainWindow::on_KED_log_button_toggled(bool isChecked)
{
  if (!isChecked) return;

  m_kedPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
  QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
  logTicker->setLogBase(10);
  logTicker->setSubTickCount(8);
  m_kedPlot->yAxis->setTicker(logTicker);
  m_kedPlot->yAxis->setNumberFormat("eb");
  m_kedPlot->yAxis->setNumberPrecision(0);
  m_kedPlot->replot();

  KDevLogger::getInstance().logEvent("User selected KED Log radio button");
}

void MainWindow::on_XRF_log_button_toggled(bool isChecked)
{
  if (!isChecked) return;

  m_xrfPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
  QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
  logTicker->setLogBase(10);
  logTicker->setSubTickCount(8);
  m_xrfPlot->yAxis->setTicker(logTicker);
  m_xrfPlot->yAxis->setNumberFormat("eb");
  m_xrfPlot->yAxis->setNumberPrecision(0);
  m_xrfPlot->replot();

  KDevLogger::getInstance().logEvent("User selected XRF Log radio button");
}

void MainWindow::on_KED_results_lin_button_toggled(bool isChecked)
{
  if (!isChecked) return;

  m_kedResultsPlot->yAxis->setScaleType(QCPAxis::stLinear);
  QSharedPointer<QCPAxisTicker> linearTicker(new QCPAxisTicker);
  linearTicker->setTickCount(6);
  m_kedResultsPlot->yAxis->setTicker(linearTicker);
  m_kedResultsPlot->yAxis->setNumberFormat("g");
  m_kedResultsPlot->yAxis->setNumberPrecision(6);
  m_kedResultsPlot->replot();

  KDevLogger::getInstance().logEvent("User selected KED Results Lin radio button");
}

void MainWindow::on_KED_results_log_button_toggled(bool isChecked)
{
  if (!isChecked) return;

  m_kedResultsPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
  QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
  logTicker->setLogBase(10);
  logTicker->setSubTickCount(8);
  m_kedResultsPlot->yAxis->setTicker(logTicker);
  m_kedResultsPlot->yAxis->setNumberFormat("eb");
  m_kedResultsPlot->yAxis->setNumberPrecision(0);
  m_kedResultsPlot->replot();

  KDevLogger::getInstance().logEvent("User selected KED Results Log radio button");
}

void MainWindow::on_XRF_results_lin_button_toggled(bool isChecked)
{
  if (!isChecked) return;

  m_xrfResultsPlot->yAxis->setScaleType(QCPAxis::stLinear);
  QSharedPointer<QCPAxisTicker> linearTicker(new QCPAxisTicker);
  linearTicker->setTickCount(6);
  m_xrfResultsPlot->yAxis->setTicker(linearTicker);
  m_xrfResultsPlot->yAxis->setNumberFormat("g");
  m_xrfResultsPlot->yAxis->setNumberPrecision(6);
  m_xrfResultsPlot->replot();

  KDevLogger::getInstance().logEvent("User selected XRF Results Lin radio button");
}

void MainWindow::on_XRF_results_log_button_toggled(bool isChecked)
{
  if (!isChecked) return;

  m_xrfResultsPlot->yAxis->setScaleType(QCPAxis::stLogarithmic);
  QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
  logTicker->setLogBase(10);
  logTicker->setSubTickCount(8);
  m_xrfResultsPlot->yAxis->setTicker(logTicker);
  m_xrfResultsPlot->yAxis->setNumberFormat("eb");
  m_xrfResultsPlot->yAxis->setNumberPrecision(0);
  m_xrfResultsPlot->replot();

  KDevLogger::getInstance().logEvent("User selected KED Results Log radio button");
}

void MainWindow::on_KED_reset_button_clicked()
{
  if(KFileManager::getInstance().m_kedSampData.m_isLoaded)
  {
    KFileManager::getInstance().scale_KED_y_data(0, 1.0);
  }
  if(KFileManager::getInstance().m_kedBkgdData.m_isLoaded)
  {
    KFileManager::getInstance().scale_KED_y_data(1, 1.0);
  }
  if(KFileManager::getInstance().m_kedRefData.m_isLoaded)
  {
    KFileManager::getInstance().scale_KED_y_data(2, 1.0);
  }
  handle_KED_graphing();

  //setValue emits valueChanged. That can call on_KED_slider_changed() 
  //while you are halfway through reset. 
  //Block the signal during reset:
  {
    QSignalBlocker blocker(m_kedSlider);
    m_kedSlider->setValue(500);
  }

  m_kedSampSliderPosition = 500;
  m_kedBkgdSliderPosition = 500;
  m_kedRefSliderPosition = 500;
  m_kedScaleField->setText("1.00");
  m_kedPlot->rescaleAxes();
  
  if (m_kedLogButton->isChecked())
  {
    double upper = m_kedPlot->yAxis->range().upper;
    if (upper <= 1.0)
    {
      upper = 10.0;
    }
    m_kedPlot->yAxis->setRange(1.0, upper);
  }
  
  m_kedPlot->replot();
  KDevLogger::getInstance().logEvent("User selected KED Reset Plot button");
}

void MainWindow::on_XRF_reset_button_clicked()
{
  if(KFileManager::getInstance().m_xrfSampData.m_isLoaded)
  {
    KFileManager::getInstance().scale_XRF_y_data(0, 1.0);
  }
  if(KFileManager::getInstance().m_xrfBkgdData.m_isLoaded)
  {
    KFileManager::getInstance().scale_XRF_y_data(1, 1.0);
  }
  if(KFileManager::getInstance().m_xrfRefData.m_isLoaded)
  {
    KFileManager::getInstance().scale_XRF_y_data(2, 1.0);
  }
  handle_XRF_graphing();

  //setValue emits valueChanged. That can call on_XRF_slider_changed() 
  //while you are halfway through reset. 
  //Block the signal during reset:
  {
    QSignalBlocker blocker(m_xrfSlider);
    m_xrfSlider->setValue(500);
  }

  m_xrfSampSliderPosition = 500;
  m_xrfBkgdSliderPosition = 500;
  m_xrfRefSliderPosition = 500;
  m_xrfScaleField->setText("1.00");
  m_xrfPlot->rescaleAxes();
  
  if (m_xrfLogButton->isChecked())
  {
    double upper = m_xrfPlot->yAxis->range().upper;
    if (upper <= 1.0)
    {
      upper = 10.0;
    }
    m_xrfPlot->yAxis->setRange(1.0, upper);
  }

  m_xrfPlot->replot();
  KDevLogger::getInstance().logEvent("User selected XRF Reset Plot button");
}


void MainWindow::on_KED_samp_checkBox_toggled()
{
  handle_KED_graphing();
  bool state = m_kedSampCheckBox->isChecked();
  if(state)
  {
    KDevLogger::getInstance().logEvent("User selected KED sample check box");
  }
  else
  {
    KDevLogger::getInstance().logEvent("User deselected KED sample check box");
  }
}

void MainWindow::on_XRF_samp_checkBox_toggled()
{
  handle_XRF_graphing();
  bool state = m_xrfSampCheckBox->isChecked();
  if(state)
  {
    KDevLogger::getInstance().logEvent("User selected XRF sample check box");
  }
  else
  {
    KDevLogger::getInstance().logEvent("User deselected XRF sample check box");
  }
}

void MainWindow::on_KED_bkgd_checkBox_toggled()
{
  handle_KED_graphing();
  bool state = m_kedBkgdCheckBox->isChecked();
  if(state)
  {
    KDevLogger::getInstance().logEvent("User selected KED background check box");
  }
  else
  {
    KDevLogger::getInstance().logEvent("User deselected KED background check box");
  }
}

void MainWindow::on_XRF_bkgd_checkBox_toggled()
{
  handle_XRF_graphing();
  bool state = m_xrfBkgdCheckBox->isChecked();
  if(state)
  {
    KDevLogger::getInstance().logEvent("User selected XRF background check box");
  }
  else
  {
    KDevLogger::getInstance().logEvent("User deselected XRF background check box");
  }
}

void MainWindow::on_KED_ref_checkBox_toggled()
{
  handle_KED_graphing();
  bool state = m_kedRefCheckBox->isChecked();
  if(state)
  {
    KDevLogger::getInstance().logEvent("User selected KED reference check box");
  }
  else
  {
    KDevLogger::getInstance().logEvent("User deselected KED reference check box");
  }
}

void MainWindow::on_XRF_ref_checkBox_toggled()
{
  handle_XRF_graphing();
  bool state = m_xrfRefCheckBox->isChecked();
  if(state)
  {
    KDevLogger::getInstance().logEvent("User selected XRF reference check box");
  }
  else
  {
    KDevLogger::getInstance().logEvent("User deselected XRF reference check box");
  }
}

void MainWindow::on_KED_samp_comboBox_toggled()
{
  handle_KED_graphing();
  KDevLogger::getInstance().logEvent("User changed KED sample color");
}

void MainWindow::on_XRF_samp_comboBox_toggled()
{
  handle_XRF_graphing();
  KDevLogger::getInstance().logEvent("User changed XRF sample color");
}

void MainWindow::on_KED_bkgd_comboBox_toggled()
{
  handle_KED_graphing();
  KDevLogger::getInstance().logEvent("User changed KED background color");
}

void MainWindow::on_XRF_bkgd_comboBox_toggled()
{
  handle_XRF_graphing();
  KDevLogger::getInstance().logEvent("User changed XRF background color");
}

void MainWindow::on_KED_ref_comboBox_toggled()
{
  handle_KED_graphing();
  KDevLogger::getInstance().logEvent("User changed KED reference color");
}

void MainWindow::on_XRF_ref_comboBox_toggled()
{
  handle_XRF_graphing();
  KDevLogger::getInstance().logEvent("User changed XRF reference color");
}

void MainWindow::on_KED_x_grid_checkBox_toggled()
{
  if(m_kedXGridCheckBox->isChecked()) 
  {
    m_kedPlot->xAxis->grid()->setVisible(true);
    KDevLogger::getInstance().logEvent("User selected KED x-gridlines check box");
  } 
  else 
  {
    m_kedPlot->xAxis->grid()->setVisible(false);
    KDevLogger::getInstance().logEvent("User deselected KED x-gridlines check box");
  }
  m_kedPlot->replot();
}

void MainWindow::on_XRF_x_grid_checkBox_toggled()
{
  if(m_xrfXGridCheckBox->isChecked()) 
  {
    m_xrfPlot->xAxis->grid()->setVisible(true);
    KDevLogger::getInstance().logEvent("User selected XRF x-gridlines check box");
  } 
  else 
  {
    m_xrfPlot->xAxis->grid()->setVisible(false);
    KDevLogger::getInstance().logEvent("User deselected XRF x-gridlines check box");
  }
  m_xrfPlot->replot();
}

void MainWindow::on_KED_y_grid_checkBox_toggled()
{
  if(m_kedYGridCheckBox->isChecked()) 
  {
    m_kedPlot->yAxis->grid()->setVisible(true);
    KDevLogger::getInstance().logEvent("User selected KED y-gridlines check box");
  } 
  else 
  {
    m_kedPlot->yAxis->grid()->setVisible(false);
    KDevLogger::getInstance().logEvent("User deselected KED y-gridlines check box");
  }
  m_kedPlot->replot();
}

void MainWindow::on_XRF_y_grid_checkBox_toggled()
{
  if(m_xrfYGridCheckBox->isChecked()) 
  {
    m_xrfPlot->yAxis->grid()->setVisible(true);
    KDevLogger::getInstance().logEvent("User selected XRF y-gridlines check box");
  } 
  else 
  {
    m_xrfPlot->yAxis->grid()->setVisible(false);
    KDevLogger::getInstance().logEvent("User deselected XRF y-gridlines check box");
  }
  m_xrfPlot->replot();
}

void MainWindow::on_KED_cd109_checkBox_toggled()
{
  if (m_cd109Line == nullptr) 
  {
    m_cd109Line = new QCPItemStraightLine(m_kedPlot);
    m_cd109Line->point1->setCoords(88.03, 0);
    m_cd109Line->point2->setCoords(88.03, 1);
    m_cd109Line->setPen(QPen(Qt::black));
  }
  m_cd109Line->setVisible(m_kedCd109CheckBox->isChecked());
  if(m_kedCd109CheckBox->isChecked()) 
  {
    KDevLogger::getInstance().logEvent("User selected KED Cd-109 line check box");
  }
  else
  {
    KDevLogger::getInstance().logEvent("User deselected KED Cd-109 line check box");
  }
  m_kedPlot->replot();
}

void MainWindow::on_KED_Pu_KEdge_checkBox_toggled()
{
  if (m_puKEdgeLine == nullptr) 
  {
    m_puKEdgeLine = new QCPItemStraightLine(m_kedPlot);
    m_puKEdgeLine->point1->setCoords(121.8, 0);
    m_puKEdgeLine->point2->setCoords(121.8, 1);
    m_puKEdgeLine->setPen(QPen(Qt::black));
  }
  m_puKEdgeLine->setVisible(m_kedPuKEdgeCheckBox->isChecked());
  if(m_kedPuKEdgeCheckBox->isChecked()) 
  {
    KDevLogger::getInstance().logEvent("User selected KED Pu k-edge line check box");
  }
  else
  {
    KDevLogger::getInstance().logEvent("User deselected KED Pu k-edge line check box");
  }
  m_kedPlot->replot();
}

void MainWindow::on_KED_U_KEdge_checkBox_toggled()
{
  if (m_uKEdgeLine == nullptr) 
  {
    m_uKEdgeLine = new QCPItemStraightLine(m_kedPlot);
    m_uKEdgeLine->point1->setCoords(115.6, 0);
    m_uKEdgeLine->point2->setCoords(115.6, 1);
    m_uKEdgeLine->setPen(QPen(Qt::red));
  }
  m_uKEdgeLine->setVisible(m_kedUKEdgeCheckBox->isChecked());
  if(m_kedUKEdgeCheckBox->isChecked()) 
  {
    KDevLogger::getInstance().logEvent("User selected KED U k-edge line check box");
  }
  else
  {
    KDevLogger::getInstance().logEvent("User deselected KED U k-edge line check box");
  }
  m_kedPlot->replot();
}


void MainWindow::on_XRF_Pu_xray_checkBox_toggled()
{
  //Pu K-Alpha_1 McElroy MEXRF
  if(m_puKAlpha1Line==nullptr) 
  {
    m_puKAlpha1Line = new QCPItemStraightLine(m_xrfPlot);
    m_puKAlpha1Line->point1->setCoords(103.734, 0);
    m_puKAlpha1Line->point2->setCoords(103.734, 1);
    m_puKAlpha1Line->setPen(QPen(Qt::black));
  }
  //Pu K-Alpha_2 McElroy MEXRF
  if(m_puKAlpha2Line==nullptr) 
  {
    m_puKAlpha2Line = new QCPItemStraightLine(m_xrfPlot);
    m_puKAlpha2Line->point1->setCoords(99.5232, 0);
    m_puKAlpha2Line->point2->setCoords(99.5232, 1);
    m_puKAlpha2Line->setPen(QPen(Qt::black));
  }
  //Pu K-Alpha_3 McElroy MEXRF
  if(m_puKAlpha3Line==nullptr) 
  {
    m_puKAlpha3Line = new QCPItemStraightLine(m_xrfPlot);
    m_puKAlpha3Line->point1->setCoords(98.6815, 0);
    m_puKAlpha3Line->point2->setCoords(98.6815, 1);
    m_puKAlpha3Line->setPen(QPen(Qt::black));
  }
  //Pu K-Beta_1 McElroy MEXRF
  if(m_puKBeta1Line==nullptr)
  {
    m_puKBeta1Line = new QCPItemStraightLine(m_xrfPlot);
    m_puKBeta1Line->point1->setCoords(117.2437, 0); 
    m_puKBeta1Line->point2->setCoords(117.2437, 1);
    m_puKBeta1Line->setPen(QPen(Qt::black));
  }
  //Pu K-Beta_3 McElroy MEXRF
  if(m_puKBeta3Line==nullptr)
  {
    m_puKBeta3Line = new QCPItemStraightLine(m_xrfPlot);
    m_puKBeta3Line->point1->setCoords(116.2518, 0); 
    m_puKBeta3Line->point2->setCoords(116.2518, 1);
    m_puKBeta3Line->setPen(QPen(Qt::black));
  }
  //Pu K-Beta_2I Zchornak
  if(m_puKBeta2ILine==nullptr)
  {
    m_puKBeta2ILine = new QCPItemStraightLine(m_xrfPlot);
    m_puKBeta2ILine->point1->setCoords(120.674, 0); 
    m_puKBeta2ILine->point2->setCoords(120.674, 1);
    m_puKBeta2ILine->setPen(QPen(Qt::black));
  }
  //Pu K-Beta_2II Zchornak
  if(m_puKBeta2IILine==nullptr)
  {
    m_puKBeta2IILine = new QCPItemStraightLine(m_xrfPlot);
    m_puKBeta2IILine->point1->setCoords(120.4052, 0); 
    m_puKBeta2IILine->point2->setCoords(120.4052, 1);
    m_puKBeta2IILine->setPen(QPen(Qt::black));
  }
  //Pu K-Beta_4I McElroy MEXRF
  if(m_puKBeta4ILine==nullptr)
  {
    m_puKBeta4ILine = new QCPItemStraightLine(m_xrfPlot);
    m_puKBeta4ILine->point1->setCoords(120.991, 0); 
    m_puKBeta4ILine->point2->setCoords(120.991, 1);
    m_puKBeta4ILine->setPen(QPen(Qt::black));
  }
  m_puKAlpha1Line->setVisible(m_xrfPuXrayCheckBox->isChecked());
  m_puKAlpha2Line->setVisible(m_xrfPuXrayCheckBox->isChecked());
  m_puKAlpha3Line->setVisible(m_xrfPuXrayCheckBox->isChecked());
  m_puKBeta1Line->setVisible(m_xrfPuXrayCheckBox->isChecked());
  m_puKBeta3Line->setVisible(m_xrfPuXrayCheckBox->isChecked());
  m_puKBeta2ILine->setVisible(m_xrfPuXrayCheckBox->isChecked());
  m_puKBeta2IILine->setVisible(m_xrfPuXrayCheckBox->isChecked());
  m_puKBeta4ILine->setVisible(m_xrfPuXrayCheckBox->isChecked());
  if(m_xrfPuXrayCheckBox->isChecked())
  {
    KDevLogger::getInstance().logEvent("User selected XRF Pu x-rays check box");
  }
  else
  {
    KDevLogger::getInstance().logEvent("User deselected XRF Pu x-rays check box");
  }
  m_xrfPlot->replot();
}

void MainWindow::on_XRF_U_xray_checkBox_toggled()
{
  //U K-Alpha_1 McElroy MEXRF
  if(m_uKAlpha1Line==nullptr)
  {
    m_uKAlpha1Line = new QCPItemStraightLine(m_xrfPlot);
    m_uKAlpha1Line->point1->setCoords(98.434, 0); 
    m_uKAlpha1Line->point2->setCoords(98.434, 1);
    m_uKAlpha1Line->setPen(QPen(Qt::red));
  }
  //U K-Alpha_2 McElroy MEXRF
  if(m_uKAlpha2Line==nullptr)
  {
    m_uKAlpha2Line = new QCPItemStraightLine(m_xrfPlot);
    m_uKAlpha2Line->point1->setCoords(94.654, 0); 
    m_uKAlpha2Line->point2->setCoords(94.654, 1);
    m_uKAlpha2Line->setPen(QPen(Qt::red));
  }
  //U K-Beta_1 McElroy MEXRF
  if(m_uKBeta1Line==nullptr)
  {
    m_uKBeta1Line = new QCPItemStraightLine(m_xrfPlot);
    m_uKBeta1Line->point1->setCoords(111.298, 0); 
    m_uKBeta1Line->point2->setCoords(111.298, 1);
    m_uKBeta1Line->setPen(QPen(Qt::red));
  }

  //U K-Beta_2 McElroy MEXRF
  if(m_uKBeta2Line==nullptr)
  {
    m_uKBeta2Line = new QCPItemStraightLine(m_xrfPlot);
    m_uKBeta2Line->point1->setCoords(114.445, 0); 
    m_uKBeta2Line->point2->setCoords(114.445, 1);
    m_uKBeta2Line->setPen(QPen(Qt::red));
  }
  //U K-Beta_3 McElroy MEXRF
  if(m_uKBeta3Line==nullptr)
  {
    m_uKBeta3Line = new QCPItemStraightLine(m_xrfPlot);
    m_uKBeta3Line->point1->setCoords(110.421, 0); 
    m_uKBeta3Line->point2->setCoords(110.421, 1);
    m_uKBeta3Line->setPen(QPen(Qt::red));
  }
  //U K-Beta_4 McElroy MEXRF
  if(m_uKBeta4Line==nullptr)
  {
    m_uKBeta4Line = new QCPItemStraightLine(m_xrfPlot);
    m_uKBeta4Line->point1->setCoords(114.844, 0); 
    m_uKBeta4Line->point2->setCoords(114.844, 1);
    m_uKBeta4Line->setPen(QPen(Qt::red));
  }
  m_uKAlpha1Line->setVisible(m_xrfUXrayCheckBox->isChecked());
  m_uKAlpha2Line->setVisible(m_xrfUXrayCheckBox->isChecked());
  m_uKBeta1Line->setVisible(m_xrfUXrayCheckBox->isChecked());
  m_uKBeta3Line->setVisible(m_xrfUXrayCheckBox->isChecked());
  m_uKBeta2Line->setVisible(m_xrfUXrayCheckBox->isChecked());
  m_uKBeta4Line->setVisible(m_xrfUXrayCheckBox->isChecked());
  if(m_xrfUXrayCheckBox->isChecked())
  {
    KDevLogger::getInstance().logEvent("User selected XRF U x-rays check box");
  }
  else
  {
    KDevLogger::getInstance().logEvent("User deselected XRF U x-rays check box");
  }
  m_xrfPlot->replot();
}

void MainWindow::on_XRF_fission_checkBox_toggled()
{
  //Eu154 123 keV line
  if(m_eu154_123Line==nullptr)
  {
    m_eu154_123Line = new QCPItemStraightLine(m_xrfPlot);
    m_eu154_123Line->point1->setCoords(123.07, 0); 
    m_eu154_123Line->point2->setCoords(123.07, 1);
    m_eu154_123Line->setPen(QPen(Qt::green));
  }
  //Eu-155 105 keV line
  if(m_eu155_105Line==nullptr)
  {
    m_eu155_105Line = new QCPItemStraightLine(m_xrfPlot);
    m_eu155_105Line->point1->setCoords(105.31, 0); 
    m_eu155_105Line->point2->setCoords(105.31, 1);
    m_eu155_105Line->setPen(QPen(Qt::green));
  }
  m_eu154_123Line->setVisible(m_xrfFissionCheckBox->isChecked());
  m_eu155_105Line->setVisible(m_xrfFissionCheckBox->isChecked());
  if(m_xrfFissionCheckBox->isChecked())
  {
    KDevLogger::getInstance().logEvent("User selected XRF fission products check box");
  }
  else
  {
    KDevLogger::getInstance().logEvent("User deselected XRF fission products check box");
  }
  m_xrfPlot->replot();
}

void MainWindow::on_KED_fit_curve_button_toggled()
{
  //make sure the vectors have been filled before using
  if(!m_kedResults.m_fitROIEnergies.empty() && !m_kedResults.m_fitROIModelCounts.empty())
  {
    handle_KED_graphing(); //rerun plotter
  }
}

void MainWindow::on_KED_results_reset_button_clicked()
{
  const parameter_data::KParameterData& constants =
  KFileManager::getInstance().m_workingParameterSet;
  double maxYValue = KMath::maxCounts(KFileManager::getInstance().m_kedSampData.m_counts);
  m_kedResultsPlot->xAxis->setRange(constants.m_KEDFitLowerBound, 
                                    constants.m_KEDFitUpperBound);
  m_kedResultsPlot->yAxis->setRange(0, maxYValue);
  m_kedResultsPlot->replot();
  KDevLogger::getInstance().logEvent("User selected KED results Reset Plot button");
}

void MainWindow::on_XRF_results_reset_button_clicked()
{
  const parameter_data::KParameterData& constants =
  KFileManager::getInstance().m_workingParameterSet;
  double maxYValue = KMath::maxCounts(KFileManager::getInstance().m_xrfSampData.m_counts);
  m_xrfResultsPlot->xAxis->setRange(constants.m_XRFFitLowerBound, 
                                    constants.m_XRFFitUpperBound);
  m_xrfResultsPlot->yAxis->setRange(0, maxYValue);
  m_xrfResultsPlot->replot();
  KDevLogger::getInstance().logEvent("User selected KED results Reset Plot button");
}

void MainWindow::on_KED_results_fit_checkBox_toggled()
{
  if(m_kedResultsPlot->graph(1))
  {
    if(m_kedResultsFitCheckBox->isChecked()) 
    {
      m_kedResultsPlot->graph(1)->setVisible(true);
      KDevLogger::getInstance().logEvent("User selected KED results fit check box");
    } 
    else 
    {
      m_kedResultsPlot->graph(1)->setVisible(false);
      KDevLogger::getInstance().logEvent("User deselected KED results fit check box");
    }
    m_kedResultsPlot->replot();
  }
}

void MainWindow::on_XRF_results_fit_checkBox_toggled()
{
  if(m_xrfResultsPlot->graph(1))
  {
    if(m_xrfResultsFitCheckBox->isChecked()) 
    {
      m_xrfResultsPlot->graph(1)->setVisible(true);
      KDevLogger::getInstance().logEvent("User selected XRF results fit check box");
    } 
    else 
    {
      m_xrfResultsPlot->graph(1)->setVisible(false);
      KDevLogger::getInstance().logEvent("User deselected XRF results fit check box");
    }
    m_xrfResultsPlot->replot();
  }
}

void MainWindow::on_KED_results_continuum_checkBox_toggled()
{
  if(m_kedResultsPlot->graph(2))
  {
    if(m_kedResultsContCheckBox->isChecked()) 
    {
      m_kedResultsPlot->graph(2)->setVisible(true);
      KDevLogger::getInstance().logEvent("User selected KED results continuum check box");
    } 
    else 
    {
      m_kedResultsPlot->graph(2)->setVisible(false);
      KDevLogger::getInstance().logEvent("User deselected KED results continuum check box");
    }
    m_kedResultsPlot->replot();
  }
}

void MainWindow::on_XRF_results_continuum_checkBox_toggled()
{
  if(m_xrfResultsPlot->graph(2))
  {
    if(m_xrfResultsContCheckBox->isChecked()) 
    {
      m_xrfResultsPlot->graph(2)->setVisible(true);
      KDevLogger::getInstance().logEvent("User selected XRF results continuum check box");
    } 
    else 
    {
      m_xrfResultsPlot->graph(2)->setVisible(false);
      KDevLogger::getInstance().logEvent("User deselected XRF results continuum check box");
    }
    m_xrfResultsPlot->replot();
  }
}

void MainWindow::update_ked_ecal_boxes(std::array<float,3> a_coeffs)
{
  m_kedOffsetField->setText(QString::number(a_coeffs.at(0)));
  m_kedLinearField->setText(QString::number(a_coeffs.at(1)));
  m_kedQuadraticField->setText(QString::number(a_coeffs.at(2)));
}

void MainWindow::update_xrf_ecal_boxes(std::array<float,3> a_coeffs)
{
  m_xrfOffsetField->setText(QString::number(a_coeffs.at(0)));
  m_xrfLinearField->setText(QString::number(a_coeffs.at(1)));
  m_xrfQuadraticField->setText(QString::number(a_coeffs.at(2)));
}

void MainWindow::on_KED_Ecal_group_toggled(int a_id, bool a_checked)
{
  if (!a_checked) return; //only care about checked=true
  int m_kedEnergyCalGroupCurrentID = a_id;
  save_KED_slider_positons(m_kedEnergyCalGroupPrevID);
  m_kedEnergyCalGroupPrevID = m_kedEnergyCalGroupCurrentID;

  std::array<float,3> coeffs = {0.,0.,0.};
  switch (m_kedEnergyCalGroupCurrentID) 
  {
  case 0:
      coeffs.at(0) = KFileManager::getInstance().m_kedSampData.m_coeffs.at(0);
      coeffs.at(1) = KFileManager::getInstance().m_kedSampData.m_coeffs.at(1);
      if(KFileManager::getInstance().m_kedSampData.m_coeffs.size()>2)
      {
        coeffs.at(2) = KFileManager::getInstance().m_kedSampData.m_coeffs.at(2);
      }
      else
      {
        coeffs.at(2) = 0.;
        KFileManager::getInstance().m_kedSampData.m_coeffs.push_back(0.0);
      }
      update_ked_ecal_boxes(coeffs);
      //Update the slider info too
      m_kedSlider->setValue(m_kedSampSliderPosition);
      m_kedScaleField->setText(QString::number(map_slider_to_scaler(m_kedSampSliderPosition), 'f', 2));
      break;
  case 1:
      coeffs.at(0) = KFileManager::getInstance().m_kedBkgdData.m_coeffs.at(0);
      coeffs.at(1) = KFileManager::getInstance().m_kedBkgdData.m_coeffs.at(1);
      if(KFileManager::getInstance().m_kedBkgdData.m_coeffs.size()>2)
      {
        coeffs.at(2) = KFileManager::getInstance().m_kedBkgdData.m_coeffs.at(2);
      }
      else
      {
        coeffs.at(2) = 0.;
        KFileManager::getInstance().m_kedBkgdData.m_coeffs.push_back(0.0);
      }
      update_ked_ecal_boxes(coeffs);
      //Update the slider info too
      m_kedSlider->setValue(m_kedBkgdSliderPosition);
      m_kedScaleField->setText(QString::number(map_slider_to_scaler(m_kedBkgdSliderPosition), 'f', 2));
      break;
  case 2:
      coeffs.at(0) = KFileManager::getInstance().m_kedRefData.m_coeffs.at(0);
      coeffs.at(1) = KFileManager::getInstance().m_kedRefData.m_coeffs.at(1);
      if(KFileManager::getInstance().m_kedRefData.m_coeffs.size()>2)
      {
        coeffs.at(2) = KFileManager::getInstance().m_kedRefData.m_coeffs.at(2);
      }
      else
      {
        coeffs.at(2) = 0.;
        KFileManager::getInstance().m_kedRefData.m_coeffs.push_back(0.0);
      }
      update_ked_ecal_boxes(coeffs);
      //Update the slider info too
      m_kedSlider->setValue(m_kedRefSliderPosition);
      m_kedScaleField->setText(QString::number(map_slider_to_scaler(m_kedRefSliderPosition), 'f', 2));
      break;
  }
}

void MainWindow::on_XRF_Ecal_group_toggled(int a_id, bool a_checked)
{
  if (!a_checked) return; //only care about checked=true
  int m_xrfEnergyCalGroupCurrentID = a_id;
  save_XRF_slider_positons(m_xrfEnergyCalGroupPrevID);
  m_xrfEnergyCalGroupPrevID = m_xrfEnergyCalGroupCurrentID;

  std::array<float,3> coeffs = {0.,0.,0.};
  switch (m_xrfEnergyCalGroupCurrentID) 
  {
  case 0:
      coeffs.at(0) = KFileManager::getInstance().m_xrfSampData.m_coeffs.at(0);
      coeffs.at(1) = KFileManager::getInstance().m_xrfSampData.m_coeffs.at(1);
      if(KFileManager::getInstance().m_xrfSampData.m_coeffs.size()>2)
      {
        coeffs.at(2) = KFileManager::getInstance().m_xrfSampData.m_coeffs.at(2);;
      }
      else
      {
        coeffs.at(2) = 0.;
        KFileManager::getInstance().m_xrfSampData.m_coeffs.push_back(0.0);
      }
      update_xrf_ecal_boxes(coeffs);
      //Update the slider info too
      m_xrfSlider->setValue(m_xrfSampSliderPosition);
      m_xrfScaleField->setText(QString::number(map_slider_to_scaler(m_xrfSampSliderPosition), 'f', 2));
      break;
  case 1:
      coeffs.at(0) = KFileManager::getInstance().m_xrfBkgdData.m_coeffs.at(0);
      coeffs.at(1) = KFileManager::getInstance().m_xrfBkgdData.m_coeffs.at(1);
      if(KFileManager::getInstance().m_xrfBkgdData.m_coeffs.size()>2)
      {
        coeffs.at(2) = KFileManager::getInstance().m_xrfBkgdData.m_coeffs.at(2);;
      }
      else
      {
        coeffs.at(2) = 0.;
        KFileManager::getInstance().m_xrfBkgdData.m_coeffs.push_back(0.0);
      }
      update_xrf_ecal_boxes(coeffs);
      //Update the slider info too
      m_xrfSlider->setValue(m_xrfBkgdSliderPosition);
      m_xrfScaleField->setText(QString::number(map_slider_to_scaler(m_xrfBkgdSliderPosition), 'f', 2));
      break;
  case 2:
      coeffs.at(0) = KFileManager::getInstance().m_xrfRefData.m_coeffs.at(0);
      coeffs.at(1) = KFileManager::getInstance().m_xrfRefData.m_coeffs.at(1);
      if(KFileManager::getInstance().m_xrfRefData.m_coeffs.size()>2)
      {
        coeffs.at(2) = KFileManager::getInstance().m_xrfRefData.m_coeffs.at(2);;
      }
      else
      {
        coeffs.at(2) = 0.;
        KFileManager::getInstance().m_xrfRefData.m_coeffs.push_back(0.0);
      }
      update_xrf_ecal_boxes(coeffs);
      //Update the slider info too
      m_xrfSlider->setValue(m_xrfRefSliderPosition);
      m_xrfScaleField->setText(QString::number(map_slider_to_scaler(m_xrfRefSliderPosition), 'f', 2));
      break;
  }
}

void MainWindow::on_KED_mouse_move(QMouseEvent* a_event) 
{
  // map mouse position to plot coordinates for the default axes
  double xCoord = m_kedPlot->xAxis->pixelToCoord(a_event->pos().x());
  double yCoord = m_kedPlot->yAxis->pixelToCoord(a_event->pos().y());
  // Find the nearest data point on a specific graph (e.g., graph(0))
  if (m_kedPlot->graphCount() > 0) 
  {
    QCPGraph* graph = m_kedPlot->graph(0); // Assuming you want data from the first graph
    QCPGraphDataContainer::const_iterator it = graph->data()->findBegin(xCoord);
    if (it != graph->data()->constEnd()) 
    {
      QString tooltipText = QString("X: %1, Y: %2").arg(it->key).arg(it->value);
      m_kedPlot->setToolTip(tooltipText);
    } 
    else 
    {
      m_kedPlot->setToolTip(""); // Clear tooltip if no point is found
    }
  }
}

void MainWindow::on_XRF_mouse_move(QMouseEvent* a_event) 
{
  // map mouse position to plot coordinates for the default axes
  double xCoord = m_xrfPlot->xAxis->pixelToCoord(a_event->pos().x());
  double yCoord = m_xrfPlot->yAxis->pixelToCoord(a_event->pos().y());
  // Find the nearest data point on a specific graph (e.g., graph(0))
  if (m_xrfPlot->graphCount() > 0) 
  {
    QCPGraph* graph = m_xrfPlot->graph(0); // Assuming you want data from the first graph
    QCPGraphDataContainer::const_iterator it = graph->data()->findBegin(xCoord);
    if (it != graph->data()->constEnd()) 
    {
      QString tooltipText = QString("X: %1, Y: %2").arg(it->key).arg(it->value);
      m_xrfPlot->setToolTip(tooltipText);
    } 
    else 
    {
      m_xrfPlot->setToolTip(""); // Clear tooltip if no point is found
    }
  }
}

void MainWindow::on_KED_item_clicked(QCPAbstractItem *item, QMouseEvent *event)
{
  if(item == m_uKEdgeLine)
  {
    QString tooltipText = "U: 115.6 keV";
    QToolTip::showText(event->globalPos(), tooltipText, m_kedPlot);
  }
  else if(item == m_puKEdgeLine)
  {
    QString tooltipText = "Pu: 121.8 keV";
    QToolTip::showText(event->globalPos(), tooltipText, m_kedPlot);
  }
}

void MainWindow::on_XRF_item_clicked(QCPAbstractItem *item, QMouseEvent *event)
{
  if(item == m_puKAlpha1Line)
  {
    QString alpha = QChar(0x03B1);
    QString tooltipText = "Pu K" + alpha + "1: 103.734 keV";
    QToolTip::showText(event->globalPos(), tooltipText, m_xrfPlot);
  }
  if(item == m_puKAlpha2Line)
  {
    QString alpha = QChar(0x03B1);
    QString tooltipText = "Pu K" + alpha + "2: 99.5232 keV";
    QToolTip::showText(event->globalPos(), tooltipText, m_xrfPlot);
  }
  if(item == m_puKAlpha3Line)
  {
    QString alpha = QChar(0x03B1);
    QString tooltipText = "Pu K" + alpha + "3: 98.615 keV";
    QToolTip::showText(event->globalPos(), tooltipText, m_xrfPlot);
  }
  else if(item == m_puKBeta3Line)
  {
    QString beta = QChar(0x03B2);
    QString tooltipText = "Pu K" + beta + "3: 116.241 keV";
    QToolTip::showText(event->globalPos(), tooltipText, m_xrfPlot);
  }
  else if(item == m_puKBeta1Line)
  {
    QString beta = QChar(0x03B2);
    QString tooltipText = "Pu K" + beta + "1: 117.232 keV";
    QToolTip::showText(event->globalPos(), tooltipText, m_xrfPlot);
  }
  else if(item == m_puKBeta2ILine)
  {
    QString beta = QChar(0x03B2);
    QString tooltipText = "Pu K" + beta + "2I: 120.674 keV";
    QToolTip::showText(event->globalPos(), tooltipText, m_xrfPlot);
  }
  else if(item == m_puKBeta2IILine)
  {
    QString beta = QChar(0x03B2);
    QString tooltipText = "Pu K" + beta + "2II: 120.4052 keV";
    QToolTip::showText(event->globalPos(), tooltipText, m_xrfPlot);
  }
  else if(item == m_puKBeta4ILine)
  {
    QString beta = QChar(0x03B2);
    QString tooltipText = "Pu K" + beta + "4I: 120.991 keV";
    QToolTip::showText(event->globalPos(), tooltipText, m_xrfPlot);
  }
  else if(item == m_uKAlpha1Line)
  {
    QString alpha = QChar(0x03B1);
    QString tooltipText = "U K" + alpha + "1: 98.439 keV";
    QToolTip::showText(event->globalPos(), tooltipText, m_xrfPlot);
  }
  else if(item == m_uKAlpha2Line)
  {
    QString alpha = QChar(0x03B1);
    QString tooltipText = "U K" + alpha + "2: 94.653 keV";
    QToolTip::showText(event->globalPos(), tooltipText, m_xrfPlot);
  }
  else if(item == m_uKBeta1Line)
  {
    QString beta = QChar(0x03B2);
    QString tooltipText = "U K" + beta + "1: 111.296 keV";
    QToolTip::showText(event->globalPos(), tooltipText, m_xrfPlot);
  }
  else if(item == m_uKBeta3Line)
  {
    QString beta = QChar(0x03B2);
    QString tooltipText = "U K" + beta + "3: 110.416 keV";
    QToolTip::showText(event->globalPos(), tooltipText, m_xrfPlot);
  }
  else if(item == m_uKBeta2Line)
  {
    QString beta = QChar(0x03B2);
    QString tooltipText = "U K" + beta + "2: 114.117 keV";
    QToolTip::showText(event->globalPos(), tooltipText, m_xrfPlot);
  }
  else if(item == m_uKBeta4Line)
  {
    QString beta = QChar(0x03B2);
    QString tooltipText = "U K" + beta + "4: 115.011 keV";
    QToolTip::showText(event->globalPos(), tooltipText, m_xrfPlot);
  }
  else if(item == m_eu154_123Line)
  {
    QString tooltipText = "Eu154: 123.07 keV";
    QToolTip::showText(event->globalPos(), tooltipText, m_xrfPlot);
  }
  else if(item == m_eu155_105Line)
  {
    QString tooltipText = "Eu155: 105.31 keV";
    QToolTip::showText(event->globalPos(), tooltipText, m_xrfPlot);
  }
}

void MainWindow::on_KED_offsetField_return()
{
  QString newOffsetString = m_kedOffsetField->text();
  bool ok = false;
  float newOffset = newOffsetString.toFloat(&ok);
  if(!ok)
  {
    WarningMsgBox msgBox("Enter a valid energy calibration.");
    int ret = msgBox.exec();
    return;
  }
  int checkedId = m_kedEnergyCalGroup->checkedId();
  KFileManager::getInstance().update_KED_energy_calibration(checkedId, 0, newOffset);
  KKEDPlotManager::getInstance().setState(KFileManager::getInstance().calc_KED_state_number());
  KKEDPlotManager::getInstance().applyState(m_kedPlot);
}

void MainWindow::on_KED_linearField_return()
{
  QString newLinearString = m_kedLinearField->text();
  bool ok = false;
  float newLinear = newLinearString.toFloat(&ok);
  if(!ok)
  {
    WarningMsgBox msgBox("Enter a valid energy calibration.");
    int ret = msgBox.exec();
    return;
  }
  int checkedId = m_kedEnergyCalGroup->checkedId();
  KFileManager::getInstance().update_KED_energy_calibration(checkedId, 1, newLinear);
  KKEDPlotManager::getInstance().setState(KFileManager::getInstance().calc_KED_state_number());
  KKEDPlotManager::getInstance().applyState(m_kedPlot);
}

void MainWindow::on_KED_quadraticField_return()
{
  QString newQuadraticString = m_kedQuadraticField->text();
  bool ok = false;
  float newQuadratic = newQuadraticString.toFloat(&ok);
  if(!ok)
  {
    WarningMsgBox msgBox("Enter a valid energy calibration.");
    int ret = msgBox.exec();
    return;
  }
  int checkedId = m_kedEnergyCalGroup->checkedId();
  KFileManager::getInstance().update_KED_energy_calibration(checkedId, 2, newQuadratic);
  KKEDPlotManager::getInstance().setState(KFileManager::getInstance().calc_KED_state_number());
  KKEDPlotManager::getInstance().applyState(m_kedPlot);
}

void MainWindow::on_XRF_offsetField_return()
{
  QString newOffsetString = m_xrfOffsetField->text();
  bool ok = false;
  float newOffset = newOffsetString.toFloat(&ok);
  if(!ok)
  {
    WarningMsgBox msgBox("Enter a valid energy calibration.");
    int ret = msgBox.exec();
    return;
  }
  int checkedId = m_xrfEnergyCalGroup->checkedId();
  KFileManager::getInstance().update_XRF_energy_calibration(checkedId, 0, newOffset);
  KXRFPlotManager::getInstance().setState(KFileManager::getInstance().calc_XRF_state_number());
  KXRFPlotManager::getInstance().applyState(m_xrfPlot);
}

void MainWindow::on_XRF_linearField_return()
{
  QString newLinearString = m_xrfLinearField->text();
  bool ok = false;
  float newLinear = newLinearString.toFloat(&ok);
  if(!ok)
  {
    WarningMsgBox msgBox("Enter a valid energy calibration.");
    int ret = msgBox.exec();
    return;
  }
  int checkedId = m_xrfEnergyCalGroup->checkedId();
  KFileManager::getInstance().update_XRF_energy_calibration(checkedId, 1, newLinear);
  KXRFPlotManager::getInstance().setState(KFileManager::getInstance().calc_XRF_state_number());
  KXRFPlotManager::getInstance().applyState(m_xrfPlot);
}

void MainWindow::on_XRF_quadraticField_return()
{
  QString newQuadraticString = m_xrfQuadraticField->text();
  bool ok = false;
  float newQuadratic = newQuadraticString.toFloat(&ok);
  if(!ok)
  {
    WarningMsgBox msgBox("Enter a valid energy calibration.");
    int ret = msgBox.exec();
    return;
  }
  int checkedId = m_xrfEnergyCalGroup->checkedId();
  KFileManager::getInstance().update_XRF_energy_calibration(checkedId, 2, newQuadratic);
  KXRFPlotManager::getInstance().setState(KFileManager::getInstance().calc_XRF_state_number());
  KXRFPlotManager::getInstance().applyState(m_xrfPlot);
}

void MainWindow::on_ked_mouse_pressed(QMouseEvent* event)
{
  if(event->button() == Qt::RightButton) 
  {
    // Get pixel coordinates
    int peakPixel = event->pos().x();
    // map pixel coordinates to plot coordinates
    m_peakLocActual = m_kedPlot->xAxis->pixelToCoord(peakPixel);
  }
  else if(event->button() == Qt::LeftButton) 
  {
    return;
  }
}

void MainWindow::on_ked_mouse_released(QMouseEvent *event)
{
  if(event->button() == Qt::RightButton) 
  {
    // Get pixel coordinates
    int peakPixel = event->pos().x();
    // map pixel coordinates to plot coordinates
    m_peakLocDesired = m_kedPlot->xAxis->pixelToCoord(peakPixel);
    if(typeid(*KKEDPlotManager::getInstance().get_plot_state()) == typeid(KKEDDefaultPlotState)) 
    {
      return;
    }
    //get the initial cal vlaues
    int checkedId = m_kedEnergyCalGroup->checkedId();
    std::array<float,3> coeffs = KFileManager::getInstance().update_KED_energy_calibration(checkedId, m_peakLocActual, m_peakLocDesired);
    KKEDPlotManager::getInstance().setState(KFileManager::getInstance().calc_KED_state_number());
    KKEDPlotManager::getInstance().applyState(m_kedPlot);
    update_ked_ecal_boxes(coeffs);
  }
  else
  {
    return;
  }
}

void MainWindow::on_xrf_mouse_pressed(QMouseEvent* event)
{
  if(event->button() == Qt::RightButton) 
  {
    // Get pixel coordinates
    int peakPixel = event->pos().x();
    // map pixel coordinates to plot coordinates
    m_peakLocActual = m_xrfPlot->xAxis->pixelToCoord(peakPixel);
  }
  else if(event->button() == Qt::LeftButton) 
  {
    return;
  }
}

void MainWindow::on_xrf_mouse_released(QMouseEvent* event)
{
  if(event->button() == Qt::RightButton) 
  {
    // Get pixel coordinates
    int peakPixel = event->pos().x();
    // map pixel coordinates to plot coordinates
    m_peakLocDesired = m_xrfPlot->xAxis->pixelToCoord(peakPixel);
    if(typeid(*KXRFPlotManager::getInstance().get_plot_state()) == typeid(KXRFDefaultPlotState)) 
    {
      return;
    }
    //get the initial cal values
    int checkedId = m_xrfEnergyCalGroup->checkedId();
    std::array<float,3> coeffs = KFileManager::getInstance().update_XRF_energy_calibration(checkedId, m_peakLocActual, m_peakLocDesired);
    KXRFPlotManager::getInstance().setState(KFileManager::getInstance().calc_XRF_state_number());
    KXRFPlotManager::getInstance().applyState(m_xrfPlot);
    update_xrf_ecal_boxes(coeffs);
  }
  else
  {
    return;
  }
}

void MainWindow::on_KED_slider_changed()
{
  //make sure plot exists
  if(m_kedPlot)
  {
    int sliderValue = m_kedSlider->value();
    //If the signal comes from the slider then don't remake the plot every time. 
    //Only do it every four steps to reduce computing overherad.
    //If the signal source is the QLineEdit box then do it every time
    if(m_kedScaleSignalSource)
    {
      if(sliderValue % 4 == 0)
      {
        do_KED_slider_changed(sliderValue);
        std::string comment = "User changed KED slider to " + std::to_string(sliderValue);
        KDevLogger::getInstance().logEvent(comment);
      }
    }
    else
    {
      do_KED_slider_changed(sliderValue);
    }
  }
}

void MainWindow::do_KED_slider_changed(int a_sliderValue)
{
  int checkedId = m_kedEnergyCalGroup->checkedId();
  double mappedValue = map_slider_to_scaler(a_sliderValue);
  KFileManager::getInstance().scale_KED_y_data(checkedId, mappedValue);
  handle_KED_graphing();
  m_kedPlot->rescaleAxes();
  m_kedPlot->replot();
  m_kedScaleField->setText(QString::number(mappedValue, 'f', 2));
  save_KED_slider_positons(checkedId);
}

void MainWindow::on_XRF_slider_changed()
{
  //make sure plot exists
  if(m_xrfPlot)
  {
    int sliderValue = m_xrfSlider->value();
    //If the signal comes from the slider then don't remake the plot every time. 
    //Only do it every four steps to reduce computing overherad.
    //If the signal source is the QLineEdit box then do it every time
    if(m_xrfScaleSignalSource)
    {
      if(sliderValue % 4 == 0)
      {
        do_XRF_slider_changed(sliderValue);
        std::string comment = "User changed XRF slider to " + std::to_string(sliderValue);
        KDevLogger::getInstance().logEvent(comment);
      }
    }
    else
    {
      do_XRF_slider_changed(sliderValue);
    }
  }
}

void MainWindow::do_XRF_slider_changed(int a_sliderValue)
{
  int checkedId = m_xrfEnergyCalGroup->checkedId();
  double mappedValue = map_slider_to_scaler(a_sliderValue);
  KFileManager::getInstance().scale_XRF_y_data(checkedId, mappedValue);
  handle_XRF_graphing();
  m_xrfPlot->rescaleAxes();
  m_xrfPlot->replot();
  m_xrfScaleField->setText(QString::number(mappedValue, 'f', 2));
}

void MainWindow::save_KED_slider_positons(int a_prevID)
{
  switch(a_prevID)
  {
    case 0:
    {
      m_kedSampSliderPosition = m_kedSlider->value();
      break;
    }
    case 1:
    {
      m_kedBkgdSliderPosition = m_kedSlider->value();
      break;
    }
    case 2:
    {
      m_kedRefSliderPosition = m_kedSlider->value();
      break;
    }
    default: 
    {
      m_kedSampSliderPosition = m_kedSlider->value();
      break;
    }
  }
}

void MainWindow::save_XRF_slider_positons(int a_prevID)
{
  switch(a_prevID)
  {
    case 0:
    {
      m_xrfSampSliderPosition = m_xrfSlider->value();
      break;
    }
    case 1:
    {
      m_xrfBkgdSliderPosition = m_xrfSlider->value();
      break;
    }
    case 2:
    {
      m_xrfRefSliderPosition = m_xrfSlider->value();
      break;
    }
    default: 
    {
      m_xrfSampSliderPosition = m_xrfSlider->value();
      break;
    }
  }
}

void MainWindow::on_KED_scaler_return()
{
  // Use QDoubleValidator to restrict user entry
  QDoubleValidator* validator = new QDoubleValidator(0.0, 1000.00, 2, this);
  validator->setNotation(QDoubleValidator::StandardNotation);
  // Set the locale to 'C' to ensure the dot (.) is always the decimal separator
  validator->setLocale(QLocale::C); 
  // Apply it to the line edit
  m_kedScaleField->setValidator(validator);

  //set flag to false to bypass moduldo 
  m_kedScaleSignalSource = false; 
  //get the user entry
  QString newScaleString = m_kedScaleField->text();
  bool ok = false;
  double newScaleMapped = newScaleString.toDouble(&ok);
  //No check if !ok since QDoubleValidator does this already
  //Allowing use to enter 0-1000 so it is clear what the bounds are.
  //If I restrict QDoubleValitor to 0.1-10 then they don't know the limits
  //except by trial and error
  if(newScaleMapped > 10.) newScaleMapped = 10.;
  if(newScaleMapped < 0.1) newScaleMapped = 0.1;
  //map user entered value to slider position int
  int sliderValue = map_scaler_to_slider(newScaleMapped);
  int checkedId = m_kedEnergyCalGroup->checkedId();
  KFileManager::getInstance().scale_KED_y_data(checkedId, newScaleMapped);
  handle_KED_graphing();
  m_kedPlot->rescaleAxes();
  m_kedPlot->replot();
  m_kedSlider->setValue(sliderValue);
  save_KED_slider_positons(checkedId);
  std::string comment = "User changed KED slider scale box to " + std::to_string(newScaleMapped);
  KDevLogger::getInstance().logEvent(comment);
  m_kedScaleSignalSource = true; //reset flag back 
}

void MainWindow::on_XRF_scaler_return()
{
  // Use QDoubleValidator to restrict user entry
  QDoubleValidator* validator = new QDoubleValidator(0.0, 1000.00, 2, this);
  validator->setNotation(QDoubleValidator::StandardNotation);
  // Set the locale to 'C' to ensure the dot (.) is always the decimal separator
  validator->setLocale(QLocale::C); 
  // Apply it to the line edit
  m_xrfScaleField->setValidator(validator);

  //set flag to false to bypass moduldo 
  m_xrfScaleSignalSource = false; 
  //get the user entry
  QString newScaleString = m_xrfScaleField->text();
  bool ok = false;
  double newScaleMapped = newScaleString.toDouble(&ok);
  //No check if !ok since QDoubleValidator does this already
  //Allowing use to enter 0-1000 so it is clear what the bounds are.
  //If I restrict QDoubleValitor to 0.1-10 then they don't know the limits
  //except by trial and error
  if(newScaleMapped > 10.) newScaleMapped = 10.;
  if(newScaleMapped < 0.1) newScaleMapped = 0.1;
  //map user entered value to slider position int
  int sliderValue = map_scaler_to_slider(newScaleMapped);
  int checkedId = m_xrfEnergyCalGroup->checkedId();
  KFileManager::getInstance().scale_XRF_y_data(checkedId, newScaleMapped);
  handle_XRF_graphing();
  m_xrfPlot->rescaleAxes();
  m_xrfPlot->replot();
  m_xrfSlider->setValue(sliderValue);
  save_XRF_slider_positons(checkedId);
  std::string comment = "User changed XRF slider scale box to " + std::to_string(newScaleMapped);
  KDevLogger::getInstance().logEvent(comment);
  m_xrfScaleSignalSource = true; //reset flag back 
}

double MainWindow::map_slider_to_scaler(int a_sliderValue)
{
  return pow(10, a_sliderValue/500. -1.);
}

int MainWindow::map_scaler_to_slider(double a_scalerValue)
{
  return static_cast<int>(std::round((std::log10(a_scalerValue) + 1.)*500.));
}

void MainWindow::reset_KED_slider()
{
  m_kedSlider->setValue(500);
  m_kedSlider->setDisabled(true);
  m_kedSlider->setStyleSheet(
                              "QSlider:vertical \
                              {\
                                border: none;\
                              }\
                              QSlider::handle:vertical \
                              {\
                                background: lightgray;\
                                border: 1px solid lightgray;\
                                width: 18px;\
                                height: 9px;\
                                margin: -5px 0;\
                                border-radius: 9px;\
                              }");
}

void MainWindow::reset_XRF_slider()
{
  m_xrfSlider->setValue(500);
  m_xrfSlider->setDisabled(true);
  m_xrfSlider->setStyleSheet(
                              "QSlider:vertical \
                              {\
                                border: none;\
                              }\
                              QSlider::handle:vertical \
                              {\
                                background: lightgray;\
                                border: 1px solid lightgray;\
                                width: 18px;\
                                height: 9px;\
                                margin: -5px 0;\
                                border-radius: 9px;\
                              }");
}

void MainWindow::closeEvent(QCloseEvent* event)
{
  save_settings();
  event->accept();
}

void MainWindow::load_settings()
{
  QSettings settings;
  QString workingParamSet = settings.value("parameter_set", "EXAMPLE").toString();
  KFileManager::getInstance().load_previous_parameter_set(workingParamSet.toStdString());
}

void MainWindow::save_settings()
{
  QSettings settings;
  std::string workingParamSet = KFileManager::getInstance().m_workingParameterSet.m_setName;
  settings.setValue("parameter_set", QString::fromStdString(workingParamSet));
  std::string message = "Saved QSettings: parameter_set->" + workingParamSet;
  KDevLogger::getInstance().logEvent(message);
}

