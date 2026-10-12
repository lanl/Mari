#include "newparameterdialog.h"

NewParameterDialog::NewParameterDialog(const int a_state, QWidget* parent)
    : QDialog(parent),
      m_paramNameField(nullptr),
      m_authorNameField(nullptr),
      m_facilityNameField(nullptr),
      m_dateEdit(nullptr),
      m_commentField(nullptr),
      m_hvField(nullptr),
      m_incidenceField(nullptr),
      m_takeoffField(nullptr),
      m_backscatterField(nullptr),
      m_beWindowField(nullptr),
      m_cdFilterField(nullptr),
      m_feFilterField(nullptr),
      m_xfrEntranceField(nullptr),
      m_xfrExitField(nullptr),
      m_KEDA1Field(nullptr),
      m_KEDA2Field(nullptr),
      m_KEDA3Field(nullptr),
      m_KEDT1Field(nullptr),
      m_KEDT2Field(nullptr),
      m_KEDT3Field(nullptr),
      m_KEDT4Field(nullptr),
      m_XRFA1Field(nullptr),
      m_XRFA2Field(nullptr),
      m_XRFA3Field(nullptr),
      m_XRFT1Field(nullptr),
      m_XRFT2Field(nullptr),
      m_XRFT3Field(nullptr),
      m_XRFT4Field(nullptr),
      m_vialIDField(nullptr),
      m_vialODField(nullptr),
      m_vialMaterialComboBox(nullptr),
      m_matrixMaterialComboBox(nullptr),
      m_KEDLowerBoundField(nullptr),
      m_KEDUpperBoundField(nullptr),
      m_KEDBkgdLowLowBoundField(nullptr),
      m_KEDBkgdLowHighBoundField(nullptr),
      m_KEDBkgdHighLowBoundField(nullptr),
      m_KEDBkgdHighHighBoundField(nullptr),
      m_XRFLowerBoundField(nullptr),
      m_XRFUpperBoundField(nullptr),
      m_XRFBkgdLowLowBoundField(nullptr),
      m_XRFBkgdLowHighBoundField(nullptr),
      m_XRFBkgdHighLowBoundField(nullptr),
      m_XRFBkgdHighHighBoundField(nullptr),
      m_KEDCdCorrField(nullptr),
      m_KEDBkgdNormField(nullptr),
      m_loadedParamSetName("default")
{
  KDevLogger::getInstance().logEvent("NewParameterDialog object constructed");
  //gui stuff
  resize(466, 539);
  this->setStyleSheet("background-color: white");
  QVBoxLayout* newParamVLayout = new QVBoxLayout(this);
  m_mainTabWidget = new QTabWidget();
  m_mainTabWidget->setObjectName("mainTabWidget");
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
  }");
  // Add tabs to the QTabWidget
  m_mainTabWidget->addTab(layout_general_tab(), "General");
  m_mainTabWidget->addTab(layout_inst_tab(), "Instrument.");
  m_mainTabWidget->addTab(layout_detector_tab(), "HPGe Det.");
  m_mainTabWidget->addTab(layout_sample_tab(), "Sample");
  m_mainTabWidget->addTab(layout_roi_tab(), "ROIs");

  //Horizontal layout for ok and cancel buttons
  QHBoxLayout* okCancelHLayout = new QHBoxLayout();
  m_okButton = new OkButton();
  m_okButton->setFixedWidth(65);
  CancelButton* cancelButton = new CancelButton();
  okCancelHLayout->addItem(new QSpacerItem(10, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  okCancelHLayout->addWidget(m_okButton);
  okCancelHLayout->addItem(new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  okCancelHLayout->addWidget(cancelButton);
  okCancelHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Put the H layouts in main V layout
  newParamVLayout->addWidget(m_mainTabWidget);
  newParamVLayout->addLayout(okCancelHLayout);
  //connect cancel button to Qt internal close (reject) mechanism
  connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
  //connect ok button to internal slot
  connect(m_okButton, &QPushButton::clicked, this, &NewParameterDialog::on_ok_button_clicked);
  //Question button connections
  connect(m_backScattBremButton,  &QPushButton::clicked, this, &NewParameterDialog::on_backscattBrem_button_clicked);

  if(a_state == 0)
  {
    setup_for_new();
  }
  else
  {
    setup_for_edit();
  }
}

NewParameterDialog::~NewParameterDialog()
{
  KDevLogger::getInstance().logEvent("NewParameterDialog object destructed");
}

QWidget* NewParameterDialog::layout_general_tab()
{
  QWidget* generalPage = new QWidget(m_mainTabWidget);
  generalPage->setObjectName("generalPage");
  //Create the main vertical layout
  QVBoxLayout* generalPgVLayout = new QVBoxLayout(generalPage);
  generalPgVLayout->setObjectName("generalPgVLayout");

  //Horizontal layout for file name box
  QHBoxLayout* paramNameHLayout = new QHBoxLayout();
  QLabel* paramNameLabel = new QLabel();
  paramNameLabel->setText("Parameter Set Name *");
  m_paramNameField = new UpdatableLabel();
  m_paramNameField->setObjectName("paramNameField");
  m_paramNameField->setMinimumWidth(250);
  paramNameHLayout->addWidget(paramNameLabel);
  paramNameHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  paramNameHLayout->addWidget(m_paramNameField);
  paramNameHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  
  connect(m_paramNameField, &QLineEdit::editingFinished, this, [this]() 
                      {
                        QString text = this->m_paramNameField->text(); 
                        bool isGood = check_valid_string(text);
                        //make sure the supplied name doesn't match another
                        if (text != m_loadedParamSetName)
                        {
                          auto it = std::find(std::begin(KFileManager::getInstance().m_listofParamSets), 
                                              std::end(KFileManager::getInstance().m_listofParamSets), 
                                              text);
                          if (it != std::end(KFileManager::getInstance().m_listofParamSets)) 
                          {
                            isGood=false;
                            WarningMsgBox msgBox("Name cannot match that of another parameter set.");
                            int ret = msgBox.exec();
                          }
                          //If name is bad, highlight box in red and keep cursor in box
                          if(isGood==false)
                          {
                            m_paramNameField->setFocus();
                            m_paramNameField->highlight();
                          }
                          else
                          {
                            m_paramNameField->unhighlight();
                          }
                          m_paramNameField->set_isValid(isGood);
                        }
                        else
                        {
                          m_paramNameField->set_isValid(isGood);
                          m_paramNameField->unhighlight();
                        }
                        update_ok_button_state();
                      });

  //Horizontal layout for author box
  QHBoxLayout* authorNameHLayout = new QHBoxLayout();
  QLabel* authorNameLabel = new QLabel();
  authorNameLabel->setText("Author ");
  m_authorNameField = new UpdatableLabel();
  m_authorNameField->setObjectName("authorNameField");
  m_authorNameField->setMinimumWidth(250);
  authorNameHLayout->addWidget(authorNameLabel);
  authorNameHLayout->addItem(new QSpacerItem(30, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  authorNameHLayout->addWidget(m_authorNameField);
  authorNameHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for facility box
  QHBoxLayout* facilityNameHLayout = new QHBoxLayout();
  QLabel* facilityNameLabel = new QLabel();
  facilityNameLabel->setText("Facility Name ");
  m_facilityNameField = new UpdatableLabel();
  m_facilityNameField->setObjectName("facilityNameField");
  m_facilityNameField->setMinimumWidth(250);
  facilityNameHLayout->addWidget(facilityNameLabel);
  facilityNameHLayout->addItem(new QSpacerItem(30, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  facilityNameHLayout->addWidget(m_facilityNameField);
  facilityNameHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for date box
  QHBoxLayout* dateHLayout = new QHBoxLayout();
  QLabel* dateLabel = new QLabel();
  dateLabel->setText("Date *");
  m_dateEdit = new QDateEdit();
  m_dateEdit->setCalendarPopup(true); // Enable the pop-up calendar
  m_dateEdit->setDate(QDate::currentDate()); // Set initial date
  dateHLayout->addWidget(dateLabel);
  dateHLayout->addItem(new QSpacerItem(70, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  dateHLayout->addWidget(m_dateEdit);
  dateHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));

  //Horizontal layout for the comment box
  QHBoxLayout* commentHLayout = new QHBoxLayout();
  QLabel* commentLabel = new QLabel();
  commentLabel->setText("Comment ");
  m_commentField = new UpdatableLabel();
  m_commentField->setObjectName("commentField");
  m_commentField->setMinimumWidth(300);
  commentHLayout->addWidget(commentLabel);
  commentHLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  commentHLayout->addWidget(m_commentField);
  commentHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Put the H layouts in main V layout
  generalPgVLayout->addLayout(paramNameHLayout);
  generalPgVLayout->addLayout(authorNameHLayout);
  generalPgVLayout->addLayout(facilityNameHLayout);
  generalPgVLayout->addLayout(dateHLayout);
  generalPgVLayout->addItem(new QSpacerItem(1, 20, QSizePolicy::Minimum, QSizePolicy::Fixed));
  generalPgVLayout->addLayout(commentHLayout);
  generalPgVLayout->addItem(new QSpacerItem(1, 170, QSizePolicy::Minimum, QSizePolicy::Fixed));

  return generalPage;
}

QWidget* NewParameterDialog::layout_inst_tab()
{
  QWidget* instPage = new QWidget();
  QVBoxLayout* instPageVLayout = new QVBoxLayout(instPage);
  QTabWidget* instTabWidget = new QTabWidget();
  instTabWidget->addTab(layout_inst_design_tab(), "Design Const.");
  instTabWidget->addTab(layout_inst_KED_empirical_tab(), "KED Emprircal Const.");
  instTabWidget->addTab(layout_inst_XRF_empirical_tab(), "XRF Emprircal Const.");
  instPageVLayout->addWidget(instTabWidget);
  return instPage;
}

QWidget* NewParameterDialog::layout_inst_design_tab()
{
  QWidget* xrayPage = new QWidget(m_mainTabWidget);
  xrayPage->setObjectName("xrayPage");
  //Create the main vertical layout
  QVBoxLayout* xrayPgVLayout = new QVBoxLayout(xrayPage);
  xrayPgVLayout->setObjectName("xrayPgVLayout");

  //Horizontal layout for HV box
  QHBoxLayout* hvHLayout = new QHBoxLayout();
  QLabel* hvLabel = new QLabel();
  hvLabel->setText("X-Ray Generator HV Setpoint (kV) *");
  m_hvField = new UpdatableLabel();
  m_hvField->setFixedWidth(100);
  hvHLayout->addWidget(hvLabel);
  hvHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  hvHLayout->addWidget(m_hvField);
  hvHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number. 
  QDoubleValidator* validator = new QDoubleValidator(0.0, 1000.0, 4, this);
  validator->setNotation(QDoubleValidator::StandardNotation);
  //Set the locale to 'C' to ensure the dot (.) is always the decimal separator
  validator->setLocale(QLocale::C); 
  //Apply it to the line edit
  m_hvField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_hvField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_hvField->validator() &&
                                  m_hvField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_hvField->set_isValid(isValid);
            m_hvField->update_field_style();
            update_ok_button_state();
          });

  //Horizontal layout for incidence angle box
  QHBoxLayout* incidenceHLayout = new QHBoxLayout();
  QLabel* incidenceLabel = new QLabel();
  incidenceLabel->setText("Electron Beam Incidence Angle (deg) *");
  m_incidenceField = new UpdatableLabel();
  m_incidenceField->setFixedWidth(100);
  incidenceHLayout->addWidget(incidenceLabel);
  incidenceHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  incidenceHLayout->addWidget(m_incidenceField);
  incidenceHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number. 
  m_incidenceField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_incidenceField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_incidenceField->validator() &&
                                  m_incidenceField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_incidenceField->set_isValid(isValid);
            m_incidenceField->update_field_style();
            update_ok_button_state();
          });

  //Horizontal layout for takeoff angle box
  QHBoxLayout* takeoffHLayout = new QHBoxLayout();
  QLabel* takeoffLabel = new QLabel();
  takeoffLabel->setText("X-Ray Takeoff Angle (deg) *");
  m_takeoffField = new UpdatableLabel();
  m_takeoffField->setFixedWidth(100);
  takeoffHLayout->addWidget(takeoffLabel);
  takeoffHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  takeoffHLayout->addWidget(m_takeoffField);
  takeoffHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number. 
  m_takeoffField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_takeoffField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_takeoffField->validator() &&
                                  m_takeoffField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_takeoffField->set_isValid(isValid);
            m_takeoffField->update_field_style();
            update_ok_button_state();
          });

  //Horizontal layout for backscatter angle box
  QHBoxLayout* backscatterHLayout = new QHBoxLayout();
  QLabel* backscatterLabel = new QLabel();
  backscatterLabel->setText("Min Backscatter Angle (deg) *");
  m_backscatterField = new UpdatableLabel();
  m_backscatterField->setFixedWidth(100);
  backscatterHLayout->addWidget(backscatterLabel);
  backscatterHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  backscatterHLayout->addWidget(m_backscatterField);
  backscatterHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number. 
  m_backscatterField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_backscatterField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_backscatterField->validator() &&
                                  m_backscatterField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_backscatterField->set_isValid(isValid);
            m_backscatterField->update_field_style();
            update_ok_button_state();
          });

  //Horizontal layout for Be window box
  QHBoxLayout* beWindowHLayout = new QHBoxLayout();
  QLabel* beWindowLabel = new QLabel();
  beWindowLabel->setText("Be Window Thickness (mm Be) *");
  m_beWindowField = new UpdatableLabel();
  m_beWindowField->setFixedWidth(100);
  beWindowHLayout->addWidget(beWindowLabel);
  beWindowHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  beWindowHLayout->addWidget(m_beWindowField);
  beWindowHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number. 
  m_beWindowField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_beWindowField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_beWindowField->validator() &&
                                  m_beWindowField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_beWindowField->set_isValid(isValid);
            m_beWindowField->update_field_style();
            update_ok_button_state();
          });

  //Horizontal layout for Cd filter box
  QHBoxLayout* cdFilterHLayout = new QHBoxLayout();
  QLabel* cdFilterLabel = new QLabel();
  cdFilterLabel->setText("Cd Filter Thickness (mm Cd) *");
  m_cdFilterField = new UpdatableLabel();
  m_cdFilterField->setFixedWidth(100);
  cdFilterHLayout->addWidget(cdFilterLabel);
  cdFilterHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  cdFilterHLayout->addWidget(m_cdFilterField);
  cdFilterHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number. 
  m_cdFilterField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_cdFilterField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_cdFilterField->validator() &&
                                  m_cdFilterField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_cdFilterField->set_isValid(isValid);
            m_cdFilterField->update_field_style();
            update_ok_button_state();
          });

  //Horizontal layout for Fe filter box
  QHBoxLayout* feFilterHLayout = new QHBoxLayout();
  QLabel* feFilterLabel = new QLabel();
  feFilterLabel->setText("Steel Filter Thickness (mm fe) *");
  m_feFilterField = new UpdatableLabel();
  m_feFilterField->setFixedWidth(100);
  feFilterHLayout->addWidget(feFilterLabel);
  feFilterHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  feFilterHLayout->addWidget(m_feFilterField);
  feFilterHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number. 
  m_feFilterField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_feFilterField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_feFilterField->validator() &&
                                  m_feFilterField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_feFilterField->set_isValid(isValid);
            m_feFilterField->update_field_style();
            update_ok_button_state();
          });

  //Horizontal layout for Xfr (transfer) entrance box
  QHBoxLayout* xfrEntranceHLayout = new QHBoxLayout();
  QLabel* xfrEntranceLabel = new QLabel();
  xfrEntranceLabel->setText("Transfer Tube Entrance Wall (mm Fe) *");
  m_xfrEntranceField = new UpdatableLabel();
  m_xfrEntranceField->setFixedWidth(100);
  xfrEntranceHLayout->addWidget(xfrEntranceLabel);
  xfrEntranceHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  xfrEntranceHLayout->addWidget(m_xfrEntranceField);
  xfrEntranceHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number. 
  m_xfrEntranceField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_xfrEntranceField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_xfrEntranceField->validator() &&
                                  m_xfrEntranceField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_xfrEntranceField->set_isValid(isValid);
            m_xfrEntranceField->update_field_style();
            update_ok_button_state();
          });

  //Horizontal layout for Xfr (transfer) exit box
  QHBoxLayout* xfrExitHLayout = new QHBoxLayout();
  QLabel* xfrExitLabel = new QLabel();
  xfrExitLabel->setText("Transfer Tube Exit Wall (mm Fe) *");
  m_xfrExitField = new UpdatableLabel();
  m_xfrExitField->setFixedWidth(100);
  xfrExitHLayout->addWidget(xfrExitLabel);
  xfrExitHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  xfrExitHLayout->addWidget(m_xfrExitField);
  xfrExitHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number. 
  m_xfrExitField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_xfrExitField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_xfrExitField->validator() &&
                                  m_xfrExitField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_xfrExitField->set_isValid(isValid);
            m_xfrExitField->update_field_style();
            update_ok_button_state();
          });

  //Put the H layouts in main V layout
  xrayPgVLayout->addLayout(hvHLayout);
  xrayPgVLayout->addLayout(incidenceHLayout);
  xrayPgVLayout->addLayout(takeoffHLayout);
  xrayPgVLayout->addLayout(backscatterHLayout);
  xrayPgVLayout->addLayout(beWindowHLayout);
  xrayPgVLayout->addLayout(cdFilterHLayout);
  xrayPgVLayout->addLayout(feFilterHLayout);
  xrayPgVLayout->addLayout(xfrEntranceHLayout);
  xrayPgVLayout->addLayout(xfrExitHLayout);

  return xrayPage;
}

QWidget* NewParameterDialog::layout_inst_KED_empirical_tab()
{
  QWidget* KEDEmpiricalPage = new QWidget(m_mainTabWidget);
  QVBoxLayout* KEDEmpiricalPgVLayout = new QVBoxLayout(KEDEmpiricalPage);

  //Horizontal layout for KED Cd peak correction factor box
  QHBoxLayout* KEDCdCorrHLayout = new QHBoxLayout();
  QLabel* KEDCdCorrLabel = new QLabel();
  KEDCdCorrLabel->setText("Cd-109 Correction Factor *");
  m_KEDCdCorrField = new UpdatableLabel();
  m_KEDCdCorrField->setFixedWidth(100);
  KEDCdCorrHLayout->addWidget(KEDCdCorrLabel);
  KEDCdCorrHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  KEDCdCorrHLayout->addWidget(m_KEDCdCorrField);
  KEDCdCorrHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number. 
  QDoubleValidator* validator = new QDoubleValidator(0.0, 1000.0, 4, this);
  validator->setNotation(QDoubleValidator::StandardNotation);
  //Set the locale to 'C' to ensure the dot (.) is always the decimal separator
  validator->setLocale(QLocale::C); 
  m_KEDCdCorrField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_KEDCdCorrField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_KEDCdCorrField->validator() &&
                                  m_KEDCdCorrField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_KEDCdCorrField->set_isValid(isValid);
            m_KEDCdCorrField->update_field_style();
            update_ok_button_state();
          });

  //Horizontal layout for KED Bkgd normalization factor box
  QHBoxLayout* KEDBkgdNormHLayout = new QHBoxLayout();
  QLabel* KEDBkgdNormLabel = new QLabel();
  KEDBkgdNormLabel->setText("Bkgd Normalization Factor *");
  m_KEDBkgdNormField = new UpdatableLabel();
  m_KEDBkgdNormField->setFixedWidth(100);
  KEDBkgdNormHLayout->addWidget(KEDBkgdNormLabel);
  KEDBkgdNormHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  KEDBkgdNormHLayout->addWidget(m_KEDBkgdNormField);
  KEDBkgdNormHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number. 
  m_KEDBkgdNormField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_KEDBkgdNormField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_KEDBkgdNormField->validator() &&
                                  m_KEDBkgdNormField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_KEDBkgdNormField->set_isValid(isValid);
            m_KEDBkgdNormField->update_field_style();
            update_ok_button_state();
          });

  //Horizontal layout for KED temperature factor box
  QHBoxLayout* KEDTempHLayout = new QHBoxLayout();
  QLabel* KEDTempLabel = new QLabel();
  KEDTempLabel->setText("Temperature Factor *");
  m_KEDTempField = new UpdatableLabel();
  m_KEDTempField->setFixedWidth(100);
  KEDTempHLayout->addWidget(KEDTempLabel);
  KEDTempHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  KEDTempHLayout->addWidget(m_KEDTempField);
  KEDTempHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number. 
  m_KEDTempField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_KEDTempField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_KEDTempField->validator() &&
                                  m_KEDTempField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_KEDTempField->set_isValid(isValid);
            m_KEDTempField->update_field_style();
            update_ok_button_state();
          });

  //Put the H layouts in main V layout
  KEDEmpiricalPgVLayout->addLayout(KEDCdCorrHLayout);
  KEDEmpiricalPgVLayout->addLayout(KEDBkgdNormHLayout);
  KEDEmpiricalPgVLayout->addLayout(KEDTempHLayout);

  return KEDEmpiricalPage;
}

QWidget* NewParameterDialog::layout_inst_XRF_empirical_tab()
{
  QWidget* XRFEmpiricalPage = new QWidget(m_mainTabWidget);
  //Create the main vertical layout
  QVBoxLayout* XRFEmpiricalPgVLayout = new QVBoxLayout(XRFEmpiricalPage);

  //Horizontal layout for backscatter Brem title.
  QHBoxLayout* backScattBremTitleHLayout = new QHBoxLayout();
  QLabel* backScattBremTitleLabel = new QLabel;
  backScattBremTitleLabel->setText("Backscattered Bremsstrahlung Continuum");
  backScattBremTitleLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  m_backScattBremButton = new QuestionButton;
  backScattBremTitleHLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  backScattBremTitleHLayout->addWidget(backScattBremTitleLabel);
  backScattBremTitleHLayout->addWidget(m_backScattBremButton);
  backScattBremTitleHLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  backScattBremTitleHLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for C box
  QHBoxLayout* CHLayout = new QHBoxLayout();
  QLabel* CLabel = new QLabel();
  CLabel->setText("C *");
  m_CField = new UpdatableLabel();
  m_CField->setFixedWidth(100);
  //check to make sure user entered a real number. 
  QDoubleValidator* validator = new QDoubleValidator(-100000.0, 100000.0, 4, this);
  validator->setNotation(QDoubleValidator::StandardNotation);
  //Set the locale to 'C' to ensure the dot (.) is always the decimal separator
  validator->setLocale(QLocale::C); 
  //Apply it to the line edit
  m_CField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_CField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_CField->validator() &&
                                  m_CField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_CField->set_isValid(isValid);
            m_CField->update_field_style();
            update_ok_button_state();
          });
  CHLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  CHLayout->addWidget(CLabel);
  CHLayout->addItem(new QSpacerItem(13, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  CHLayout->addWidget(m_CField);
  CHLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  CHLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for b line 1 box
  QHBoxLayout* b3HLayout = new QHBoxLayout();
  QLabel* b3Label = new QLabel();
  b3Label->setText("b3 *");
  m_b3Field = new UpdatableLabel();
  m_b3Field->setFixedWidth(100);
  //check to make sure user entered a real number. 
  m_b3Field->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_b3Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_b3Field->validator() &&
                                  m_b3Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_b3Field->set_isValid(isValid);
            m_b3Field->update_field_style();
            update_ok_button_state();
          });
  QLabel* b4Label = new QLabel();
  b4Label->setText("b4 *");
  m_b4Field = new UpdatableLabel();
  m_b4Field->setFixedWidth(100);
  //check to make sure user entered a real number. 
  m_b4Field->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_b4Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_b4Field->validator() &&
                                  m_b4Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_b4Field->set_isValid(isValid);
            m_b4Field->update_field_style();
            update_ok_button_state();
          });
  b3HLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  b3HLayout->addWidget(b3Label);
  b3HLayout->addItem(new QSpacerItem(8, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  b3HLayout->addWidget(m_b3Field);
  b3HLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  b3HLayout->addWidget(b4Label);
  b3HLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  b3HLayout->addWidget(m_b4Field);
  b3HLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for b line 2 box
  QHBoxLayout* b5HLayout = new QHBoxLayout();
  QLabel* b5Label = new QLabel();
  b5Label->setText("b5 *");
  m_b5Field = new UpdatableLabel();
  m_b5Field->setFixedWidth(100);
  //check to make sure user entered a real number. 
  m_b5Field->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_b5Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_b5Field->validator() &&
                                  m_b5Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_b5Field->set_isValid(isValid);
            m_b5Field->update_field_style();
            update_ok_button_state();
          });
  QLabel* b6Label = new QLabel();
  b6Label->setText("b6 *");
  m_b6Field = new UpdatableLabel();
  m_b6Field->setFixedWidth(100);
  //check to make sure user entered a real number. 
  m_b6Field->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_b6Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_b6Field->validator() &&
                                  m_b6Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_b6Field->set_isValid(isValid);
            m_b6Field->update_field_style();
            update_ok_button_state();
          });
  b5HLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  b5HLayout->addWidget(b5Label);
  b5HLayout->addItem(new QSpacerItem(8, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  b5HLayout->addWidget(m_b5Field);
  b5HLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  b5HLayout->addWidget(b6Label);
  b5HLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  b5HLayout->addWidget(m_b6Field);
  b5HLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for b line 3 box
  QHBoxLayout* b7HLayout = new QHBoxLayout();
  QLabel* b7Label = new QLabel();
  b7Label->setText("b7 *");
  m_b7Field = new UpdatableLabel();
  m_b7Field->setFixedWidth(100);
  //check to make sure user entered a real number. 
  m_b7Field->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_b7Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_b7Field->validator() &&
                                  m_b7Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_b7Field->set_isValid(isValid);
            m_b7Field->update_field_style();
            update_ok_button_state();
          });
  QLabel* b8Label = new QLabel();
  b8Label->setText("b8 *");
  m_b8Field = new UpdatableLabel();
  m_b8Field->setFixedWidth(100);
  //check to make sure user entered a real number. 
  m_b8Field->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_b8Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_b8Field->validator() &&
                                  m_b8Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_b8Field->set_isValid(isValid);
            m_b8Field->update_field_style();
            update_ok_button_state();
          });
  b7HLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  b7HLayout->addWidget(b7Label);
  b7HLayout->addItem(new QSpacerItem(8, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  b7HLayout->addWidget(m_b7Field);
  b7HLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  b7HLayout->addWidget(b8Label);
  b7HLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  b7HLayout->addWidget(m_b8Field);
  b7HLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for b line 4 box
  QHBoxLayout* b9HLayout = new QHBoxLayout();
  QLabel* b9Label = new QLabel();
  b9Label->setText("b9 *");
  m_b9Field = new UpdatableLabel();
  m_b9Field->setFixedWidth(100);
  //check to make sure user entered a real number. 
  m_b9Field->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_b9Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_b9Field->validator() &&
                                  m_b9Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_b9Field->set_isValid(isValid);
            m_b9Field->update_field_style();
            update_ok_button_state();
          });
  QLabel* b10Label = new QLabel();
  b10Label->setText("b10 *");
  m_b10Field = new UpdatableLabel();
  m_b10Field->setFixedWidth(100);
  //check to make sure user entered a real number. 
  m_b10Field->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_b10Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_b10Field->validator() &&
                                  m_b10Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_b10Field->set_isValid(isValid);
            m_b10Field->update_field_style();
            update_ok_button_state();
          });
  b9HLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  b9HLayout->addWidget(b9Label);
  b9HLayout->addItem(new QSpacerItem(8, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  b9HLayout->addWidget(m_b9Field);
  b9HLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  b9HLayout->addWidget(b10Label);
  b9HLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  b9HLayout->addWidget(m_b10Field);
  b9HLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for b line 5 box
  QHBoxLayout* b11HLayout = new QHBoxLayout();
  QLabel* b11Label = new QLabel();
  b11Label->setText("b11 *");
  m_b11Field = new UpdatableLabel();
  m_b11Field->setFixedWidth(100);
  //check to make sure user entered a real number. 
  m_b11Field->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_b11Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_b11Field->validator() &&
                                  m_b11Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_b11Field->set_isValid(isValid);
            m_b11Field->update_field_style();
            update_ok_button_state();
          });
  QLabel* b12Label = new QLabel();
  b12Label->setText("b12 *");
  m_b12Field = new UpdatableLabel();
  m_b12Field->setFixedWidth(100);
  //check to make sure user entered a real number. 
  m_b12Field->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_b12Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_b12Field->validator() &&
                                  m_b12Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_b12Field->set_isValid(isValid);
            m_b12Field->update_field_style();
            update_ok_button_state();
          });
  b11HLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  b11HLayout->addWidget(b11Label);
  b11HLayout->addItem(new QSpacerItem(2, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  b11HLayout->addWidget(m_b11Field);
  b11HLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  b11HLayout->addWidget(b12Label);
  b11HLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  b11HLayout->addWidget(m_b12Field);
  b11HLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for b line 6 box
  QHBoxLayout* b13HLayout = new QHBoxLayout();
  QLabel* b13Label = new QLabel();
  b13Label->setText("b13 *");
  m_b13Field = new UpdatableLabel();
  m_b13Field->setFixedWidth(100);
  //check to make sure user entered a real number. 
  m_b13Field->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_b13Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_b13Field->validator() &&
                                  m_b13Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_b13Field->set_isValid(isValid);
            m_b13Field->update_field_style();
            update_ok_button_state();
          });
  QLabel* b14Label = new QLabel();
  b14Label->setText("b14 *");
  m_b14Field = new UpdatableLabel();
  m_b14Field->setFixedWidth(100);
  //check to make sure user entered a real number. 
  m_b14Field->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_b14Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_b14Field->validator() &&
                                  m_b14Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_b14Field->set_isValid(isValid);
            m_b14Field->update_field_style();
            update_ok_button_state();
          });
  b13HLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  b13HLayout->addWidget(b13Label);
  b13HLayout->addItem(new QSpacerItem(2, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  b13HLayout->addWidget(m_b13Field);
  b13HLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  b13HLayout->addWidget(b14Label);
  b13HLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  b13HLayout->addWidget(m_b14Field);
  b13HLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Put the H layouts in main V layout
  XRFEmpiricalPgVLayout->addLayout(backScattBremTitleHLayout);
  XRFEmpiricalPgVLayout->addLayout(CHLayout);
  XRFEmpiricalPgVLayout->addLayout(b3HLayout);
  XRFEmpiricalPgVLayout->addLayout(b5HLayout);
  XRFEmpiricalPgVLayout->addLayout(b7HLayout);
  XRFEmpiricalPgVLayout->addLayout(b9HLayout);
  XRFEmpiricalPgVLayout->addLayout(b11HLayout);
  XRFEmpiricalPgVLayout->addLayout(b13HLayout);

  return XRFEmpiricalPage;
}

QWidget* NewParameterDialog::layout_detector_tab()
{
  QWidget* detectorPage = new QWidget(m_mainTabWidget);
  detectorPage->setObjectName("detectorPage");
  //Create the main vertical layout
  QVBoxLayout* detectorPgVLayout = new QVBoxLayout(detectorPage);
  detectorPgVLayout->setObjectName("detectorPgVLayout");

/*------------------------------------------------------------------------------
                               KED
 -----------------------------------------------------------------------------*/

  //Horizontal layout for the KED detector header
  QHBoxLayout* KEDHeaderHLayout = new QHBoxLayout();
  QLabel* KEDHeader = new QLabel;
  KEDHeader->setText("KED HPGe Detector");
  KEDHeader->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  KEDHeaderHLayout->addWidget(KEDHeader);
  KEDHeaderHLayout->setAlignment(KEDHeader, Qt::AlignCenter);

  //Create a horizontal line to separate KED Header and cal parts
  QFrame* KEDheaderCalLine = new QFrame();
  KEDheaderCalLine->setFrameShape(QFrame::HLine);
  KEDheaderCalLine->setStyleSheet("color: #006b75;");

  //Horizontal layout for the KED FWHM header
  QHBoxLayout* KEDFwhmHeaderHLayout = new QHBoxLayout();
  QLabel* KEDFwhmHeader = new QLabel;
  KEDFwhmHeader->setText("FWHM Calibration:");
  KEDFwhmHeader->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  QLabel* KEDfwhmEqn = new QLabel;
  KEDfwhmEqn->setText("(A<sub>1</sub> + A<sub>2</sub>*E + A<sub>3</sub>/E)<sup>0.5</sup>");
  KEDfwhmEqn->setStyleSheet("color: #005975; font-size: 13px;");
  KEDFwhmHeaderHLayout->addWidget(KEDFwhmHeader);
  KEDFwhmHeaderHLayout->addWidget(KEDfwhmEqn);

  //Horizontal layout for KED FWHM calibration box
  QHBoxLayout* KEDFwhmHLayout = new QHBoxLayout();
  QLabel* KEDA1Label = new QLabel();
  KEDA1Label->setText("A<sub>1</sub> *");
  m_KEDA1Field =  new UpdatableLabel();
  m_KEDA1Field->setMinimumWidth(20);
  QLabel* KEDA2Label = new QLabel();
  KEDA2Label->setText("A<sub>2</sub> *");
  m_KEDA2Field =  new UpdatableLabel();
  m_KEDA2Field->setMinimumWidth(20);
  QLabel* KEDA3Label = new QLabel();
  KEDA3Label->setText("A<sub>3</sub> *");
  m_KEDA3Field =  new UpdatableLabel();
  m_KEDA3Field->setMinimumWidth(20);
  KEDFwhmHLayout->addWidget(KEDA1Label);
  KEDFwhmHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  KEDFwhmHLayout->addWidget(m_KEDA1Field);
  KEDFwhmHLayout->addWidget(KEDA2Label);
  KEDFwhmHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  KEDFwhmHLayout->addWidget(m_KEDA2Field);
  KEDFwhmHLayout->addWidget(KEDA3Label);
  KEDFwhmHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  KEDFwhmHLayout->addWidget(m_KEDA3Field);
  KEDFwhmHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number.
  QDoubleValidator* validator = new QDoubleValidator(0.0, 1000.0, 4, this);
  validator->setNotation(QDoubleValidator::StandardNotation);
  //Set the locale to 'C' to ensure the dot (.) is always the decimal separator
  validator->setLocale(QLocale::C); 
  //Apply it to the line edit 
  m_KEDA1Field->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_KEDA1Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_KEDA1Field->validator() &&
                                  m_KEDA1Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_KEDA1Field->set_isValid(isValid);
            m_KEDA1Field->update_field_style();
            update_ok_button_state();
          });
  m_KEDA2Field->setValidator(validator);
  connect(m_KEDA2Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_KEDA2Field->validator() &&
                                  m_KEDA2Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_KEDA2Field->set_isValid(isValid);
            m_KEDA2Field->update_field_style();
            update_ok_button_state();
          });
  m_KEDA3Field->setValidator(validator);
  connect(m_KEDA3Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_KEDA3Field->validator() &&
                                  m_KEDA3Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_KEDA3Field->set_isValid(isValid);
            m_KEDA3Field->update_field_style();
            update_ok_button_state();
          });

  //Horizontal layout for the KED Tail header
  QHBoxLayout* KEDTailHeaderHLayout = new QHBoxLayout();
  QLabel* KEDtailHeader = new QLabel;
  KEDtailHeader->setText("Tailing Calibration:");
  KEDtailHeader->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  QLabel* KEDtailEqn = new QLabel;
  KEDtailEqn->setText("H*exp[(T<sub>1</sub> + T<sub>2</sub>*E) + (T<sub>3</sub> + T<sub>4</sub>*E)]*[1-exp(-a*(E<sub>i</sub>-E)<sup>2</sup>)]");
  KEDtailEqn->setStyleSheet("color: #005975; font-size: 13px;");
  KEDTailHeaderHLayout->addWidget(KEDtailHeader);
  KEDTailHeaderHLayout->addWidget(KEDtailEqn);

  //Horizontal layout for KED Tailing calibration box
  QHBoxLayout* KEDTailHLayout = new QHBoxLayout();
  QLabel* KEDT1Label = new QLabel();
  KEDT1Label->setText("T<sub>1</sub> *");
  m_KEDT1Field =  new UpdatableLabel();
  m_KEDT1Field->setMinimumWidth(20);
  QLabel* KEDT2Label = new QLabel();
  KEDT2Label->setText("T<sub>2</sub> *");
  m_KEDT2Field =  new UpdatableLabel();
  m_KEDT2Field->setMinimumWidth(20);
  QLabel* KEDT3Label = new QLabel();
  KEDT3Label->setText("T<sub>3</sub> *");
  m_KEDT3Field =  new UpdatableLabel();
  m_KEDT3Field->setMinimumWidth(20);
  QLabel* KEDT4Label = new QLabel();
  KEDT4Label->setText("T<sub>4</sub> *");
  m_KEDT4Field =  new UpdatableLabel();
  m_KEDT4Field->setMinimumWidth(20);
  KEDTailHLayout->addWidget(KEDT1Label);
  KEDTailHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  KEDTailHLayout->addWidget(m_KEDT1Field);
  KEDTailHLayout->addWidget(KEDT2Label);
  KEDTailHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  KEDTailHLayout->addWidget(m_KEDT2Field);
  KEDTailHLayout->addWidget(KEDT3Label);
  KEDTailHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  KEDTailHLayout->addWidget(m_KEDT3Field);
  KEDTailHLayout->addWidget(KEDT4Label);
  KEDTailHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  KEDTailHLayout->addWidget(m_KEDT4Field);
  KEDTailHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  m_KEDT1Field->setValidator(validator);
  connect(m_KEDT1Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_KEDT1Field->validator() &&
                                  m_KEDT1Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_KEDT1Field->set_isValid(isValid);
            m_KEDT1Field->update_field_style();
            update_ok_button_state();
          });
  m_KEDT2Field->setValidator(validator);
  connect(m_KEDT2Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_KEDT2Field->validator() &&
                                  m_KEDT2Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_KEDT2Field->set_isValid(isValid);
            m_KEDT2Field->update_field_style();
            update_ok_button_state();
          });
  m_KEDT3Field->setValidator(validator);
  connect(m_KEDT3Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_KEDT3Field->validator() &&
                                  m_KEDT3Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_KEDT3Field->set_isValid(isValid);
            m_KEDT3Field->update_field_style();
            update_ok_button_state();
          });
  m_KEDT4Field->setValidator(validator);
  connect(m_KEDT4Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_KEDT4Field->validator() &&
                                  m_KEDT4Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_KEDT4Field->set_isValid(isValid);
            m_KEDT4Field->update_field_style();
            update_ok_button_state();
          });

/*------------------------------------------------------------------------------
                               XRF
 -----------------------------------------------------------------------------*/
  //Horizontal layout for the XRF detector header
  QHBoxLayout* XRFHeaderHLayout = new QHBoxLayout();
  QLabel* XRFHeader = new QLabel;
  XRFHeader->setText("XRF HPGe Detector");
  XRFHeader->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  XRFHeaderHLayout->addWidget(XRFHeader);
  XRFHeaderHLayout->setAlignment(XRFHeader, Qt::AlignCenter);

  //Create a horizontal line to separate XRF Header and cal parts
  QFrame* XRFheaderCalLine = new QFrame();
  XRFheaderCalLine->setFrameShape(QFrame::HLine);
  XRFheaderCalLine->setStyleSheet("color: #006b75;");

  //Horizontal layout for the XRF FWHM header
  QHBoxLayout* XRFFwhmHeaderHLayout = new QHBoxLayout();
  QLabel* XRFFwhmHeader = new QLabel;
  XRFFwhmHeader->setText("FWHM Calibration:");
  XRFFwhmHeader->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  QLabel* XRFfwhmEqn = new QLabel;
  XRFfwhmEqn->setText("(A<sub>1</sub> + A<sub>2</sub>*E + A<sub>3</sub>/E)<sup>0.5</sup>");
  XRFfwhmEqn->setStyleSheet("color: #005975; font-size: 13px;");
  XRFFwhmHeaderHLayout->addWidget(XRFFwhmHeader);
  XRFFwhmHeaderHLayout->addWidget(XRFfwhmEqn);

  //Horizontal layout for XRF FWHM calibration box
  QHBoxLayout* XRFFwhmHLayout = new QHBoxLayout();
  QLabel* XRFA1Label = new QLabel();
  XRFA1Label->setText("A<sub>1</sub> *");
  m_XRFA1Field =  new UpdatableLabel();
  m_XRFA1Field->setMinimumWidth(20);
  QLabel* XRFA2Label = new QLabel();
  XRFA2Label->setText("A<sub>2</sub> *");
  m_XRFA2Field =  new UpdatableLabel();
  m_XRFA2Field->setMinimumWidth(20);
  QLabel* XRFA3Label = new QLabel();
  XRFA3Label->setText("A<sub>3</sub> *");
  m_XRFA3Field =  new UpdatableLabel();
  m_XRFA3Field->setMinimumWidth(20);
  XRFFwhmHLayout->addWidget(XRFA1Label);
  XRFFwhmHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  XRFFwhmHLayout->addWidget(m_XRFA1Field);
  XRFFwhmHLayout->addWidget(XRFA2Label);
  XRFFwhmHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  XRFFwhmHLayout->addWidget(m_XRFA2Field);
  XRFFwhmHLayout->addWidget(XRFA3Label);
  XRFFwhmHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  XRFFwhmHLayout->addWidget(m_XRFA3Field);
  XRFFwhmHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  m_XRFA1Field->setValidator(validator);
  connect(m_XRFA1Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_XRFA1Field->validator() &&
                                  m_XRFA1Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_XRFA1Field->set_isValid(isValid);
            m_XRFA1Field->update_field_style();
            update_ok_button_state();
          });
  m_XRFA2Field->setValidator(validator);
  connect(m_XRFA2Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_XRFA2Field->validator() &&
                                  m_XRFA2Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_XRFA2Field->set_isValid(isValid);
            m_XRFA2Field->update_field_style();
            update_ok_button_state();
          });
  m_XRFA3Field->setValidator(validator);
  connect(m_XRFA3Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_XRFA3Field->validator() &&
                                  m_XRFA3Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_XRFA3Field->set_isValid(isValid);
            m_XRFA3Field->update_field_style();
            update_ok_button_state();
          });

  //Horizontal layout for the XRF Tail header
  QHBoxLayout* XRFTailHeaderHLayout = new QHBoxLayout();
  QLabel* XRFtailHeader = new QLabel;
  XRFtailHeader->setText("Tailing Calibration:");
  XRFtailHeader->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  QLabel* XRFtailEqn = new QLabel;
  XRFtailEqn->setText("H*exp[(T<sub>1</sub> + T<sub>2</sub>*E) + (T<sub>3</sub> + T<sub>4</sub>*E)]*[1-exp(-a*(E<sub>i</sub>-E)<sup>2</sup>)]");
  XRFtailEqn->setStyleSheet("color: #005975; font-size: 13px;");
  XRFTailHeaderHLayout->addWidget(XRFtailHeader);
  XRFTailHeaderHLayout->addWidget(XRFtailEqn);

  //Horizontal layout for XRF Tailing calibration box
  QHBoxLayout* XRFTailHLayout = new QHBoxLayout();
  QLabel* XRFT1Label = new QLabel();
  XRFT1Label->setText("T<sub>1</sub> *");
  m_XRFT1Field =  new UpdatableLabel();
  m_XRFT1Field->setMinimumWidth(20);
  QLabel* XRFT2Label = new QLabel();
  XRFT2Label->setText("T<sub>2</sub> *");
  m_XRFT2Field =  new UpdatableLabel();
  m_XRFT2Field->setMinimumWidth(20);
  QLabel* XRFT3Label = new QLabel();
  XRFT3Label->setText("T<sub>3</sub> *");
  m_XRFT3Field =  new UpdatableLabel();
  m_XRFT3Field->setMinimumWidth(20);
  QLabel* XRFT4Label = new QLabel();
  XRFT4Label->setText("T<sub>4</sub> *");
  m_XRFT4Field =  new UpdatableLabel();
  m_XRFT4Field->setMinimumWidth(20);
  XRFTailHLayout->addWidget(XRFT1Label);
  XRFTailHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  XRFTailHLayout->addWidget(m_XRFT1Field);
  XRFTailHLayout->addWidget(XRFT2Label);
  XRFTailHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  XRFTailHLayout->addWidget(m_XRFT2Field);
  XRFTailHLayout->addWidget(XRFT3Label);
  XRFTailHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  XRFTailHLayout->addWidget(m_XRFT3Field);
  XRFTailHLayout->addWidget(XRFT4Label);
  XRFTailHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  XRFTailHLayout->addWidget(m_XRFT4Field);
  XRFTailHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  m_XRFT1Field->setValidator(validator);
  connect(m_XRFT1Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_XRFT1Field->validator() &&
                                  m_XRFT1Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_XRFT1Field->set_isValid(isValid);
            m_XRFT1Field->update_field_style();
            update_ok_button_state();
          });
  m_XRFT2Field->setValidator(validator);
  connect(m_XRFT2Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_XRFT2Field->validator() &&
                                  m_XRFT2Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_XRFT2Field->set_isValid(isValid);
            m_XRFT2Field->update_field_style();
            update_ok_button_state();
          });
  m_XRFT3Field->setValidator(validator);
  connect(m_XRFT3Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_XRFT3Field->validator() &&
                                  m_XRFT3Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_XRFT3Field->set_isValid(isValid);
            m_XRFT3Field->update_field_style();
            update_ok_button_state();
          });
  m_XRFT4Field->setValidator(validator);
  connect(m_XRFT4Field, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_XRFT4Field->validator() &&
                                  m_XRFT4Field->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_XRFT4Field->set_isValid(isValid);
            m_XRFT4Field->update_field_style();
            update_ok_button_state();
          });

  //Put the H layouts in main V layout
  //KED
  detectorPgVLayout->addLayout(KEDHeaderHLayout);
  detectorPgVLayout->addWidget(KEDheaderCalLine);
  detectorPgVLayout->addLayout(KEDFwhmHeaderHLayout);
  detectorPgVLayout->addLayout(KEDFwhmHLayout);
  detectorPgVLayout->addLayout(KEDTailHeaderHLayout);
  detectorPgVLayout->addLayout(KEDTailHLayout);
  //detectorPgVLayout->addWidget(headerCalLine);
  detectorPgVLayout->addItem(new QSpacerItem(1, 30, QSizePolicy::Minimum, QSizePolicy::Fixed));
  //XRF
  detectorPgVLayout->addLayout(XRFHeaderHLayout);
  detectorPgVLayout->addWidget(XRFheaderCalLine);
  detectorPgVLayout->addLayout(XRFFwhmHeaderHLayout);
  detectorPgVLayout->addLayout(XRFFwhmHLayout);
  detectorPgVLayout->addLayout(XRFTailHeaderHLayout);
  detectorPgVLayout->addLayout(XRFTailHLayout);

  return detectorPage;
}

QWidget* NewParameterDialog::layout_sample_tab()
{
  QWidget* samplePage = new QWidget(m_mainTabWidget);
  samplePage->setObjectName("samplePage");
  //Create the main vertical layout
  QVBoxLayout* samplePgVLayout = new QVBoxLayout(samplePage);
  samplePgVLayout->setObjectName("samplePgVLayout");

  //Horizontal layout for Vial ID box
  QHBoxLayout* vialIDHLayout = new QHBoxLayout();
  QLabel* vialIDLabel = new QLabel();
  vialIDLabel->setText("Sample Vial ID (mm) *");
  m_vialIDField =  new UpdatableLabel();
  m_vialIDField->setFixedWidth(100);
  vialIDHLayout->addWidget(vialIDLabel);
  vialIDHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  vialIDHLayout->addWidget(m_vialIDField);
  vialIDHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number. 
  QDoubleValidator* validator = new QDoubleValidator(0.0, 1000.0, 4, this);
  validator->setNotation(QDoubleValidator::StandardNotation);
  //Set the locale to 'C' to ensure the dot (.) is always the decimal separator
  validator->setLocale(QLocale::C); 
  //Apply it to the line edit
  m_vialIDField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_vialIDField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_vialIDField->validator() &&
                                  m_vialIDField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_vialIDField->set_isValid(isValid);
            m_vialIDField->update_field_style();
            update_ok_button_state();
          });

  //Horizontal layout for Vial OD box
  QHBoxLayout* vialODHLayout = new QHBoxLayout();
  QLabel* vialODLabel = new QLabel();
  vialODLabel->setText("Sample Vial OD (mm) *");
  m_vialODField =  new UpdatableLabel();
  m_vialODField->setFixedWidth(100);
  vialODHLayout->addWidget(vialODLabel);
  vialODHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  vialODHLayout->addWidget(m_vialODField);
  vialODHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number. 
  m_vialODField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_vialODField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_vialODField->validator() &&
                                  m_vialODField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_vialODField->set_isValid(isValid);
            m_vialODField->update_field_style();
            update_ok_button_state();
          });

  //Horizontal layout for Vial Material box
  QHBoxLayout* vialMaterialHLayout = new QHBoxLayout();
  QLabel* vialMaterialLabel = new QLabel();
  vialMaterialLabel->setText("Sample Vial Material *");
  m_vialMaterialComboBox = new QComboBox();
  m_vialMaterialComboBox->setMinimumWidth(70);
  m_vialMaterialComboBox->addItem("Quartz Glass");
  m_vialMaterialComboBox->addItem("HDPE");
  vialMaterialHLayout->addWidget(vialMaterialLabel);
  vialMaterialHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  vialMaterialHLayout->addWidget(m_vialMaterialComboBox);
  vialMaterialHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for Matrix Material box
  QHBoxLayout* matrixMaterialHLayout = new QHBoxLayout();
  QLabel* matrixMaterialLabel = new QLabel();
  matrixMaterialLabel->setText("Sample Matrix Material *");
  m_matrixMaterialComboBox = new QComboBox();
  m_matrixMaterialComboBox->setMinimumWidth(70);
  m_matrixMaterialComboBox->addItem("Nitric Acid");
  m_matrixMaterialComboBox->addItem("Other");
  matrixMaterialHLayout->addWidget(matrixMaterialLabel);
  matrixMaterialHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  matrixMaterialHLayout->addWidget(m_matrixMaterialComboBox);
  matrixMaterialHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Put the H layouts in main V layout
  samplePgVLayout->addLayout(vialIDHLayout);
  samplePgVLayout->addLayout(vialODHLayout);
  samplePgVLayout->addLayout(vialMaterialHLayout);
  samplePgVLayout->addLayout(matrixMaterialHLayout);
  samplePgVLayout->addItem(new QSpacerItem(1, 120, QSizePolicy::Minimum, QSizePolicy::Fixed));

  return samplePage;
}
QWidget* NewParameterDialog::layout_roi_tab()
{
  QWidget* roiPage = new QWidget();
  QVBoxLayout* roiPageVLayout = new QVBoxLayout(roiPage);
  QTabWidget* roiTabWidget = new QTabWidget();
  roiTabWidget->addTab(layout_roi_KED_tab(), "KED ROIs");
  roiTabWidget->addTab(layout_roi_XRF_tab(), "XRF ROIs");
  roiPageVLayout->addWidget(roiTabWidget);
  return roiPage;
}

QWidget* NewParameterDialog::layout_roi_KED_tab()
{
  QWidget* kedFittingPage = new QWidget(m_mainTabWidget);
  kedFittingPage->setObjectName("kedFittingPage");
  //Create the main vertical layout
  QVBoxLayout* kedFittingPgVLayout = new QVBoxLayout(kedFittingPage);
  kedFittingPgVLayout->setObjectName("kedFittingPgVLayout");

  //Horizontal layout for KED lower fit bound box
  QHBoxLayout* KEDLowerBoundHLayout = new QHBoxLayout();
  QLabel* KEDLowerBoundLabel = new QLabel();
  KEDLowerBoundLabel->setText("KED Fit ROI Lower Bound (keV) *");
  m_KEDLowerBoundField = new UpdatableLabel();
  m_KEDLowerBoundField->setFixedWidth(100);
  KEDLowerBoundHLayout->addWidget(KEDLowerBoundLabel);
  KEDLowerBoundHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  KEDLowerBoundHLayout->addWidget(m_KEDLowerBoundField);
  KEDLowerBoundHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number.
  QDoubleValidator* validator = new QDoubleValidator(0.0, 1000.0, 4, this);
  validator->setNotation(QDoubleValidator::StandardNotation);
  //Set the locale to 'C' to ensure the dot (.) is always the decimal separator
  validator->setLocale(QLocale::C); 
  //Apply it to the line edit
  m_KEDLowerBoundField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_KEDLowerBoundField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_KEDLowerBoundField->validator() &&
                                  m_KEDLowerBoundField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_KEDLowerBoundField->set_isValid(isValid);
            m_KEDLowerBoundField->update_field_style();
            update_ok_button_state();
          });

  //Horizontal layout for KED upper fit bound box
  QHBoxLayout* KEDUpperBoundHLayout = new QHBoxLayout();
  QLabel* KEDUpperBoundLabel = new QLabel();
  KEDUpperBoundLabel->setText("KED Fit ROI Upper Bound (keV) *");
  m_KEDUpperBoundField = new UpdatableLabel();
  m_KEDUpperBoundField->setFixedWidth(100);
  KEDUpperBoundHLayout->addWidget(KEDUpperBoundLabel);
  KEDUpperBoundHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  KEDUpperBoundHLayout->addWidget(m_KEDUpperBoundField);
  KEDUpperBoundHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number. 
  m_KEDUpperBoundField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_KEDUpperBoundField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_KEDUpperBoundField->validator() &&
                                  m_KEDUpperBoundField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_KEDUpperBoundField->set_isValid(isValid);
            m_KEDUpperBoundField->update_field_style();
            update_ok_button_state();
          });

  //Horizontal layout for KED bkgd low ROI lower fit bound box
  QHBoxLayout* KEDBkgdLowLowBoundHLayout = new QHBoxLayout();
  QLabel* KEDBkgdLowLowBoundLabel = new QLabel();
  KEDBkgdLowLowBoundLabel->setText("KED Fit Bkgd Low ROI Lower Bound (keV) *");
  m_KEDBkgdLowLowBoundField = new UpdatableLabel();
  m_KEDBkgdLowLowBoundField->setFixedWidth(100);
  KEDBkgdLowLowBoundHLayout->addWidget(KEDBkgdLowLowBoundLabel);
  KEDBkgdLowLowBoundHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  KEDBkgdLowLowBoundHLayout->addWidget(m_KEDBkgdLowLowBoundField);
  KEDBkgdLowLowBoundHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number. 
  m_KEDBkgdLowLowBoundField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_KEDBkgdLowLowBoundField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_KEDBkgdLowLowBoundField->validator() &&
                                  m_KEDBkgdLowLowBoundField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_KEDBkgdLowLowBoundField->set_isValid(isValid);
            m_KEDBkgdLowLowBoundField->update_field_style();
            update_ok_button_state();
          });

  //Horizontal layout for KED bkgd low ROI upper fit bound box
  QHBoxLayout* KEDBkgdLowHighBoundHLayout = new QHBoxLayout();
  QLabel* KEDBkgdLowHighBoundLabel = new QLabel();
  KEDBkgdLowHighBoundLabel->setText("KED Fit Bkgd Low ROI Upper Bound (keV) *");
  m_KEDBkgdLowHighBoundField = new UpdatableLabel();
  m_KEDBkgdLowHighBoundField->setFixedWidth(100);
  KEDBkgdLowHighBoundHLayout->addWidget(KEDBkgdLowHighBoundLabel);
  KEDBkgdLowHighBoundHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  KEDBkgdLowHighBoundHLayout->addWidget(m_KEDBkgdLowHighBoundField);
  KEDBkgdLowHighBoundHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number. 
  m_KEDBkgdLowHighBoundField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_KEDBkgdLowHighBoundField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_KEDBkgdLowHighBoundField->validator() &&
                                  m_KEDBkgdLowHighBoundField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_KEDBkgdLowHighBoundField->set_isValid(isValid);
            m_KEDBkgdLowHighBoundField->update_field_style();
            update_ok_button_state();
          });

  //Horizontal layout for KED bkgd high ROI lower fit bound box
  QHBoxLayout* KEDBkgdHighLowBoundHLayout = new QHBoxLayout();
  QLabel* KEDBkgdHighLowBoundLabel = new QLabel();
  KEDBkgdHighLowBoundLabel->setText("KED Fit Bkgd High ROI Lower Bound (keV) *");
  m_KEDBkgdHighLowBoundField = new UpdatableLabel();
  m_KEDBkgdHighLowBoundField->setFixedWidth(100);
  KEDBkgdHighLowBoundHLayout->addWidget(KEDBkgdHighLowBoundLabel);
  KEDBkgdHighLowBoundHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  KEDBkgdHighLowBoundHLayout->addWidget(m_KEDBkgdHighLowBoundField);
  KEDBkgdHighLowBoundHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number. 
  m_KEDBkgdHighLowBoundField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_KEDBkgdHighLowBoundField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_KEDBkgdHighLowBoundField->validator() &&
                                  m_KEDBkgdHighLowBoundField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_KEDBkgdHighLowBoundField->set_isValid(isValid);
            m_KEDBkgdHighLowBoundField->update_field_style();
            update_ok_button_state();
          });

  //Horizontal layout for KED bkgd high ROI upper fit bound box
  QHBoxLayout* KEDBkgdHighHighBoundHLayout = new QHBoxLayout();
  QLabel* KEDBkgdHighHighBoundLabel = new QLabel();
  KEDBkgdHighHighBoundLabel->setText("KED Fit Bkgd High ROI Upper Bound (keV) *");
  m_KEDBkgdHighHighBoundField = new UpdatableLabel();
  m_KEDBkgdHighHighBoundField->setFixedWidth(100);
  KEDBkgdHighHighBoundHLayout->addWidget(KEDBkgdHighHighBoundLabel);
  KEDBkgdHighHighBoundHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  KEDBkgdHighHighBoundHLayout->addWidget(m_KEDBkgdHighHighBoundField);
  KEDBkgdHighHighBoundHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number. 
  m_KEDBkgdHighHighBoundField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_KEDBkgdHighHighBoundField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_KEDBkgdHighHighBoundField->validator() &&
                                  m_KEDBkgdHighHighBoundField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_KEDBkgdHighHighBoundField->set_isValid(isValid);
            m_KEDBkgdHighHighBoundField->update_field_style();
            update_ok_button_state();
          });

  //Put the H layouts in main V layout
  kedFittingPgVLayout->addLayout(KEDLowerBoundHLayout);
  kedFittingPgVLayout->addLayout(KEDUpperBoundHLayout);
  kedFittingPgVLayout->addLayout(KEDBkgdLowLowBoundHLayout);
  kedFittingPgVLayout->addLayout(KEDBkgdLowHighBoundHLayout);
  kedFittingPgVLayout->addLayout(KEDBkgdHighLowBoundHLayout);
  kedFittingPgVLayout->addLayout(KEDBkgdHighHighBoundHLayout);

  return kedFittingPage;
}

QWidget* NewParameterDialog::layout_roi_XRF_tab()
{
  QWidget* XRFFittingPage = new QWidget(m_mainTabWidget);
  XRFFittingPage->setObjectName("XRFFittingPage");
  //Create the main vertical layout
  QVBoxLayout* XRFFittingPgVLayout = new QVBoxLayout(XRFFittingPage);
  XRFFittingPgVLayout->setObjectName("XRFFittingPgVLayout");

  //Horizontal layout for XRF lower fit bound box
  QHBoxLayout* XRFLowerBoundHLayout = new QHBoxLayout();
  QLabel* XRFLowerBoundLabel = new QLabel();
  XRFLowerBoundLabel->setText("XRF Fit ROI Lower Bound (keV) *");
  m_XRFLowerBoundField = new UpdatableLabel();
  m_XRFLowerBoundField->setFixedWidth(100);
  XRFLowerBoundHLayout->addWidget(XRFLowerBoundLabel);
  XRFLowerBoundHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  XRFLowerBoundHLayout->addWidget(m_XRFLowerBoundField);
  XRFLowerBoundHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number.
  QDoubleValidator* validator = new QDoubleValidator(0.0, 1000.0, 4, this);
  validator->setNotation(QDoubleValidator::StandardNotation);
  //Set the locale to 'C' to ensure the dot (.) is always the decimal separator
  validator->setLocale(QLocale::C); 
  //Apply it to the line edit
  m_XRFLowerBoundField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_XRFLowerBoundField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_XRFLowerBoundField->validator() &&
                                  m_XRFLowerBoundField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_XRFLowerBoundField->set_isValid(isValid);
            m_XRFLowerBoundField->update_field_style();
            update_ok_button_state();
          });

  //Horizontal layout for XRF upper fit bound box
  QHBoxLayout* XRFUpperBoundHLayout = new QHBoxLayout();
  QLabel* XRFUpperBoundLabel = new QLabel();
  XRFUpperBoundLabel->setText("XRF Fit ROI Upper Bound (keV) *");
  m_XRFUpperBoundField = new UpdatableLabel();
  m_XRFUpperBoundField->setFixedWidth(100);
  XRFUpperBoundHLayout->addWidget(XRFUpperBoundLabel);
  XRFUpperBoundHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  XRFUpperBoundHLayout->addWidget(m_XRFUpperBoundField);
  XRFUpperBoundHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number. 
  m_XRFUpperBoundField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_XRFUpperBoundField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_XRFUpperBoundField->validator() &&
                                  m_XRFUpperBoundField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_XRFUpperBoundField->set_isValid(isValid);
            m_XRFUpperBoundField->update_field_style();
            update_ok_button_state();
          });

  //Horizontal layout for XRF bkgd low ROI lower fit bound box
  QHBoxLayout* XRFBkgdLowLowBoundHLayout = new QHBoxLayout();
  QLabel* XRFBkgdLowLowBoundLabel = new QLabel();
  XRFBkgdLowLowBoundLabel->setText("XRF Fit Bkgd Low ROI Lower Bound (keV) *");
  m_XRFBkgdLowLowBoundField = new UpdatableLabel();
  m_XRFBkgdLowLowBoundField->setFixedWidth(100);
  XRFBkgdLowLowBoundHLayout->addWidget(XRFBkgdLowLowBoundLabel);
  XRFBkgdLowLowBoundHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  XRFBkgdLowLowBoundHLayout->addWidget(m_XRFBkgdLowLowBoundField);
  XRFBkgdLowLowBoundHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number. 
  m_XRFBkgdLowLowBoundField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_XRFBkgdLowLowBoundField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_XRFBkgdLowLowBoundField->validator() &&
                                  m_XRFBkgdLowLowBoundField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_XRFBkgdLowLowBoundField->set_isValid(isValid);
            m_XRFBkgdLowLowBoundField->update_field_style();
            update_ok_button_state();
          });

  //Horizontal layout for XRF bkgd low ROI upper fit bound box
  QHBoxLayout* XRFBkgdLowHighBoundHLayout = new QHBoxLayout();
  QLabel* XRFBkgdLowHighBoundLabel = new QLabel();
  XRFBkgdLowHighBoundLabel->setText("XRF Fit Bkgd Low ROI Upper Bound (keV) *");
  m_XRFBkgdLowHighBoundField = new UpdatableLabel();
  m_XRFBkgdLowHighBoundField->setFixedWidth(100);
  XRFBkgdLowHighBoundHLayout->addWidget(XRFBkgdLowHighBoundLabel);
  XRFBkgdLowHighBoundHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  XRFBkgdLowHighBoundHLayout->addWidget(m_XRFBkgdLowHighBoundField);
  XRFBkgdLowHighBoundHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number. 
  m_XRFBkgdLowHighBoundField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_XRFBkgdLowHighBoundField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_XRFBkgdLowHighBoundField->validator() &&
                                  m_XRFBkgdLowHighBoundField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_XRFBkgdLowHighBoundField->set_isValid(isValid);
            m_XRFBkgdLowHighBoundField->update_field_style();
            update_ok_button_state();
          });

  //Horizontal layout for XRF bkgd high ROI lower fit bound box
  QHBoxLayout* XRFBkgdHighLowBoundHLayout = new QHBoxLayout();
  QLabel* XRFBkgdHighLowBoundLabel = new QLabel();
  XRFBkgdHighLowBoundLabel->setText("XRF Fit Bkgd High ROI Lower Bound (keV) *");
  m_XRFBkgdHighLowBoundField = new UpdatableLabel();
  m_XRFBkgdHighLowBoundField->setFixedWidth(100);
  XRFBkgdHighLowBoundHLayout->addWidget(XRFBkgdHighLowBoundLabel);
  XRFBkgdHighLowBoundHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  XRFBkgdHighLowBoundHLayout->addWidget(m_XRFBkgdHighLowBoundField);
  XRFBkgdHighLowBoundHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number. 
  m_XRFBkgdHighLowBoundField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_XRFBkgdHighLowBoundField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_XRFBkgdHighLowBoundField->validator() &&
                                  m_XRFBkgdHighLowBoundField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_XRFBkgdHighLowBoundField->set_isValid(isValid);
            m_XRFBkgdHighLowBoundField->update_field_style();
            update_ok_button_state();
          });

  //Horizontal layout for XRF bkgd high ROI upper fit bound box
  QHBoxLayout* XRFBkgdHighHighBoundHLayout = new QHBoxLayout();
  QLabel* XRFBkgdHighHighBoundLabel = new QLabel();
  XRFBkgdHighHighBoundLabel->setText("XRF Fit Bkgd High ROI Upper Bound (keV) *");
  m_XRFBkgdHighHighBoundField = new UpdatableLabel();
  m_XRFBkgdHighHighBoundField->setFixedWidth(100);
  XRFBkgdHighHighBoundHLayout->addWidget(XRFBkgdHighHighBoundLabel);
  XRFBkgdHighHighBoundHLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  XRFBkgdHighHighBoundHLayout->addWidget(m_XRFBkgdHighHighBoundField);
  XRFBkgdHighHighBoundHLayout->addItem(new QSpacerItem(60, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  //check to make sure user entered a real number. 
  m_XRFBkgdHighHighBoundField->setValidator(validator);
  //Connect  field for live validation and button-state updates
  connect(m_XRFBkgdHighHighBoundField, &QLineEdit::textChanged,
          this, [this](const QString& text)
          {
            QString copy = text;
            int pos = 0;
            const bool isValid = !copy.isEmpty() &&
                                  m_XRFBkgdHighHighBoundField->validator() &&
                                  m_XRFBkgdHighHighBoundField->validator()->validate(copy, pos) == QValidator::Acceptable;
            m_XRFBkgdHighHighBoundField->set_isValid(isValid);
            m_XRFBkgdHighHighBoundField->update_field_style();
            update_ok_button_state();
          });

  //Put the H layouts in main V layout
  XRFFittingPgVLayout->addLayout(XRFLowerBoundHLayout);
  XRFFittingPgVLayout->addLayout(XRFUpperBoundHLayout);
  XRFFittingPgVLayout->addLayout(XRFBkgdLowLowBoundHLayout);
  XRFFittingPgVLayout->addLayout(XRFBkgdLowHighBoundHLayout);
  XRFFittingPgVLayout->addLayout(XRFBkgdHighLowBoundHLayout);
  XRFFittingPgVLayout->addLayout(XRFBkgdHighHighBoundHLayout);

  return XRFFittingPage;
}

void NewParameterDialog::setup_for_new()
{
  this->setWindowTitle("New Parameter Set");
  if(m_paramNameField != nullptr)
  {
    m_paramNameField->setText("");
    m_paramNameField->set_isValid(false);//false so user has to enter a name
    m_paramNameField->setFocus();
  } 
  if(m_authorNameField != nullptr)
  {
    m_authorNameField->setText("");
    m_authorNameField->set_isValid(true);
  } 
  if(m_facilityNameField != nullptr)
  {
    m_facilityNameField->setText("");
    m_facilityNameField->set_isValid(true);
  } 
  if(m_commentField != nullptr)
  {
    m_commentField->setText("");
    m_commentField->set_isValid(true);
  } 
  if(m_hvField != nullptr)
  {
    m_hvField->setText("150");
    m_hvField->set_isValid(true);
  } 
  if(m_incidenceField != nullptr)
  {
    m_incidenceField->setText("19.49");
    m_incidenceField->set_isValid(true);
  } 
  if(m_takeoffField != nullptr)
  {
    m_takeoffField->setText("62.93");
    m_takeoffField->set_isValid(true);
  } 
  if(m_backscatterField != nullptr)
  {
    m_backscatterField->setText("114.0");
    m_backscatterField->set_isValid(true);
  } 
  if(m_beWindowField != nullptr)
  {
    m_beWindowField->setText("0.3");
    m_beWindowField->set_isValid(true);
  } 
  if(m_cdFilterField != nullptr)
  {
    m_cdFilterField->setText("0.1");
    m_cdFilterField->set_isValid(true);
  } 
  if(m_feFilterField != nullptr)
  {
    m_feFilterField->setText("2.4");
    m_feFilterField->set_isValid(true);
  } 
  if(m_xfrEntranceField != nullptr)
  {
    m_xfrEntranceField->setText("0.4");
    m_xfrEntranceField->set_isValid(true);
  } 
  if(m_xfrExitField != nullptr)
  {
    m_xfrExitField->setText("0.4");
    m_xfrExitField->set_isValid(true);
  } 
  if(m_KEDA1Field != nullptr)
  {
    m_KEDA1Field->setText("0.06");
    m_KEDA1Field->set_isValid(true);
  } 
  if(m_KEDA2Field != nullptr)
  {
    m_KEDA2Field->setText("0.002");
    m_KEDA2Field->set_isValid(true);
  } 
  if(m_KEDA3Field != nullptr)
  {
    m_KEDA3Field->setText("0.0");
    m_KEDA3Field->set_isValid(true);
  } 
  if(m_KEDT1Field != nullptr)
  {
    m_KEDT1Field->setText("0.06");
    m_KEDT1Field->set_isValid(true);
  } 
  if(m_KEDT2Field != nullptr)
  {
    m_KEDT2Field->setText("0.002");
    m_KEDT2Field->set_isValid(true);
  } 
  if(m_KEDT3Field != nullptr)
  {
    m_KEDT3Field->setText("0.0");
    m_KEDT3Field->set_isValid(true);
  } 
  if(m_KEDT4Field != nullptr)
  {
    m_KEDT4Field->setText("0.0");
    m_KEDT4Field->set_isValid(true);
  } 
  if(m_XRFA1Field != nullptr)
  {
    m_XRFA1Field->setText("0.06");
    m_XRFA1Field->set_isValid(true);
  } 
  if(m_XRFA2Field != nullptr)
  {
    m_XRFA2Field->setText("0.002");
    m_XRFA2Field->set_isValid(true);
  } 
  if(m_XRFA3Field != nullptr)
  {
    m_XRFA3Field->setText("0.0");
    m_XRFA3Field->set_isValid(true);
  } 
  if(m_XRFT1Field != nullptr)
  {
    m_XRFT1Field->setText("0.06");
    m_XRFT1Field->set_isValid(true);
  } 
  if(m_XRFT2Field != nullptr)
  {
    m_XRFT2Field->setText("0.002");
    m_XRFT2Field->set_isValid(true);
  } 
  if(m_XRFT3Field != nullptr)
  {
    m_XRFT3Field->setText("0.0");
    m_XRFT3Field->set_isValid(true);
  } 
  if(m_XRFT4Field != nullptr)
  {
    m_XRFT4Field->setText("0.0");
    m_XRFT4Field->set_isValid(true);
  } 
  if(m_vialIDField != nullptr)
  {
    m_vialIDField->setText("14.18");
   m_vialIDField->set_isValid(true);
  } 
  if(m_vialODField != nullptr)
  {
    m_vialODField->setText("16.18");
    m_vialODField->set_isValid(true);
  } 
  if(m_vialMaterialComboBox != nullptr)
  {
    m_vialMaterialComboBox->setCurrentIndex(0);
  } 
  if(m_matrixMaterialComboBox != nullptr)
  {
    m_matrixMaterialComboBox->setCurrentIndex(0);
  } 
  if(m_KEDLowerBoundField != nullptr)
  {
    m_KEDLowerBoundField->setText("87");
    m_KEDLowerBoundField->set_isValid(true);
  } 
  if(m_KEDUpperBoundField != nullptr)
  {
    m_KEDUpperBoundField->setText("147");
    m_KEDUpperBoundField->set_isValid(true);
  } 
  if(m_KEDBkgdLowLowBoundField != nullptr)
  {
    m_KEDBkgdLowLowBoundField->setText("61.57");
    m_KEDBkgdLowLowBoundField->set_isValid(true);
  } 
  if(m_KEDBkgdLowHighBoundField != nullptr)
  {
    m_KEDBkgdLowHighBoundField->setText("64.54");
    m_KEDBkgdLowHighBoundField->set_isValid(true);
  } 
  if(m_KEDBkgdHighLowBoundField != nullptr)
  {
    m_KEDBkgdHighLowBoundField->setText("170.0");
    m_KEDBkgdHighLowBoundField->set_isValid(true);
  } 
  if(m_KEDBkgdHighHighBoundField != nullptr)
  {
    m_KEDBkgdHighHighBoundField->setText("176.4");
    m_KEDBkgdHighHighBoundField->set_isValid(true);
  } 
  if(m_XRFLowerBoundField != nullptr)
  {
    m_XRFLowerBoundField->setText("87");
    m_XRFLowerBoundField->set_isValid(true);
  } 
  if(m_XRFUpperBoundField != nullptr)
  {
    m_XRFUpperBoundField->setText("147");
    m_XRFUpperBoundField->set_isValid(true);
  } 
  if(m_XRFBkgdLowLowBoundField != nullptr)
  {
    m_XRFBkgdLowLowBoundField->setText("61.57");
    m_XRFBkgdLowLowBoundField->set_isValid(true);
  } 
  if(m_XRFBkgdLowHighBoundField != nullptr)
  {
    m_XRFBkgdLowHighBoundField->setText("64.54");
    m_XRFBkgdLowHighBoundField->set_isValid(true);
  } 
  if(m_XRFBkgdHighLowBoundField != nullptr)
  {
    m_XRFBkgdHighLowBoundField->setText("170.0");
    m_XRFBkgdHighLowBoundField->set_isValid(true);
  } 
  if(m_XRFBkgdHighHighBoundField != nullptr)
  {
    m_XRFBkgdHighHighBoundField->setText("176.4");
    m_XRFBkgdHighHighBoundField->set_isValid(true);
  } 
  if(m_KEDCdCorrField != nullptr)
  {
    m_KEDCdCorrField->setText("0.0006");
    m_KEDCdCorrField->set_isValid(true);
  } 
  if(m_KEDBkgdNormField != nullptr)
  {
    m_KEDBkgdNormField->setText("0.75");
    m_KEDBkgdNormField->set_isValid(true);
  } 
  if(m_KEDTempField != nullptr)
  {
    m_KEDTempField->setText("0.0012");
    m_KEDTempField->set_isValid(true);
  } 
  if(m_CField != nullptr)
  {
    m_CField->setText("100.0");
    m_CField->set_isValid(true);
  } 
  if(m_b3Field != nullptr)
  {
    m_b3Field->setText("4.312");
    m_b3Field->set_isValid(true);
  } 
    if(m_b4Field != nullptr)
  {
    m_b4Field->setText("348.879");
    m_b4Field->set_isValid(true);
  } 
  if(m_b5Field != nullptr)
  {
    m_b5Field->setText("-27.491");
    m_b5Field->set_isValid(true);
  } 
  if(m_b6Field != nullptr)
  {
    m_b6Field->setText("275.427");
    m_b6Field->set_isValid(true);
  } 
  if(m_b7Field != nullptr)
  {
    m_b7Field->setText("-28.125");
    m_b7Field->set_isValid(true);
  } 
  if(m_b8Field != nullptr)
  {
    m_b8Field->setText("1419.462");
    m_b8Field->set_isValid(true);
  } 
  if(m_b9Field != nullptr)
  {
    m_b9Field->setText("111.853");
    m_b9Field->set_isValid(true);
  } 
  if(m_b10Field != nullptr)
  {
    m_b10Field->setText("644.646");
    m_b10Field->set_isValid(true);
  } 
  if(m_b11Field != nullptr)
  {
    m_b11Field->setText("2352.294");
    m_b11Field->set_isValid(true);
  } 
  if(m_b12Field != nullptr)
  {
    m_b12Field->setText("2725.173");
    m_b12Field->set_isValid(true);
  } 
  if(m_b13Field != nullptr)
  {
    m_b13Field->setText("3448.158");
    m_b13Field->set_isValid(true);
  } 
  if(m_b14Field != nullptr)
  {
    m_b14Field->setText("11.291");
    m_b14Field->set_isValid(true);
  } 
}

void NewParameterDialog::setup_for_edit()
{
  this->setWindowTitle("Edit Parameter Set");
  if(m_paramNameField != nullptr)
  {
    m_loadedParamSetName = QString::fromStdString(KFileManager::getInstance().m_workingParameterSet.m_setName);
    m_paramNameField->setText(m_loadedParamSetName);
    bool isGood = check_valid_string(m_loadedParamSetName);
    m_paramNameField->set_isValid(isGood);
  } 
  if(m_authorNameField != nullptr)
  {
    m_authorNameField->setText(QString::fromStdString(KFileManager::getInstance().m_workingParameterSet.m_authorName));
    m_authorNameField->set_isValid(true);//not needed but initialize anyway
  } 
  if(m_facilityNameField != nullptr)
  {
    m_facilityNameField->setText(QString::fromStdString(KFileManager::getInstance().m_workingParameterSet.m_facilityName));
    m_facilityNameField->set_isValid(true);//not needed but initialize anyway
  } 

  //Add date line

  if(m_commentField != nullptr)
  {
    m_commentField->setText(QString::fromStdString(KFileManager::getInstance().m_workingParameterSet.m_comment));
    m_commentField->set_isValid(true);//not needed but initialize anyway
  } 
  if(m_hvField != nullptr)
  {
    m_hvField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_generatorHVSetpoint, 'f', 4));
    m_hvField->set_isValid(true);
  } 
  if(m_incidenceField != nullptr)
  {
    m_incidenceField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_beamIncidenceAngle, 'f', 4));
    m_incidenceField->set_isValid(true);
  } 
  if(m_takeoffField != nullptr)
  {
    m_takeoffField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_takeoffAngle, 'f', 4));
    m_takeoffField->set_isValid(true);
  } 
  if(m_backscatterField != nullptr)
  {
    m_backscatterField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_minBackscatterAngle, 'f', 4));
    m_backscatterField->set_isValid(true);
  } 
  if(m_beWindowField != nullptr)
  {
    m_beWindowField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_beWindowThickness, 'f', 4));
    m_beWindowField->set_isValid(true);
  } 
  if(m_cdFilterField != nullptr)
  {
    m_cdFilterField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_cdFilterThickness, 'f', 4));
    m_cdFilterField->set_isValid(true);
  } 
  if(m_feFilterField != nullptr)
  {
    m_feFilterField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_feFilterThickness, 'f', 4));
    m_feFilterField->set_isValid(true);
  } 
  if(m_xfrEntranceField != nullptr)
  {
    m_xfrEntranceField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_xfrEntraceThickness, 'f', 4));
    m_xfrEntranceField->set_isValid(true);
  } 
  if(m_xfrExitField != nullptr)
  {
    m_xfrExitField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_xfrExitThickness, 'f', 4));
    m_xfrExitField->set_isValid(true);
  } 
  if(m_KEDA1Field != nullptr)
  {
    m_KEDA1Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_KEDA1, 'f', 4));
    m_KEDA1Field->set_isValid(true);
  } 
  if(m_KEDA2Field != nullptr)
  {
    m_KEDA2Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_KEDA2, 'f', 4));
    m_KEDA2Field->set_isValid(true);
  } 
  if(m_KEDA3Field != nullptr)
  {
    m_KEDA3Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_KEDA3, 'f', 4));
    m_KEDA3Field->set_isValid(true);
  } 
  if(m_KEDT1Field != nullptr)
  {
    m_KEDT1Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_KEDT1, 'f', 4));
    m_KEDT1Field->set_isValid(true);
  } 
  if(m_KEDT2Field != nullptr)
  {
    m_KEDT2Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_KEDT2, 'f', 4));
    m_KEDT2Field->set_isValid(true);
  } 
  if(m_KEDT3Field != nullptr)
  {
    m_KEDT3Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_KEDT3, 'f', 4));
    m_KEDT3Field->set_isValid(true);
  } 
  if(m_KEDT4Field != nullptr)
  {
    m_KEDT4Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_KEDT4, 'f', 4));
    m_KEDT4Field->set_isValid(true);
  } 
  if(m_XRFA1Field != nullptr)
  {
    m_XRFA1Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_XRFA1, 'f', 4));
    m_XRFA1Field->set_isValid(true);
  } 
  if(m_XRFA2Field != nullptr)
  {
    m_XRFA2Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_XRFA2, 'f', 4));
    m_XRFA2Field->set_isValid(true);
  } 
  if(m_XRFA3Field != nullptr)
  {
    m_XRFA3Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_XRFA3, 'f', 4));
    m_XRFA3Field->set_isValid(true);
  } 
  if(m_XRFT1Field != nullptr)
  {
    m_XRFT1Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_XRFT1, 'f', 4));
    m_XRFT1Field->set_isValid(true);
  } 
  if(m_XRFT2Field != nullptr)
  {
    m_XRFT2Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_XRFT2, 'f', 4));
    m_XRFT2Field->set_isValid(true);
  } 
  if(m_XRFT3Field != nullptr)
  {
    m_XRFT3Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_XRFT3, 'f', 4));
    m_XRFT3Field->set_isValid(true);
  } 
  if(m_XRFT4Field != nullptr)
  {
    m_XRFT4Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_XRFT4, 'f', 4));
    m_XRFT4Field->set_isValid(true);
  } 
  if(m_vialIDField != nullptr)
  {
    m_vialIDField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_vialID, 'f', 4));
   m_vialIDField->set_isValid(true);
  } 
  if(m_vialODField != nullptr)
  {
    m_vialODField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_vialOD, 'f', 4));
    m_vialODField->set_isValid(true);
  } 
  if(m_vialMaterialComboBox != nullptr)
  {
    m_vialMaterialComboBox->setCurrentIndex(0);
  } 
  if(m_matrixMaterialComboBox != nullptr)
  {
    m_matrixMaterialComboBox->setCurrentIndex(0);
  } 
  if(m_KEDLowerBoundField != nullptr)
  {
    m_KEDLowerBoundField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_KEDFitLowerBound, 'f', 4));
    m_KEDLowerBoundField->set_isValid(true);
  } 
  if(m_KEDUpperBoundField != nullptr)
  {
    m_KEDUpperBoundField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_KEDFitUpperBound, 'f', 4));
    m_KEDUpperBoundField->set_isValid(true);
  } 
  if(m_KEDBkgdLowLowBoundField != nullptr)
  {
    m_KEDBkgdLowLowBoundField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_KEDBkgdLowLowBound, 'f', 4));
    m_KEDBkgdLowLowBoundField->set_isValid(true);
  } 
  if(m_KEDBkgdLowHighBoundField != nullptr)
  {
    m_KEDBkgdLowHighBoundField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_KEDBkgdLowHighBound, 'f', 4));
    m_KEDBkgdLowHighBoundField->set_isValid(true);
  } 
  if(m_KEDBkgdHighLowBoundField != nullptr)
  {
    m_KEDBkgdHighLowBoundField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_KEDBkgdHighLowBound, 'f', 4));
    m_KEDBkgdHighLowBoundField->set_isValid(true);
  } 
  if(m_KEDBkgdHighHighBoundField != nullptr)
  {
    m_KEDBkgdHighHighBoundField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_KEDBkgdHighHighBound, 'f', 4));
    m_KEDBkgdHighHighBoundField->set_isValid(true);
  } 
  if(m_XRFLowerBoundField != nullptr)
  {
    m_XRFLowerBoundField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_XRFFitLowerBound, 'f', 4));
    m_XRFLowerBoundField->set_isValid(true);
  } 
  if(m_XRFUpperBoundField != nullptr)
  {
    m_XRFUpperBoundField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_XRFFitUpperBound, 'f', 4));
    m_XRFUpperBoundField->set_isValid(true);
  } 
  if(m_XRFBkgdLowLowBoundField != nullptr)
  {
    m_XRFBkgdLowLowBoundField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_XRFBkgdLowLowBound, 'f', 4));
    m_XRFBkgdLowLowBoundField->set_isValid(true);
  } 
  if(m_XRFBkgdLowHighBoundField != nullptr)
  {
    m_XRFBkgdLowHighBoundField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_XRFBkgdLowHighBound, 'f', 4));
    m_XRFBkgdLowHighBoundField->set_isValid(true);
  } 
  if(m_XRFBkgdHighLowBoundField != nullptr)
  {
    m_XRFBkgdHighLowBoundField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_XRFBkgdHighLowBound, 'f', 4));
    m_XRFBkgdHighLowBoundField->set_isValid(true);
  } 
  if(m_XRFBkgdHighHighBoundField != nullptr)
  {
    m_XRFBkgdHighHighBoundField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_XRFBkgdHighHighBound, 'f', 4));
    m_XRFBkgdHighHighBoundField->set_isValid(true);
  } 
  if(m_KEDCdCorrField != nullptr)
  {
    m_KEDCdCorrField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_KEDCdCorr, 'f', 4));
    m_KEDCdCorrField->set_isValid(true);
  } 
  if(m_KEDBkgdNormField != nullptr)
  {
    m_KEDBkgdNormField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_KEDBkgdNorm, 'f', 4));
    m_KEDBkgdNormField->set_isValid(true);
  } 
  if(m_KEDTempField != nullptr)
  {
    m_KEDTempField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_KEDTempCorr, 'f', 4));
    m_KEDTempField->set_isValid(true);
  } 
  if(m_CField != nullptr)
  {
    m_CField->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_backScattC, 'f', 4));
    m_CField->set_isValid(true);
  } 
  if(m_b3Field != nullptr)
  {
    m_b3Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_backScattb3, 'f', 4));
    m_b3Field->set_isValid(true);
  } 
  if(m_b4Field != nullptr)
  {
    m_b4Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_backScattb4, 'f', 4));
    m_b4Field->set_isValid(true);
  } 
  if(m_b5Field != nullptr)
  {
    m_b5Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_backScattb5, 'f', 4));
    m_b5Field->set_isValid(true);
  } 
  if(m_b6Field != nullptr)
  {
    m_b6Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_backScattb6, 'f', 4));
    m_b6Field->set_isValid(true);
  } 
  if(m_b7Field != nullptr)
  {
    m_b7Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_backScattb7, 'f', 4));
    m_b7Field->set_isValid(true);
  } 
  if(m_b8Field != nullptr)
  {
    m_b8Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_backScattb8, 'f', 4));
    m_b8Field->set_isValid(true);
  } 
  if(m_b9Field != nullptr)
  {
    m_b9Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_backScattb9, 'f', 4));
    m_b9Field->set_isValid(true);
  } 
  if(m_b10Field != nullptr)
  {
    m_b10Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_backScattb10, 'f', 4));
    m_b10Field->set_isValid(true);
  } 
  if(m_b11Field != nullptr)
  {
    m_b11Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_backScattb11, 'f', 4));
    m_b11Field->set_isValid(true);
  } 
  if(m_b12Field != nullptr)
  {
    m_b12Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_backScattb12, 'f', 4));
    m_b12Field->set_isValid(true);
  } 
  if(m_b13Field != nullptr)
  {
    m_b13Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_backScattb13, 'f', 4));
    m_b13Field->set_isValid(true);
  } 
  if(m_b14Field != nullptr)
  {
    m_b14Field->set_validated_double_text(QString::number(KFileManager::getInstance().m_workingParameterSet.m_backScattb14, 'f', 4));
    m_b14Field->set_isValid(true);
  } 
}

void NewParameterDialog::build_parameter_struct()
{
  m_workingParameterSet.m_setName = m_paramNameField->text().toStdString();
  m_workingParameterSet.m_authorName = m_authorNameField->text().toStdString();
  m_workingParameterSet.m_facilityName = m_facilityNameField->text().toStdString();
  m_workingParameterSet.m_date = m_dateEdit->date().toString().toStdString();
  m_workingParameterSet.m_comment = m_commentField->text().toStdString();
  m_workingParameterSet.m_generatorHVSetpoint = m_hvField->text().toDouble();
  m_workingParameterSet.m_beamIncidenceAngle = m_incidenceField->text().toDouble();
  m_workingParameterSet.m_takeoffAngle = m_takeoffField->text().toDouble();
  m_workingParameterSet.m_minBackscatterAngle = m_backscatterField->text().toDouble();
  m_workingParameterSet.m_beWindowThickness = m_beWindowField->text().toDouble();
  m_workingParameterSet.m_cdFilterThickness = m_cdFilterField->text().toDouble();
  m_workingParameterSet.m_feFilterThickness = m_feFilterField->text().toDouble();
  m_workingParameterSet.m_xfrEntraceThickness = m_xfrEntranceField->text().toDouble();
  m_workingParameterSet.m_xfrExitThickness = m_xfrExitField->text().toDouble();
  m_workingParameterSet.m_KEDA1 = m_KEDA1Field->text().toDouble();
  m_workingParameterSet.m_KEDA2 = m_KEDA2Field->text().toDouble();
  m_workingParameterSet.m_KEDA3 = m_KEDA3Field->text().toDouble();
  m_workingParameterSet.m_KEDT1 = m_KEDT1Field->text().toDouble();
  m_workingParameterSet.m_KEDT2 = m_KEDT2Field->text().toDouble();
  m_workingParameterSet.m_KEDT3 = m_KEDT3Field->text().toDouble();
  m_workingParameterSet.m_KEDT4 = m_KEDT4Field->text().toDouble();
  m_workingParameterSet.m_XRFA1 = m_XRFA1Field->text().toDouble();
  m_workingParameterSet.m_XRFA2 = m_XRFA2Field->text().toDouble();
  m_workingParameterSet.m_XRFA3 = m_XRFA3Field->text().toDouble();
  m_workingParameterSet.m_XRFT1 = m_XRFT1Field->text().toDouble();
  m_workingParameterSet.m_XRFT2 = m_XRFT2Field->text().toDouble();
  m_workingParameterSet.m_XRFT3 = m_XRFT3Field->text().toDouble();
  m_workingParameterSet.m_XRFT4 = m_XRFT4Field->text().toDouble();
  m_workingParameterSet.m_vialID = m_vialIDField->text().toDouble();
  m_workingParameterSet.m_vialOD = m_vialODField->text().toDouble();
  m_workingParameterSet.m_vialMaterial = m_vialMaterialComboBox->currentIndex();
  m_workingParameterSet.m_matrixMaterial = m_matrixMaterialComboBox->currentIndex();
  m_workingParameterSet.m_KEDFitLowerBound = m_KEDLowerBoundField->text().toDouble();
  m_workingParameterSet.m_KEDFitUpperBound = m_KEDUpperBoundField->text().toDouble();
  m_workingParameterSet.m_KEDBkgdLowLowBound = m_KEDBkgdLowLowBoundField->text().toDouble();
  m_workingParameterSet.m_KEDBkgdLowHighBound = m_KEDBkgdLowHighBoundField->text().toDouble();
  m_workingParameterSet.m_KEDBkgdHighLowBound = m_KEDBkgdHighLowBoundField->text().toDouble();
  m_workingParameterSet.m_KEDBkgdHighHighBound = m_KEDBkgdHighHighBoundField->text().toDouble();
  m_workingParameterSet.m_XRFFitLowerBound = m_XRFLowerBoundField->text().toDouble();
  m_workingParameterSet.m_XRFFitUpperBound = m_XRFUpperBoundField->text().toDouble();
  m_workingParameterSet.m_XRFBkgdLowLowBound = m_XRFBkgdLowLowBoundField->text().toDouble();
  m_workingParameterSet.m_XRFBkgdLowHighBound = m_XRFBkgdLowHighBoundField->text().toDouble();
  m_workingParameterSet.m_XRFBkgdHighLowBound = m_XRFBkgdHighLowBoundField->text().toDouble();
  m_workingParameterSet.m_XRFBkgdHighHighBound = m_XRFBkgdHighHighBoundField->text().toDouble();
  m_workingParameterSet.m_KEDCdCorr = m_KEDCdCorrField->text().toDouble();
  m_workingParameterSet.m_KEDBkgdNorm = m_KEDBkgdNormField->text().toDouble();
  m_workingParameterSet.m_KEDTempCorr = m_KEDTempField->text().toDouble();
  m_workingParameterSet.m_backScattC = m_CField->text().toDouble();
  m_workingParameterSet.m_backScattb3 = m_b3Field->text().toDouble();
  m_workingParameterSet.m_backScattb4 = m_b4Field->text().toDouble();
  m_workingParameterSet.m_backScattb5 = m_b5Field->text().toDouble();
  m_workingParameterSet.m_backScattb6 = m_b6Field->text().toDouble();
  m_workingParameterSet.m_backScattb7 = m_b7Field->text().toDouble();
  m_workingParameterSet.m_backScattb8 = m_b8Field->text().toDouble();
  m_workingParameterSet.m_backScattb9 = m_b9Field->text().toDouble();
  m_workingParameterSet.m_backScattb10 = m_b10Field->text().toDouble();
  m_workingParameterSet.m_backScattb11 = m_b11Field->text().toDouble();
  m_workingParameterSet.m_backScattb12 = m_b12Field->text().toDouble();
  m_workingParameterSet.m_backScattb13 = m_b13Field->text().toDouble();
  m_workingParameterSet.m_backScattb14 = m_b14Field->text().toDouble();
}

bool NewParameterDialog::check_valid_string(const QString& a_text)
{
  bool ok = false;
  std::string text = a_text.toStdString();
  bool isSpace = std::find_if(text.begin(), text.end(),
                    static_cast<int (*)(int)>(std::isspace)) != text.end();
  if(isSpace)
  {
    WarningMsgBox msgBox("Spaces are not allowed in the parameter set name.");
    int ret = msgBox.exec();
    return false;
  }
  bool isEmpty = text.empty();
  if(isEmpty)
  {
    //WarningMsgBox msgBox("You must supply a parameter set name.");
    //int ret = msgBox.exec();
    return false;
  }
  if(isSpace == false && isEmpty == false)
  {
    ok = true;
  }
  return ok;
}

void NewParameterDialog::on_ok_button_clicked()
{
  //Check to make sure user doesn't supply a name that is already used.
  //But we do need to allow for the edit scenario in which the name wasn't
  //changed.
  for(int i=0; i<KFileManager::getInstance().m_listofParamSets.size(); i++)
  {
    if(m_paramNameField->text().toStdString() == KFileManager::getInstance().m_listofParamSets.at(i) &&
       m_paramNameField->text().toStdString() != KFileManager::getInstance().m_openParamSetName)
    {
      WarningMsgBox msgBox("Paramater set name is already used. \n Choose another name.");
      int ret = msgBox.exec();
      return;
    }
  }
  build_parameter_struct();
  //copy the parameter set data into KFileManager
  KFileManager::getInstance().m_workingParameterSet = m_workingParameterSet;
  //Now save the copied data
  KFileManager::getInstance().save_parameter_set();
  KFileManager::getInstance().read_in_parameter_set_database();
  accept();
}

void NewParameterDialog::update_ok_button_state()
{
  bool allValid = false;
  if(m_paramNameField->get_isValid() == true &&
     m_hvField->get_isValid() == true &&
     m_incidenceField->get_isValid() == true &&
     m_takeoffField->get_isValid() == true &&
     m_backscatterField->get_isValid() == true &&
     m_beWindowField->get_isValid() == true &&
     m_cdFilterField->get_isValid() == true &&
     m_feFilterField->get_isValid() == true &&
     m_xfrEntranceField->get_isValid() == true &&
     m_xfrExitField->get_isValid() == true &&
     m_KEDA1Field->get_isValid() == true &&
     m_KEDA2Field->get_isValid() == true &&
     m_KEDA3Field->get_isValid() == true &&
     m_KEDT1Field->get_isValid() == true &&
     m_KEDT2Field->get_isValid() == true &&
     m_KEDT3Field->get_isValid() == true &&
     m_KEDT4Field->get_isValid() == true &&
     m_XRFA1Field->get_isValid() == true &&
     m_XRFA2Field->get_isValid() == true &&
     m_XRFA3Field->get_isValid() == true &&
     m_XRFT1Field->get_isValid() == true &&
     m_XRFT2Field->get_isValid() == true &&
     m_XRFT3Field->get_isValid() == true &&
     m_XRFT4Field->get_isValid() == true &&
     m_vialIDField->get_isValid() == true &&
     m_vialODField->get_isValid() == true &&
     m_KEDLowerBoundField->get_isValid() == true &&
     m_KEDUpperBoundField->get_isValid() == true &&
     m_KEDBkgdLowLowBoundField->get_isValid() == true &&
     m_KEDBkgdLowHighBoundField->get_isValid() == true &&
     m_KEDBkgdHighLowBoundField->get_isValid() == true &&
     m_KEDBkgdHighHighBoundField->get_isValid() == true &&
     m_XRFLowerBoundField->get_isValid() == true &&
     m_XRFUpperBoundField->get_isValid() == true &&
     m_XRFBkgdLowLowBoundField->get_isValid() == true &&
     m_XRFBkgdLowHighBoundField->get_isValid() == true &&
     m_XRFBkgdHighLowBoundField->get_isValid() == true &&
     m_XRFBkgdHighHighBoundField->get_isValid() == true &&
     m_KEDCdCorrField->get_isValid() == true &&
     m_KEDBkgdNormField->get_isValid() == true &&
     m_KEDTempField->get_isValid() == true &&
     m_CField->get_isValid() == true &&
     m_b3Field->get_isValid() == true &&
     m_b4Field->get_isValid() == true &&
     m_b5Field->get_isValid() == true &&
     m_b6Field->get_isValid() == true &&
     m_b7Field->get_isValid() == true &&
     m_b8Field->get_isValid() == true &&
     m_b9Field->get_isValid() == true &&
     m_b10Field->get_isValid() == true &&
     m_b11Field->get_isValid() == true &&
     m_b12Field->get_isValid() == true &&
     m_b13Field->get_isValid() == true &&
     m_b14Field->get_isValid() == true)
  {
    allValid = true;
  }
  m_okButton->setDisabled(!allValid);
  m_okButton->set_style(allValid);
}

void NewParameterDialog::on_backscattBrem_button_clicked()
{
  CustomMsgBox msgBox(tr("<p><b>Constants to define the continuum underlying the XRF spectrum.</b></p>"
                         "<p>This backscattered Bremsstrahlung shape is repesented by a sum of two "
                         "truncated expansions in sech."
                         "These constants will be different for every HKED instrument and it recommended to "
                         "let Mari determine them using the 'calibrate' feature. </p>"
                         "<p>See the User's Manual for more information on the backscattered Bremsstrahlung "
                         "continuum as well as how to use the 'calibrate' feature.</p>"));
  int ret = msgBox.exec();
}
