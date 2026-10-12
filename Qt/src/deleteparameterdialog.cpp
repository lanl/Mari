#include "deleteparameterdialog.h"

DeleteParameterDialog::DeleteParameterDialog(QWidget *parent)
    : QDialog(parent)
{
  KDevLogger::getInstance().logEvent("DeleteParameterDialog object constructed");
  //gui stuff
  resize(250, 150);
  this->setWindowTitle("Delete Parameter Set");
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
  connect(m_okButton, &QPushButton::clicked, this, &DeleteParameterDialog::on_ok_button_clicked);
}

DeleteParameterDialog::~DeleteParameterDialog()
{
  KDevLogger::getInstance().logEvent("DeleteParameterDialog object destructed");
}

void DeleteParameterDialog::on_ok_button_clicked()
{
  QString setToDeleteQStr = m_paramComboBox->currentText();
  QString message = "Are you sure you want to delete the " + setToDeleteQStr + " parameter set?";
  YesNoMsgBox msgBox(message);
  int ret = msgBox.exec();
  if(ret==2)
  {
    std::string setToDelete = setToDeleteQStr.toStdString();
    //remove the parameter set from the database.
    KFileManager::getInstance().m_parameterDatabase.erase(
        std::remove_if(KFileManager::getInstance().m_parameterDatabase.begin(), 
                       KFileManager::getInstance().m_parameterDatabase.end(),
                            [setToDelete](const parameter_data::KParameterData& set) 
                            {
                              return set.m_setName == setToDelete;
                            }),
        KFileManager::getInstance().m_parameterDatabase.end()
    );
    //Save and reload the new database
    KFileManager::getInstance().write_database_to_disk();
    KFileManager::getInstance().read_in_parameter_set_database();
    accept();
  }
  else
  {
    reject();
  }
}

