#include "calibratekeddialog.h"

CalibrateKEDDialog::CalibrateKEDDialog(QWidget *parent)
    : QDialog(parent)
{
  KDevLogger::getInstance().logEvent("CalibrateKEDDialog object constructed");
  resize(600, 510);
  this->setWindowTitle("Calibrate KED instrument");
  this->setStyleSheet("background-color: white");

  //Main dialog vertical layout
  QVBoxLayout* analyzeVLayout = new QVBoxLayout(this);

  //Horizontal layout for the header
  QHBoxLayout* anaHeaderHLayout = new QHBoxLayout();
  QLabel* loadSpecHeader = new QLabel;
  loadSpecHeader->setText("Calibration Options:");
  loadSpecHeader->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  anaHeaderHLayout->addWidget(loadSpecHeader);

  //Horizontal layout for KED/XRF check boxes
  QHBoxLayout* kedXRFCheckBoxHLayout = new QHBoxLayout();
  m_kedCheckBox = new QCheckBox();
  m_kedCheckBox->setObjectName("kedCheckBox");
  m_kedCheckBox->setChecked(true);

  QLabel* kedCheckLabel = new QLabel();
  kedCheckLabel->setText("K-Edge (KED)");

  m_xrfCheckBox =  new QCheckBox();
  m_xrfCheckBox->setObjectName("xrfCheckBox");
  m_xrfCheckBox->setChecked(true);

  QLabel* xrfCheckLabel = new QLabel();
  xrfCheckLabel->setText("X-Ray Fluorescence (XRF)");

  kedXRFCheckBoxHLayout->addItem(new QSpacerItem(26, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  kedXRFCheckBoxHLayout->addWidget(m_kedCheckBox);
  kedXRFCheckBoxHLayout->addWidget(kedCheckLabel);
  kedXRFCheckBoxHLayout->addItem(new QSpacerItem(50, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedXRFCheckBoxHLayout->addWidget(m_xrfCheckBox);
  kedXRFCheckBoxHLayout->addWidget(xrfCheckLabel);
  kedXRFCheckBoxHLayout->addItem(new QSpacerItem(26, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));

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
  CancelButton* newParamSetButton = new CancelButton();
  newParamSetButton->setText("New Parameter Set");
  newParamSetButton->setFixedWidth(140);
  paramComboHLayout->addWidget(parameterLabel);
  paramComboHLayout->addWidget(m_paramComboBox);
  paramComboHLayout->addItem(new QSpacerItem(100, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  paramComboHLayout->addWidget(newParamSetButton);
  paramComboHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for KED/XRF label
  QHBoxLayout* kedXrfLabelHLayout = new QHBoxLayout();
  QLabel* kedLabel = new QLabel();
  kedLabel->setText("KED Spectra");
  kedLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");

  QLabel* xrfLabel = new QLabel();
  xrfLabel->setText("XRF Spectra");
  xrfLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");

  kedXrfLabelHLayout->addWidget(kedLabel);
  kedXrfLabelHLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  kedXrfLabelHLayout->addWidget(xrfLabel);

  //Create a horizontal line to separate analysis and File parts
  QFrame* anaFileLine = new QFrame();
  anaFileLine->setFrameShape(QFrame::HLine);
  anaFileLine->setStyleSheet("color: #006b75;");

  //Horizontal layout for KED/XRF bkgd files
  QHBoxLayout* loadBkgdHLayout = new QHBoxLayout();
  QLabel* loadKedBkgdLabel = new QLabel;
  loadKedBkgdLabel->setText("Background:");
  m_loadKEDBkgdField = new NonUpdatableLabel;
  m_loadKEDBkgdField->setObjectName("loadKedBkgdField");
  m_loadKEDBkgdField->setText(QString::fromStdString(KFileManager::getInstance().m_kedBkgdData.m_filename));
  DeleteButton* kedBkgdDelete = new DeleteButton();
  kedBkgdDelete->setObjectName("kedBkgdDelete");

  QLabel* loadXrfBkgdLabel = new QLabel;
  loadXrfBkgdLabel->setText("Background:");
  m_loadXRFBkgdField = new NonUpdatableLabel;
  m_loadXRFBkgdField->setObjectName("loadXrfBkgdField");
  m_loadXRFBkgdField->setText(QString::fromStdString(KFileManager::getInstance().m_xrfBkgdData.m_filename));
  DeleteButton* xrfBkgdDelete = new DeleteButton();
  xrfBkgdDelete->setObjectName("xrfBkgdDelete");

  loadBkgdHLayout->addWidget(loadKedBkgdLabel);
  loadBkgdHLayout->addWidget(m_loadKEDBkgdField);
  loadBkgdHLayout->addWidget(kedBkgdDelete);
  loadBkgdHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  loadBkgdHLayout->addWidget(loadXrfBkgdLabel);
  loadBkgdHLayout->addWidget(m_loadXRFBkgdField);
  loadBkgdHLayout->addWidget(xrfBkgdDelete);

  //Horizontal layout for KED/XRF reference files
  QHBoxLayout* loadRefHLayout = new QHBoxLayout();
  QLabel* loadKedRefLabel = new QLabel;
  loadKedRefLabel->setText("Reference:");
  m_loadKEDRefField = new NonUpdatableLabel;
  m_loadKEDRefField->setObjectName("loadKedRefField");
  m_loadKEDRefField->setText(QString::fromStdString(KFileManager::getInstance().m_kedRefData.m_filename));
  DeleteButton* kedRefDelete = new DeleteButton();
  kedRefDelete->setObjectName("kedRefDelete");

  QLabel* loadXrfRefLabel = new QLabel;
  loadXrfRefLabel->setText("Reference:");
  m_loadXRFRefField = new NonUpdatableLabel;
  m_loadXRFRefField->setObjectName("loadXrfRefField");
  m_loadXRFRefField->setText(QString::fromStdString(KFileManager::getInstance().m_xrfRefData.m_filename));
  DeleteButton* xrfRefDelete = new DeleteButton();
  xrfRefDelete->setObjectName("xrfRefDelete");

  loadRefHLayout->addWidget(loadKedRefLabel);
  loadRefHLayout->addItem(new QSpacerItem(12, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  loadRefHLayout->addWidget(m_loadKEDRefField);
  loadRefHLayout->addWidget(kedRefDelete);
  loadRefHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  loadRefHLayout->addWidget(loadXrfRefLabel);
  loadRefHLayout->addItem(new QSpacerItem(12, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  loadRefHLayout->addWidget(m_loadXRFRefField);
  loadRefHLayout->addWidget(xrfRefDelete);

  //Horizontal layout for load file button
  QHBoxLayout* loadButtonHLayout = new QHBoxLayout();
  CancelButton* loadSpecButton = new CancelButton();
  loadSpecButton->setText("Open Spectrum");
  loadSpecButton->setFixedWidth(120);
  loadButtonHLayout->addWidget(loadSpecButton);

  //Create a horizontal line to separate File and Misc parts
  QFrame* fileSaveLine = new QFrame();
  fileSaveLine->setFrameShape(QFrame::HLine);
  fileSaveLine->setStyleSheet("color: #006b75;");

  //Horizontal layout for Operator box
  QHBoxLayout* operatorHLayout = new QHBoxLayout();
  QLabel* operatorLabel = new QLabel();
  operatorLabel->setText("Operator: ");
  m_operatorField = new UpdatableLabel();
  QLabel* facilityLabel = new QLabel();
  facilityLabel->setText("Facility: ");
  m_facilityField = new UpdatableLabel();
  operatorHLayout->addWidget(operatorLabel);
  operatorHLayout->addItem(new QSpacerItem(6, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  operatorHLayout->addWidget(m_operatorField);
  operatorHLayout->addItem(new QSpacerItem(10, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  operatorHLayout->addWidget(facilityLabel);
  operatorHLayout->addWidget(m_facilityField);
  operatorHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for comment box
  QHBoxLayout* commentHLayout = new QHBoxLayout();
  QLabel* commentLabel = new QLabel();
  commentLabel->setText("Comment: ");
  m_commentField = new UpdatableLabel();
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
  m_calibrateButton->setFixedWidth(90);

  CancelButton* cancelButton = new CancelButton();
  okCancelHLayout->addItem(new QSpacerItem(10, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  okCancelHLayout->addWidget(m_calibrateButton);
  okCancelHLayout->addItem(new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  okCancelHLayout->addWidget(cancelButton);
  okCancelHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Put the H layouts in main V layout
  analyzeVLayout->addLayout(anaHeaderHLayout);
  analyzeVLayout->addLayout(kedXRFCheckBoxHLayout);
  analyzeVLayout->addLayout(paramComboHLayout);
  analyzeVLayout->addItem(new QSpacerItem(1, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));
  analyzeVLayout->addWidget(anaFileLine);
  analyzeVLayout->addLayout(kedXrfLabelHLayout);
  analyzeVLayout->addLayout(loadBkgdHLayout);
  analyzeVLayout->addLayout(loadRefHLayout);
  analyzeVLayout->addItem(new QSpacerItem(1, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));
  analyzeVLayout->addLayout(loadButtonHLayout);
  analyzeVLayout->addItem(new QSpacerItem(1, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));
  analyzeVLayout->addWidget(fileSaveLine);
  analyzeVLayout->addItem(new QSpacerItem(1, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));
  analyzeVLayout->addLayout(operatorHLayout);
  analyzeVLayout->addLayout(commentHLayout);
  analyzeVLayout->addItem(new QSpacerItem(1, 7, QSizePolicy::Minimum, QSizePolicy::Fixed));
  analyzeVLayout->addLayout(statusHLayout);
  analyzeVLayout->addItem(new QSpacerItem(1, 30, QSizePolicy::Minimum, QSizePolicy::Fixed));
  analyzeVLayout->addLayout(okCancelHLayout);


  //connect load spec button to internal slot
  connect(loadSpecButton, &QPushButton::clicked, this, &CalibrateKEDDialog::on_load_spec_button_clicked);

  //connect cancel button to Qt internal close (reject) mechanism
  connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

  //connect ok button to internal slot
  connect(m_calibrateButton, &QPushButton::clicked, this, &CalibrateKEDDialog::on_calibrate_button_clicked);

  connect(m_kedCheckBox, &QCheckBox::toggled,    this, &CalibrateKEDDialog::on_kedCheckBox_clicked);
  connect(m_xrfCheckBox, &QCheckBox::toggled,    this, &CalibrateKEDDialog::on_xrfCheckBox_clicked);
  connect(kedBkgdDelete, &DeleteButton::clicked, this, &CalibrateKEDDialog::on_KED_bkgd_delete_button_clicked);
  connect(kedRefDelete,  &DeleteButton::clicked, this, &CalibrateKEDDialog::on_KED_ref_delete_button_clicked);
  connect(xrfBkgdDelete, &DeleteButton::clicked, this, &CalibrateKEDDialog::on_XRF_bkgd_delete_button_clicked);
  connect(xrfRefDelete,  &DeleteButton::clicked, this, &CalibrateKEDDialog::on_XRF_ref_delete_button_clicked);

  // Initial button state
  update_analyze_button_state();
}

CalibrateKEDDialog::~CalibrateKEDDialog()
{
  KDevLogger::getInstance().logEvent("CalibrateKEDDialog object destructed");
}

std::string CalibrateKEDDialog::strip_path(std::string a_filenameAndPath)
{
  size_t slash_pos = a_filenameAndPath.find_last_of('/');
  std::string filename = (slash_pos != std::string::npos) ? a_filenameAndPath.substr(slash_pos + 1) : "";
  return filename;
}

bool CalibrateKEDDialog::check_if_data_loaded()
{
  bool kedPass = false;
  bool xrfPass = false;

  if (m_kedCheckBox->isChecked())
  {
    kedPass = KFileManager::getInstance().m_kedRefData.m_isLoaded;
  }
  else
  {
    kedPass = true;
  }

  if (m_xrfCheckBox->isChecked())
  {
    xrfPass = KFileManager::getInstance().m_xrfRefData.m_isLoaded;
  }
  else
  {
    xrfPass = true;
  }

  return (kedPass && xrfPass);
}

bool CalibrateKEDDialog::can_analyze() const
{
  bool kedPass = !m_kedCheckBox->isChecked() ||
                 KFileManager::getInstance().m_kedRefData.m_isLoaded;

  bool xrfPass = !m_xrfCheckBox->isChecked() ||
                 KFileManager::getInstance().m_xrfRefData.m_isLoaded;
  return kedPass && xrfPass;
}

void CalibrateKEDDialog::update_analyze_button_state()
{
  bool ok = can_analyze();
  m_calibrateButton->setEnabled(ok);
  m_calibrateButton->set_style(ok);
}

void CalibrateKEDDialog::on_kedCheckBox_clicked()
{
  update_analyze_button_state();
}

void CalibrateKEDDialog::on_xrfCheckBox_clicked()
{
  update_analyze_button_state();
}

void CalibrateKEDDialog::on_load_spec_button_clicked()
{
  KDevLogger::getInstance().logEvent("User selected File | Open Spectrum from Analyze Dialog");
  LoadSpectrumDialog* loadDialog = new LoadSpectrumDialog(this);
  connect(loadDialog, &LoadSpectrumDialog::update_state_fields, this, &CalibrateKEDDialog::handle_field_update);
  loadDialog->setAttribute(Qt::WA_DeleteOnClose);
  loadDialog->exec(); // Show modally
}

void CalibrateKEDDialog::handle_field_update()
{
  m_loadKEDBkgdField->setText(QString::fromStdString(KFileManager::getInstance().m_kedBkgdData.m_filename));
  m_loadKEDRefField->setText(QString::fromStdString(KFileManager::getInstance().m_kedRefData.m_filename));
  m_loadXRFBkgdField->setText(QString::fromStdString(KFileManager::getInstance().m_xrfBkgdData.m_filename));
  m_loadXRFRefField->setText(QString::fromStdString(KFileManager::getInstance().m_xrfRefData.m_filename));

  update_analyze_button_state();
}

void CalibrateKEDDialog::on_KED_bkgd_delete_button_clicked()
{
  KFileManager::getInstance().m_kedBkgdData.clear_data();
  m_loadKEDBkgdField->setText("");
  KFileManager::getInstance().m_kedBkgdData.m_loadMe = false;
  KFileManager::getInstance().m_kedBkgdData.m_isLoaded = false;
  update_analyze_button_state();
}

void CalibrateKEDDialog::on_KED_ref_delete_button_clicked()
{
  KFileManager::getInstance().m_kedRefData.clear_data();
  m_loadKEDRefField->setText("");
  KFileManager::getInstance().m_kedRefData.m_loadMe = false;
  KFileManager::getInstance().m_kedRefData.m_isLoaded = false;
  update_analyze_button_state();
}

void CalibrateKEDDialog::on_XRF_bkgd_delete_button_clicked()
{
  KFileManager::getInstance().m_xrfBkgdData.clear_data();
  m_loadXRFBkgdField->setText("");
  KFileManager::getInstance().m_xrfBkgdData.m_loadMe = false;
  KFileManager::getInstance().m_xrfBkgdData.m_isLoaded = false;
  update_analyze_button_state();
}

void CalibrateKEDDialog::on_XRF_ref_delete_button_clicked()
{
  KFileManager::getInstance().m_xrfRefData.clear_data();
  m_loadXRFRefField->setText("");
  KFileManager::getInstance().m_xrfRefData.m_loadMe = false;
  KFileManager::getInstance().m_xrfRefData.m_isLoaded = false;
  update_analyze_button_state();
}

void CalibrateKEDDialog::update_status_field(QString a_status)
{
  m_statusField->setText(a_status);
}

void CalibrateKEDDialog::KED_calibration_failure()
{
  WarningMsgBox msgBox("KED Calibration Failed! \n  1) Check your uploaded file \n  2) Check your energy calibration");
  int ret = msgBox.exec();
  return;
}

void CalibrateKEDDialog::XRF_calibration_failure()
{
  WarningMsgBox msgBox("XRF Calibration Failed! \n  1) Check your uploaded file \n  2) Check your energy calibration");
  int ret = msgBox.exec();
  return;
}


void CalibrateKEDDialog::on_calibrate_button_clicked()
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
  set_analysis_options(m_calibrateWorker);
  save_analysis_header_info(m_calibrateWorker);

  // -------------------------------------------------------------
  // Wire the signals – all cross-thread connections are queued
  // -------------------------------------------------------------
  connect(m_calibrateThread, &QThread::started,
          m_calibrateWorker, &KCalibrateWorker::do_inst_calibration);

  connect(m_calibrateWorker, &KCalibrateWorker::update_status,
          this, &CalibrateKEDDialog::update_status_field,
          Qt::QueuedConnection);
  connect(m_calibrateWorker, &KCalibrateWorker::cal_KED_success,
          this, &CalibrateKEDDialog::update_status_field,
          Qt::QueuedConnection);
  connect(m_calibrateWorker, &KCalibrateWorker::cal_XRF_success,
          this, &CalibrateKEDDialog::update_status_field,
          Qt::QueuedConnection);
  connect(m_calibrateWorker, &KCalibrateWorker::cal_KED_fail,
          this, &CalibrateKEDDialog::KED_calibration_failure,
          Qt::QueuedConnection);
  connect(m_calibrateWorker, &KCalibrateWorker::cal_XRF_fail,
          this, &CalibrateKEDDialog::XRF_calibration_failure,
          Qt::QueuedConnection);
  connect(m_calibrateWorker, &KCalibrateWorker::finished,
          m_calibrateWorker, &QObject::deleteLater);

  connect(m_calibrateWorker, &KCalibrateWorker::finished,
          m_calibrateThread, &QThread::quit);

  connect(m_calibrateThread, &QThread::finished,
          m_calibrateThread, &QObject::deleteLater);

  connect(m_calibrateThread, &QThread::finished,
          this, &CalibrateKEDDialog::on_thread_finished,
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

void CalibrateKEDDialog::on_thread_finished()
{
  m_calibrateWorker = nullptr;
  m_calibrateThread = nullptr;
}

void CalibrateKEDDialog::set_analysis_options(KCalibrateWorker* a_worker)
{
  a_worker->set_doKED(m_kedCheckBox->isChecked());
  a_worker->set_doXRF(m_xrfCheckBox->isChecked());
}

void CalibrateKEDDialog::save_analysis_header_info(KCalibrateWorker* a_worker)
{
  a_worker->set_date(QDateTime::currentDateTime());
  a_worker->set_facility_name(m_facilityField->text());
  a_worker->set_operator_name(m_operatorField->text());
  a_worker->set_comment(m_commentField->text());
  a_worker->set_KED_bkgd_file(m_loadKEDBkgdField->text());
  a_worker->set_KED_ref_file(m_loadKEDRefField->text());
  a_worker->set_XRF_bkgd_file(m_loadXRFBkgdField->text());
  a_worker->set_XRF_ref_file(m_loadXRFRefField->text());
  a_worker->set_param_set(m_paramComboBox->currentText());

  int anaMode = 3;
  if(m_kedCheckBox->isChecked() && m_xrfCheckBox->isChecked()) anaMode = 0;
  else if(m_kedCheckBox->isChecked() && !m_xrfCheckBox->isChecked()) anaMode = 1;
  else if(!m_kedCheckBox->isChecked() && m_xrfCheckBox->isChecked()) anaMode = 2;
  else anaMode = 3;
  a_worker->set_ana_mode(anaMode);
}

