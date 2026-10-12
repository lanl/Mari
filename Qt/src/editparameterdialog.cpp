#include "editparameterdialog.h"

EditParameterDialog::EditParameterDialog(QWidget *parent)
    : QDialog(parent)
{
  KDevLogger::getInstance().logEvent("EditParameterDialog object constructed");
  //gui stuff
  resize(250, 150);
  this->setWindowTitle("Edit Parameter Set");
  this->setStyleSheet("background-color: white");
  QVBoxLayout* newParamVLayout = new QVBoxLayout(this);

  //Horizontal layout for combo box
  QHBoxLayout* comboBoxHLayout = new QHBoxLayout();
  m_paramComboBox =  new QComboBox();
  m_paramComboBox->setObjectName("paramComboBox");
  m_paramComboBox->setStyleSheet("border-top: 2px solid gray;"
                                 "border-left: 2px solid gray;"
                                 "border-bottom: 1px solid lightgray;"
                                 "border-right: 1px solid lightgray;"
                                 "background-color: white;");
  //Fill out the combo box with the parameter set names from the database
  for(int i=0; i<KFileManager::getInstance().m_listofParamSets.size(); i++)
  {
    //don't allow user to delete EXAMPLE parameter set
    if(KFileManager::getInstance().m_listofParamSets.at(i) != "EXAMPLE")
    {
      m_paramComboBox->addItem(QString::fromStdString(KFileManager::getInstance().m_listofParamSets.at(i)));
    }
  }
  comboBoxHLayout->addWidget(m_paramComboBox);

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
  newParamVLayout->addLayout(comboBoxHLayout);
  newParamVLayout->addLayout(okCancelHLayout);
  //connect cancel button to Qt internal close (reject) mechanism
  connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
  //connect ok button to internal slot
  connect(m_okButton, &QPushButton::clicked, this, &EditParameterDialog::on_ok_button_clicked);
}

EditParameterDialog::~EditParameterDialog()
{
  KDevLogger::getInstance().logEvent("EditParameterDialog object destructed");
}

void EditParameterDialog::on_ok_button_clicked()
{
  KFileManager::getInstance().load_parameter_set(m_paramComboBox->currentText().toStdString());
  open_parameter_set_for_editing();
}

