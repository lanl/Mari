#include "calibratexrfdialog.h"

CalibrateXRFDialog::CalibrateXRFDialog(QWidget *parent)
    : QDialog(parent)
{
  KDevLogger::getInstance().logEvent("CalibrateXRFDialog object constructed");
  resize(560, 440);
  this->setWindowTitle("Calibrate XRF Instrument");
  this->setStyleSheet("background-color: white");

  //Main dialog vertical layout
  QVBoxLayout* analyzeVLayout = new QVBoxLayout(this);

  //Horizontal layout for the header
  QHBoxLayout* anaHeaderHLayout = new QHBoxLayout();
  QLabel* loadSpecHeader = new QLabel;
  loadSpecHeader->setText("Choose a parameter set to edit, or create a new one:");
  loadSpecHeader->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  anaHeaderHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  anaHeaderHLayout->addWidget(loadSpecHeader);

  //Horizontal layout for parameter set combo box
  QHBoxLayout* paramComboHLayout = new QHBoxLayout();
  QLabel* parameterLabel = new QLabel();
  parameterLabel->setText("Parameter Set: ");

  m_paramComboBox =  new QComboBox();
  m_paramComboBox->setStyleSheet("border-top: 2px solid gray;"
                                 "border-left: 2px solid gray;"
                                 "border-bottom: 1px solid lightgray;"
                                 "border-right: 1px solid lightgray;"
                                 "background-color: white;");
  m_paramComboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
  m_paramComboBox->setFixedHeight(m_fieldHeight);
  //Fill out the combo box with the parameter set names from the database
  auto it = std::find_if(KFileManager::getInstance().m_listofParamSets.begin(),
                         KFileManager::getInstance().m_listofParamSets.end(),
                         [](const std::string& name)
                         {
                           return name == KFileManager::getInstance().m_workingParameterSet.m_setName;
                         });

  if (it != KFileManager::getInstance().m_listofParamSets.end())
  {
    std::rotate(KFileManager::getInstance().m_listofParamSets.begin(), it, it + 1);
  }

  for (int i = 0; i < KFileManager::getInstance().m_listofParamSets.size(); i++)
  {
    m_paramComboBox->addItem(QString::fromStdString(KFileManager::getInstance().m_listofParamSets.at(i)));
  }
  paramComboHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  paramComboHLayout->addWidget(parameterLabel);
  paramComboHLayout->addWidget(m_paramComboBox);
  paramComboHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));

  //Horizontal layout for KED/XRF label
  QHBoxLayout* XRFLabelHLayout = new QHBoxLayout();
  QLabel* xrfLabel = new QLabel();
  xrfLabel->setText("Choose a Calibration (Reference) Spectrum:");
  xrfLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  XRFLabelHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  XRFLabelHLayout->addWidget(xrfLabel);

  //Create a horizontal line to separate analysis and File parts
  QFrame* anaFileLine = new QFrame();
  anaFileLine->setFrameShape(QFrame::HLine);
  anaFileLine->setStyleSheet("color: #006b75;");

// Create a container widget for the checkbox row
QWidget* subtractBkgdContainer = new QWidget();
subtractBkgdContainer->setFixedHeight(30); // Set to your desired height
QHBoxLayout* subtractBkgdCheckBoxHLayout = new QHBoxLayout(subtractBkgdContainer);
subtractBkgdCheckBoxHLayout->setContentsMargins(0, 0, 0, 0);
m_subBkgdCheckBox = new QCheckBox();
m_subBkgdCheckBox->setChecked(true);
m_subBkgdCheckLabel = new QLabel();
m_subBkgdCheckLabel->setText("Subtract Background");
subtractBkgdCheckBoxHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
subtractBkgdCheckBoxHLayout->addWidget(m_subBkgdCheckBox);
subtractBkgdCheckBoxHLayout->addWidget(m_subBkgdCheckLabel);
subtractBkgdCheckBoxHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
// Hide the widgets initially
m_subBkgdCheckBox->setVisible(false);
m_subBkgdCheckLabel->setVisible(false);

// Add the container instead of the layout
analyzeVLayout->addWidget(subtractBkgdContainer);

  //Horizontal layout for KED/XRF bkgd files
  QHBoxLayout* loadBkgdHLayout = new QHBoxLayout();
  QLabel* loadXrfBkgdLabel = new QLabel;
  loadXrfBkgdLabel->setText("Background:");
  m_loadXRFBkgdField = new NonUpdatableLabel;
  m_loadXRFBkgdField->setObjectName("loadXrfBkgdField");
  m_loadXRFBkgdField->setText(QString::fromStdString(KFileManager::getInstance().m_xrfBkgdData.m_filename));
  DeleteButton* xrfBkgdDelete = new DeleteButton();
  xrfBkgdDelete->setObjectName("xrfBkgdDelete");
  BrowseButton* loadXRFBkgdBrowse = new BrowseButton();
  loadBkgdHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  loadBkgdHLayout->addWidget(loadXrfBkgdLabel);
  loadBkgdHLayout->addWidget(m_loadXRFBkgdField);
  loadBkgdHLayout->addWidget(xrfBkgdDelete);
  loadBkgdHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  loadBkgdHLayout->addWidget(loadXRFBkgdBrowse);
  loadBkgdHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for KED/XRF reference files
  QHBoxLayout* loadRefHLayout = new QHBoxLayout();
  QLabel* loadXrfRefLabel = new QLabel;
  loadXrfRefLabel->setText("Reference:");
  m_loadXRFRefField = new NonUpdatableLabel;
  m_loadXRFRefField->setObjectName("loadXrfRefField");
  m_loadXRFRefField->setText(QString::fromStdString(KFileManager::getInstance().m_xrfRefData.m_filename));
  DeleteButton* xrfRefDelete = new DeleteButton();
  xrfRefDelete->setObjectName("xrfRefDelete");
  loadRefHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  BrowseButton* loadXRFRefBrowse = new BrowseButton();
  loadRefHLayout->addWidget(loadXrfRefLabel);
  loadRefHLayout->addItem(new QSpacerItem(12, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  loadRefHLayout->addWidget(m_loadXRFRefField);
  loadRefHLayout->addWidget(xrfRefDelete);
  loadRefHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  loadRefHLayout->addWidget(loadXRFRefBrowse);
  loadRefHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Create a horizontal line to separate File and Misc parts
  QFrame* fileSaveLine = new QFrame();
  fileSaveLine->setFrameShape(QFrame::HLine);
  fileSaveLine->setStyleSheet("color: #006b75;");

  //Horizontal layout for comment box
  QHBoxLayout* commentHLayout = new QHBoxLayout();
  QLabel* commentLabel = new QLabel();
  commentLabel->setText("Comment: ");
  m_commentField = new UpdatableLabel();
  commentHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  commentHLayout->addWidget(commentLabel);
  commentHLayout->addWidget(m_commentField);
  commentHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for status box
  QHBoxLayout* statusHLayout = new QHBoxLayout();
  QLabel* statusLabel = new QLabel();
  statusLabel->setText("Status: ");
  m_statusField = new NonUpdatableLabel();
  statusHLayout->addItem(new QSpacerItem(70, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  statusHLayout->addWidget(statusLabel);
  statusHLayout->addWidget(m_statusField);
  statusHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for ok and cancel buttons
  QHBoxLayout* okCancelHLayout = new QHBoxLayout();
  m_calibrateButton = new OkButton();
  m_calibrateButton->setText("Calibrate");
  m_calibrateButton->setFixedWidth(96);

  CancelButton* cancelButton = new CancelButton();
  okCancelHLayout->addItem(new QSpacerItem(10, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  okCancelHLayout->addWidget(m_calibrateButton);
  okCancelHLayout->addItem(new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  okCancelHLayout->addWidget(cancelButton);
  okCancelHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Put the H layouts in main V layout
  analyzeVLayout->addLayout(anaHeaderHLayout);
  analyzeVLayout->addLayout(paramComboHLayout);
  analyzeVLayout->addItem(new QSpacerItem(1, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));
  analyzeVLayout->addWidget(anaFileLine);
  analyzeVLayout->addLayout(XRFLabelHLayout);
  analyzeVLayout->addLayout(loadBkgdHLayout);
  analyzeVLayout->addLayout(loadRefHLayout);
  analyzeVLayout->addWidget(subtractBkgdContainer);
  analyzeVLayout->addItem(new QSpacerItem(1, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));
  analyzeVLayout->addWidget(fileSaveLine);
  analyzeVLayout->addItem(new QSpacerItem(1, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));
  analyzeVLayout->addLayout(commentHLayout);
  analyzeVLayout->addItem(new QSpacerItem(1, 7, QSizePolicy::Minimum, QSizePolicy::Fixed));
  analyzeVLayout->addLayout(statusHLayout);
  analyzeVLayout->addItem(new QSpacerItem(1, 30, QSizePolicy::Minimum, QSizePolicy::Fixed));
  analyzeVLayout->addLayout(okCancelHLayout);

  //connect cancel button to Qt internal close (reject) mechanism
  connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

  //connect ok button to internal slot
  connect(m_calibrateButton, &QPushButton::clicked, this, &CalibrateXRFDialog::on_calibrate_button_clicked);

  connect(xrfBkgdDelete, &DeleteButton::clicked, this, &CalibrateXRFDialog::on_bkgd_delete_button_clicked);
  connect(xrfRefDelete,  &DeleteButton::clicked, this, &CalibrateXRFDialog::on_ref_delete_button_clicked);

  //Retreive file name result from BrowseButton signal
  connect(loadXRFBkgdBrowse, &BrowseButton::foundFile, this, &CalibrateXRFDialog::on_bkgd_browse_button_clicked);
  connect(loadXRFRefBrowse,  &BrowseButton::foundFile, this, &CalibrateXRFDialog::on_ref_browse_button_clicked);

  // Initial button state
  update_analyze_button_state();
}

CalibrateXRFDialog::~CalibrateXRFDialog()
{
  KDevLogger::getInstance().logEvent("CalibrateXRFDialog object destructed");
}

std::string CalibrateXRFDialog::strip_path(std::string a_filenameAndPath)
{
  size_t slash_pos = a_filenameAndPath.find_last_of('/');
  std::string filename = (slash_pos != std::string::npos) ? a_filenameAndPath.substr(slash_pos + 1) : "";
  return filename;
}

bool CalibrateXRFDialog::check_if_data_loaded()
{
  bool xrfPass = false;
  xrfPass = KFileManager::getInstance().m_xrfRefData.m_isLoaded;

  return xrfPass;
}

bool CalibrateXRFDialog::can_analyze() const
{
  bool xrfPass = KFileManager::getInstance().m_xrfRefData.m_isLoaded;
  return xrfPass;
}

void CalibrateXRFDialog::update_analyze_button_state()
{
  bool ok = can_analyze();
  m_calibrateButton->setEnabled(ok);
  m_calibrateButton->set_style(ok);
}

void CalibrateXRFDialog::on_bkgd_browse_button_clicked(std::string a_filenameAndPath)
{
  KFileManager::getInstance().m_xrfBkgdData.m_filenameAndPath = a_filenameAndPath;
  std::string filename = strip_path(a_filenameAndPath);
  KFileManager::getInstance().m_xrfBkgdData.m_filename = filename;
  KDevLogger::getInstance().logEvent("User selected file " + filename +" as XRF Background");
  if (m_loadXRFBkgdField) 
  {
    m_loadXRFBkgdField->setText(QString::fromStdString(filename));
  }
  if(QString::fromStdString(filename)!="")
  {
    m_subBkgdCheckBox->setVisible(true);
    m_subBkgdCheckLabel->setVisible(true);
  }
  KFileManager::getInstance().m_xrfBkgdData.m_loadMe = true;
  if(KFileManager::getInstance().m_xrfBkgdData.m_loadMe == true) 
  {
    KFileManager::getInstance().load_with_SpecUtils(KFileManager::getInstance().m_xrfBkgdData);
  }
  update_analyze_button_state();
}

void CalibrateXRFDialog::on_ref_browse_button_clicked(std::string a_filenameAndPath)
{
  KFileManager::getInstance().m_xrfRefData.m_filenameAndPath = a_filenameAndPath;
  std::string filename = strip_path(a_filenameAndPath);
  KFileManager::getInstance().m_xrfRefData.m_filename = filename;
  KDevLogger::getInstance().logEvent("User selected file " + filename +" as XRF Reference");
  if (m_loadXRFRefField) 
  {
    m_loadXRFRefField->setText(QString::fromStdString(filename));
  }
  KFileManager::getInstance().m_xrfRefData.m_loadMe = true;
  if(KFileManager::getInstance().m_xrfRefData.m_loadMe == true) 
  {
    KFileManager::getInstance().load_with_SpecUtils(KFileManager::getInstance().m_xrfRefData);
  }
  update_analyze_button_state();
}

void CalibrateXRFDialog::handle_field_update()
{
  m_loadXRFBkgdField->setText(QString::fromStdString(KFileManager::getInstance().m_xrfBkgdData.m_filename));
  m_loadXRFRefField->setText(QString::fromStdString(KFileManager::getInstance().m_xrfRefData.m_filename));

  update_analyze_button_state();
}


void CalibrateXRFDialog::on_bkgd_delete_button_clicked()
{
  KFileManager::getInstance().m_xrfBkgdData.clear_data();
  m_loadXRFBkgdField->setText("");
  KFileManager::getInstance().m_xrfBkgdData.m_loadMe = false;
  KFileManager::getInstance().m_xrfBkgdData.m_isLoaded = false;
  m_subBkgdCheckBox->setVisible(false);
  m_subBkgdCheckLabel->setVisible(false);
  
  update_analyze_button_state();
}

void CalibrateXRFDialog::on_ref_delete_button_clicked()
{
  KFileManager::getInstance().m_xrfRefData.clear_data();
  m_loadXRFRefField->setText("");
  KFileManager::getInstance().m_xrfRefData.m_loadMe = false;
  KFileManager::getInstance().m_xrfRefData.m_isLoaded = false;
  update_analyze_button_state();
}

void CalibrateXRFDialog::update_status_field(QString a_status)
{
  m_statusField->setText(a_status);
}

void CalibrateXRFDialog::XRF_calibration_failure()
{
  WarningMsgBox msgBox("XRF Calibration Failed! \n  1) Check your uploaded file \n  2) Check your energy calibration");
  int ret = msgBox.exec();
  return;
}


void CalibrateXRFDialog::on_calibrate_button_clicked()
{
  // Defensive check in case button is triggered programmatically or state changed
  if (!check_if_data_loaded())
  {
    update_analyze_button_state();
    return;
  }

  // -------------------------------------------------------------
  // Guard against a second click while a previous run is alive
  // -------------------------------------------------------------
  if (m_calibrateThread && m_calibrateThread->isRunning())
  {
    WarningMsgBox msgBox(tr("Calibration already in progress – please wait."));
    return;
  }

  // -------------------------------------------------------------
  // Load the selected parameter set
  // -------------------------------------------------------------
  KFileManager::getInstance().load_parameter_set(
              m_paramComboBox->currentText().toStdString());
  emit load_new_spectra();

  // -------------------------------------------------------------
  // Create a 'fresh' thread + worker for this run
  // -------------------------------------------------------------
  m_calibrateThread  = new QThread();          // no parent
  m_calibrateWorker  = new KCalibrateWorker();  // no parent – will be moved

  m_calibrateWorker->moveToThread(m_calibrateThread);
  //we can go ahead and set the analysis type
  m_calibrateWorker->set_doKED(false);
  m_calibrateWorker->set_doXRF(true);

  // -------------------------------------------------------------
  // Wire the signals – all cross-thread connections are queued
  // -------------------------------------------------------------
  connect(m_calibrateThread, &QThread::started,
          m_calibrateWorker, &KCalibrateWorker::do_inst_calibration);

  connect(m_calibrateWorker, &KCalibrateWorker::update_status,
          this, &CalibrateXRFDialog::update_status_field,
          Qt::QueuedConnection);
  connect(m_calibrateWorker, &KCalibrateWorker::cal_XRF_success,
          this, &CalibrateXRFDialog::update_status_field,
          Qt::QueuedConnection);
  connect(m_calibrateWorker, &KCalibrateWorker::cal_XRF_success,
          &KRunManager::getInstance(), &KRunManager::cal_XRF_success,
          Qt::QueuedConnection);
  connect(m_calibrateWorker, &KCalibrateWorker::cal_XRF_fail,
          this, &CalibrateXRFDialog::XRF_calibration_failure,
          Qt::QueuedConnection);
  connect(m_calibrateWorker, &KCalibrateWorker::finished,
          m_calibrateWorker, &QObject::deleteLater);

  connect(m_calibrateWorker, &KCalibrateWorker::finished,
          m_calibrateThread, &QThread::quit);

  connect(m_calibrateThread, &QThread::finished,
          m_calibrateThread, &QObject::deleteLater);

  connect(m_calibrateThread, &QThread::finished,
          this, &CalibrateXRFDialog::on_thread_finished,
          Qt::QueuedConnection);

  // close the dialog when the analysis ends.
  connect(m_calibrateThread, &QThread::finished,
          this, &QDialog::accept,
          Qt::QueuedConnection);

  // -------------------------------------------------------------
  // Start the thread to do the analysis
  // -------------------------------------------------------------
  m_calibrateThread->start();
}

void CalibrateXRFDialog::on_thread_finished()
{
  m_calibrateWorker = nullptr;
  m_calibrateThread = nullptr;
}
