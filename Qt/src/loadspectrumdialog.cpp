#include "loadspectrumdialog.h"

LoadSpectrumDialog::LoadSpectrumDialog(QWidget *parent)
    : QDialog(parent)
{
  KDevLogger::getInstance().logEvent("LoadSpectrumDialog object constructed");
  resize(500, 370);
  this->setWindowTitle("Open Spectrum");
  //Dialog box central widget
  //QWidget* loadDiagWidget = new QWidget(this);
  this->setStyleSheet("background-color: white");
  //Main dialog vertical layout
  QVBoxLayout* loadDiagVLayout = new QVBoxLayout(this);

  //Horizontal layout for the header
  QHBoxLayout* loadHeaderHLayout = new QHBoxLayout();
  QLabel* loadSpecHeader = new QLabel;
  loadSpecHeader->setText("Choose one or more spectrum files to open:");
  loadSpecHeader->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  loadHeaderHLayout->addWidget(loadSpecHeader);

  //Horizontal layout for KED Sample
  QHBoxLayout* loadKedSampHLayout = new QHBoxLayout();
  QLabel* loadKedSampLabel = new QLabel;
  loadKedSampLabel->setText("KED Sample:");
  BrowseButton* loadKedSampBrowse = new BrowseButton();
  loadKedSampBrowse->setObjectName("loadKedSampBrowse");
  DeleteButton* loadKedSampDelete = new DeleteButton();
  loadKedSampDelete->setObjectName("loadKedSampDelete");
  m_loadKedSampField = new NonUpdatableLabel;
  m_loadKedSampField->setObjectName("loadKedSampField");
  m_loadKedSampField->setText(QString::fromStdString(KFileManager::getInstance().m_kedSampData.m_filename));
  loadKedSampHLayout->addWidget(loadKedSampLabel);
  loadKedSampHLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  loadKedSampHLayout->addWidget(m_loadKedSampField);
  loadKedSampHLayout->addWidget(loadKedSampDelete);
  loadKedSampHLayout->addItem(new QSpacerItem(10, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  loadKedSampHLayout->addWidget(loadKedSampBrowse);

  //Horizontal layout for KED Background
  QHBoxLayout* loadKedBkgdHLayout = new QHBoxLayout();
  QLabel* loadKedBkgdLabel = new QLabel;
  loadKedBkgdLabel->setText("KED Background:");
  BrowseButton* loadKedBkgdBrowse = new BrowseButton();
  loadKedBkgdBrowse->setObjectName("loadKedBkgdBrowse");
  DeleteButton* loadKedBkgdDelete = new DeleteButton();
  loadKedBkgdDelete->setObjectName("loadKedBkgdDelete");
  m_loadKedBkgdField = new NonUpdatableLabel;
  m_loadKedBkgdField->setObjectName("loadKedBkgdField");
  m_loadKedBkgdField->setText(QString::fromStdString(KFileManager::getInstance().m_kedBkgdData.m_filename));
  loadKedBkgdHLayout->addWidget(loadKedBkgdLabel);
  loadKedBkgdHLayout->addWidget(m_loadKedBkgdField);
  loadKedBkgdHLayout->addWidget(loadKedBkgdDelete);
  loadKedBkgdHLayout->addItem(new QSpacerItem(10, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  loadKedBkgdHLayout->addWidget(loadKedBkgdBrowse);

  //Horizontal layout for KED Reference
  QHBoxLayout* loadKedRefHLayout = new QHBoxLayout();
  QLabel* loadKedRefLabel = new QLabel;
  loadKedRefLabel->setText("KED Reference:");
  BrowseButton* loadKedRefBrowse = new BrowseButton();
  loadKedRefBrowse->setObjectName("loadKedRefBrowse");
  DeleteButton* loadKedRefDelete = new DeleteButton();
  loadKedRefDelete->setObjectName("loadKedRefDelete");
  m_loadKedRefField = new NonUpdatableLabel;
  m_loadKedRefField->setObjectName("loadKedRefField");
  m_loadKedRefField->setText(QString::fromStdString(KFileManager::getInstance().m_kedRefData.m_filename));
  loadKedRefHLayout->addWidget(loadKedRefLabel);
  loadKedRefHLayout->addItem(new QSpacerItem(12, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  loadKedRefHLayout->addWidget(m_loadKedRefField);
  loadKedRefHLayout->addWidget(loadKedRefDelete);
  loadKedRefHLayout->addItem(new QSpacerItem(10, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  loadKedRefHLayout->addWidget(loadKedRefBrowse);

  //Create a horizontal line to separate KED and XRF parts
  QFrame* kedXrfLine = new QFrame();
  kedXrfLine->setFrameShape(QFrame::HLine);
  kedXrfLine->setStyleSheet("color: #006b75;");

  //Horizontal layout for Xrf Sample
  QHBoxLayout* loadXrfSampHLayout = new QHBoxLayout();
  QLabel* loadXrfSampLabel = new QLabel;
  loadXrfSampLabel->setText("XRF Sample:");
  BrowseButton* loadXrfSampBrowse = new BrowseButton();
  loadXrfSampBrowse->setObjectName("loadXrfSampBrowse");
  DeleteButton* loadXrfSampDelete = new DeleteButton();
  loadXrfSampDelete->setObjectName("loadXrfSampDelete");
  m_loadXrfSampField = new NonUpdatableLabel;
  m_loadXrfSampField->setObjectName("loadXrfSampField");
  m_loadXrfSampField->setText(QString::fromStdString(KFileManager::getInstance().m_xrfSampData.m_filename));
  loadXrfSampHLayout->addWidget(loadXrfSampLabel);
  loadXrfSampHLayout->addItem(new QSpacerItem(26, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  loadXrfSampHLayout->addWidget(m_loadXrfSampField);
  loadXrfSampHLayout->addWidget(loadXrfSampDelete);
  loadXrfSampHLayout->addItem(new QSpacerItem(10, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  loadXrfSampHLayout->addWidget(loadXrfSampBrowse);

  //Horizontal layout for Xrf Background
  QHBoxLayout* loadXrfBkgdHLayout = new QHBoxLayout();
  QLabel* loadXrfBkgdLabel = new QLabel;
  loadXrfBkgdLabel->setText("XRF Background:");
  BrowseButton* loadXrfBkgdBrowse = new BrowseButton();
  loadXrfBkgdBrowse->setObjectName("loadXrfBkgdBrowse");
  DeleteButton* loadXrfBkgdDelete = new DeleteButton();
  loadXrfBkgdDelete->setObjectName("loadXrfBkgdDelete");
  m_loadXrfBkgdField = new NonUpdatableLabel;
  m_loadXrfBkgdField->setObjectName("loadXrfBkgdField");
  m_loadXrfBkgdField->setText(QString::fromStdString(KFileManager::getInstance().m_xrfBkgdData.m_filename));
  loadXrfBkgdHLayout->addWidget(loadXrfBkgdLabel);
  loadXrfBkgdHLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  loadXrfBkgdHLayout->addWidget(m_loadXrfBkgdField);
  loadXrfBkgdHLayout->addWidget(loadXrfBkgdDelete);
  loadXrfBkgdHLayout->addItem(new QSpacerItem(10, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  loadXrfBkgdHLayout->addWidget(loadXrfBkgdBrowse);

  //Horizontal layout for Xrf Reference
  QHBoxLayout* loadXrfRefHLayout = new QHBoxLayout();
  QLabel* loadXrfRefLabel = new QLabel;
  loadXrfRefLabel->setText("XRF Reference:");
  BrowseButton* loadXrfRefBrowse = new BrowseButton();
  loadXrfRefBrowse->setObjectName("loadXrfRefBrowse");
  DeleteButton* loadXrfRefDelete = new DeleteButton();
  loadXrfRefDelete->setObjectName("loadXrfRefDelete");
  m_loadXrfRefField = new NonUpdatableLabel;
  m_loadXrfRefField->setObjectName("loadXrfRefField");
  m_loadXrfRefField->setText(QString::fromStdString(KFileManager::getInstance().m_xrfRefData.m_filename));
  loadXrfRefHLayout->addWidget(loadXrfRefLabel);
  loadXrfRefHLayout->addItem(new QSpacerItem(13, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  loadXrfRefHLayout->addWidget(m_loadXrfRefField);
  loadXrfRefHLayout->addWidget(loadXrfRefDelete);
  loadXrfRefHLayout->addItem(new QSpacerItem(10, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  loadXrfRefHLayout->addWidget(loadXrfRefBrowse);

  //Horizontal layout for ok and cancel buttons
  QHBoxLayout* okCancelHLayout = new QHBoxLayout();
  OkButton* okButton = new OkButton();
  CancelButton* cancelButton = new CancelButton();
  okCancelHLayout->addItem(new QSpacerItem(10, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  okCancelHLayout->addWidget(okButton);
  okCancelHLayout->addItem(new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  okCancelHLayout->addWidget(cancelButton);
  okCancelHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Put the H layouts in main V layout
  loadDiagVLayout->addLayout(loadHeaderHLayout);
  loadDiagVLayout->addItem(new QSpacerItem(1, 15, QSizePolicy::Minimum, QSizePolicy::Fixed));
  loadDiagVLayout->addLayout(loadKedSampHLayout);
  loadDiagVLayout->addLayout(loadKedBkgdHLayout);
  loadDiagVLayout->addLayout(loadKedRefHLayout);
  loadDiagVLayout->addItem(new QSpacerItem(1, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));
  loadDiagVLayout->addWidget(kedXrfLine);
  loadDiagVLayout->addItem(new QSpacerItem(1, 10, QSizePolicy::Minimum, QSizePolicy::Fixed));
  loadDiagVLayout->addLayout(loadXrfSampHLayout);
  loadDiagVLayout->addLayout(loadXrfBkgdHLayout);
  loadDiagVLayout->addLayout(loadXrfRefHLayout);
  loadDiagVLayout->addItem(new QSpacerItem(1, 35, QSizePolicy::Minimum, QSizePolicy::Fixed));
  loadDiagVLayout->addLayout(okCancelHLayout);

  //connect cancel button to Qt internal close (reject) mechanism
  connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

  //connect ok button to internal slot
  connect(okButton, &QPushButton::clicked, this, &LoadSpectrumDialog::on_ok_button_clicked);

  //Retreive file name result from BrowseButton signal
  connect(loadKedSampBrowse, &BrowseButton::foundFile, this, &LoadSpectrumDialog::on_KED_samp_browse_button_clicked);
  connect(loadKedBkgdBrowse, &BrowseButton::foundFile, this, &LoadSpectrumDialog::on_KED_bkgd_browse_button_clicked);
  connect(loadKedRefBrowse,  &BrowseButton::foundFile, this, &LoadSpectrumDialog::on_KED_ref_browse_button_clicked);
  connect(loadXrfSampBrowse, &BrowseButton::foundFile, this, &LoadSpectrumDialog::on_XRF_samp_browse_button_clicked);
  connect(loadXrfBkgdBrowse, &BrowseButton::foundFile, this, &LoadSpectrumDialog::on_XRF_bkgd_browse_button_clicked);
  connect(loadXrfRefBrowse,  &BrowseButton::foundFile, this, &LoadSpectrumDialog::on_XRF_ref_browse_button_clicked);

  connect(loadKedSampDelete, &DeleteButton::clicked, this, &LoadSpectrumDialog::on_KED_samp_delete_button_clicked);
  connect(loadKedBkgdDelete, &DeleteButton::clicked, this, &LoadSpectrumDialog::on_KED_bkgd_delete_button_clicked);
  connect(loadKedRefDelete, &DeleteButton::clicked, this, &LoadSpectrumDialog::on_KED_ref_delete_button_clicked);
  connect(loadXrfSampDelete, &DeleteButton::clicked, this, &LoadSpectrumDialog::on_XRF_samp_delete_button_clicked);
  connect(loadXrfBkgdDelete, &DeleteButton::clicked, this, &LoadSpectrumDialog::on_XRF_bkgd_delete_button_clicked);
  connect(loadXrfRefDelete, &DeleteButton::clicked, this, &LoadSpectrumDialog::on_XRF_ref_delete_button_clicked);

  //send load spectrum signal to RunManager slot
  connect(this, &LoadSpectrumDialog::load_spectrum, &KRunManager::getInstance(), &KRunManager::loadSpectrum);
}

LoadSpectrumDialog::~LoadSpectrumDialog()
{
  KDevLogger::getInstance().logEvent("LoadSpectrumDialog object destructed");
}

std::string LoadSpectrumDialog::strip_path(std::string a_filenameAndPath)
{
  size_t slash_pos = a_filenameAndPath.find_last_of('/');
  std::string filename = (slash_pos != std::string::npos) ?a_filenameAndPath.substr(slash_pos + 1) : "";
  return filename;
}

void LoadSpectrumDialog::on_KED_samp_browse_button_clicked(std::string a_filenameAndPath)
{
  KFileManager::getInstance().m_kedSampData.m_filenameAndPath = a_filenameAndPath;
  std::string filename = strip_path(a_filenameAndPath);
  KFileManager::getInstance().m_kedSampData.m_filename = filename;
  KDevLogger::getInstance().logEvent("User selected file " + filename +" as KED Sample");
  if (m_loadKedSampField) 
  {
    m_loadKedSampField->setText(QString::fromStdString(filename));
  }
  KFileManager::getInstance().m_kedSampData.m_loadMe = true;
}

void LoadSpectrumDialog::on_KED_bkgd_browse_button_clicked(std::string a_filenameAndPath)
{
  KFileManager::getInstance().m_kedBkgdData.m_filenameAndPath = a_filenameAndPath;
  std::string filename = strip_path(a_filenameAndPath);
  KFileManager::getInstance().m_kedBkgdData.m_filename = filename;
  KDevLogger::getInstance().logEvent("User selected file " + filename +" as KED Background");
  if (m_loadKedBkgdField) 
  {
    m_loadKedBkgdField->setText(QString::fromStdString(filename));
  }
  KFileManager::getInstance().m_kedBkgdData.m_loadMe = true;
}

void LoadSpectrumDialog::on_KED_ref_browse_button_clicked(std::string a_filenameAndPath)
{
  KFileManager::getInstance().m_kedRefData.m_filenameAndPath = a_filenameAndPath;
  std::string filename = strip_path(a_filenameAndPath);
  KFileManager::getInstance().m_kedRefData.m_filename = filename;
  KDevLogger::getInstance().logEvent("User selected file " + filename +" as KED Reference");
  if (m_loadKedRefField) 
  {
    m_loadKedRefField->setText(QString::fromStdString(filename));
  }
  KFileManager::getInstance().m_kedRefData.m_loadMe = true;
}

void LoadSpectrumDialog::on_XRF_samp_browse_button_clicked(std::string a_filenameAndPath)
{
  KFileManager::getInstance().m_xrfSampData.m_filenameAndPath = a_filenameAndPath;
  std::string filename = strip_path(a_filenameAndPath);
  KFileManager::getInstance().m_xrfSampData.m_filename = filename;
  KDevLogger::getInstance().logEvent("User selected file " + filename +" as XRF Sample");
  if (m_loadXrfSampField) 
  {
    m_loadXrfSampField->setText(QString::fromStdString(filename));
  }
  KFileManager::getInstance().m_xrfSampData.m_loadMe = true;
}

void LoadSpectrumDialog::on_XRF_bkgd_browse_button_clicked(std::string a_filenameAndPath)
{
  KFileManager::getInstance().m_xrfBkgdData.m_filenameAndPath = a_filenameAndPath;
  std::string filename = strip_path(a_filenameAndPath);
  KFileManager::getInstance().m_xrfBkgdData.m_filename = filename;
  KDevLogger::getInstance().logEvent("User selected file " + filename +" as XRF Background");
  if (m_loadXrfBkgdField) 
  {
    m_loadXrfBkgdField->setText(QString::fromStdString(filename));
  }
  KFileManager::getInstance().m_xrfBkgdData.m_loadMe = true;
}

void LoadSpectrumDialog::on_XRF_ref_browse_button_clicked(std::string a_filenameAndPath)
{
  KFileManager::getInstance().m_xrfRefData.m_filenameAndPath = a_filenameAndPath;
  std::string filename = strip_path(a_filenameAndPath);
  KFileManager::getInstance().m_xrfRefData.m_filename = filename;
  KDevLogger::getInstance().logEvent("User selected file " + filename +" as XRf Reference");
  if (m_loadXrfRefField) 
  {
    m_loadXrfRefField->setText(QString::fromStdString(filename));
  }
  KFileManager::getInstance().m_xrfRefData.m_loadMe = true;
}


void LoadSpectrumDialog::on_KED_samp_delete_button_clicked()
{
  KFileManager::getInstance().m_kedSampData.clear_data();
  m_loadKedSampField->setText("");
  KFileManager::getInstance().m_kedSampData.m_loadMe = false;
  KFileManager::getInstance().m_kedSampData.m_isLoaded = false;
}

void LoadSpectrumDialog::on_KED_bkgd_delete_button_clicked()
{
  KFileManager::getInstance().m_kedBkgdData.clear_data();
  m_loadKedBkgdField->setText("");
  KFileManager::getInstance().m_kedBkgdData.m_loadMe = false;
  KFileManager::getInstance().m_kedBkgdData.m_isLoaded = false;
}

void LoadSpectrumDialog::on_KED_ref_delete_button_clicked()
{
  KFileManager::getInstance().m_kedRefData.clear_data();
  m_loadKedRefField->setText("");
  KFileManager::getInstance().m_kedRefData.m_loadMe = false;
  KFileManager::getInstance().m_kedRefData.m_isLoaded = false;
}

void LoadSpectrumDialog::on_XRF_samp_delete_button_clicked()
{
  KFileManager::getInstance().m_xrfSampData.clear_data();
  m_loadXrfSampField->setText("");
  KFileManager::getInstance().m_xrfSampData.m_loadMe = false;
  KFileManager::getInstance().m_xrfSampData.m_isLoaded = false;
}

void LoadSpectrumDialog::on_XRF_bkgd_delete_button_clicked()
{
  KFileManager::getInstance().m_xrfBkgdData.clear_data();
  m_loadXrfBkgdField->setText("");
  KFileManager::getInstance().m_xrfBkgdData.m_loadMe = false;
  KFileManager::getInstance().m_xrfBkgdData.m_isLoaded = false;
}

void LoadSpectrumDialog::on_XRF_ref_delete_button_clicked()
{
  KFileManager::getInstance().m_xrfRefData.clear_data();
  m_loadXrfRefField->setText("");
  KFileManager::getInstance().m_xrfRefData.m_loadMe = false;
  KFileManager::getInstance().m_xrfRefData.m_isLoaded = false;
}

void LoadSpectrumDialog::on_ok_button_clicked()
{
  //load selected files into memory
  if(KFileManager::getInstance().m_kedSampData.m_loadMe == true) 
  {
    emit load_spectrum(KFileManager::getInstance().m_kedSampData);
  }

  if(KFileManager::getInstance().m_kedBkgdData.m_loadMe == true) 
  {
    emit load_spectrum(KFileManager::getInstance().m_kedBkgdData);
  }

  if(KFileManager::getInstance().m_kedRefData.m_loadMe == true) 
  {
    emit load_spectrum(KFileManager::getInstance().m_kedRefData);
  }

  if(KFileManager::getInstance().m_xrfSampData.m_loadMe == true) 
  {
    emit load_spectrum(KFileManager::getInstance().m_xrfSampData);
  }

  if(KFileManager::getInstance().m_xrfBkgdData.m_loadMe == true) 
  {
    emit load_spectrum(KFileManager::getInstance().m_xrfBkgdData);
  }

  if(KFileManager::getInstance().m_xrfRefData.m_loadMe == true) 
  {
    emit load_spectrum(KFileManager::getInstance().m_xrfRefData);
  }

  //Now that all the data is loaded into memory, instruct the MainWindow to 
  //setup and draw the appropriate graphs upon dialog box close.
  emit handle_graphing();
  emit update_state_fields();
  //close window normally
  accept();
}