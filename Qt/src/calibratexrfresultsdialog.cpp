#include "calibratexrfresultsdialog.h"

CalibrateXRFResultsDialog::CalibrateXRFResultsDialog(QWidget *parent)
    : QDialog(parent)
{
  KDevLogger::getInstance().logEvent("CalibrateXRFResultsDialog object constructed");
  resize(920, 600);
  this->setWindowTitle("XRF Instrument Calibration Results");
  this->setStyleSheet("background-color: white");

  //Make the  central widget.
  QVBoxLayout* mainVLayout = new QVBoxLayout(this);
  m_mainTabWidget = new QTabWidget();
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

  // Add tabs to the QTabWidget
  m_mainTabWidget->addTab(layout_plot_tab(), "Plot");
  m_mainTabWidget->addTab(layout_results_tab(), "Fit Results");

  //Horizontal layout for ok and cancel buttons
  QHBoxLayout* okCancelHLayout = new QHBoxLayout();
  m_acceptButton = new OkButton();
  m_acceptButton->setText("Accept");
  m_acceptButton->setFixedWidth(90);
  CancelButton* cancelButton = new CancelButton();
  okCancelHLayout->addItem(new QSpacerItem(10, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  okCancelHLayout->addWidget(m_acceptButton);
  okCancelHLayout->addItem(new QSpacerItem(15, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  okCancelHLayout->addWidget(cancelButton);
  okCancelHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  mainVLayout->addWidget(m_mainTabWidget);
  mainVLayout->addItem(new QSpacerItem(1, 30, QSizePolicy::Minimum, QSizePolicy::Fixed));
  mainVLayout->addLayout(okCancelHLayout);

  //Question button connections
  connect(m_algoAndConvButton,  &QPushButton::clicked, this, &CalibrateXRFResultsDialog::on_algoAndConv_button_clicked);
  connect(m_initCostButton,  &QPushButton::clicked, this, &CalibrateXRFResultsDialog::on_initCost_button_clicked);
  connect(m_finalCostButton,  &QPushButton::clicked, this, &CalibrateXRFResultsDialog::on_finalCost_button_clicked);
  connect(m_stepSuccessButton,  &QPushButton::clicked, this, &CalibrateXRFResultsDialog::on_stepSuccess_button_clicked);
  connect(m_chiSqButton,  &QPushButton::clicked, this, &CalibrateXRFResultsDialog::on_chiSq_button_clicked);
  connect(m_gradMaxButton,  &QPushButton::clicked, this, &CalibrateXRFResultsDialog::on_gradMax_button_clicked);
  connect(m_relDecreaseButton,  &QPushButton::clicked, this, &CalibrateXRFResultsDialog::on_relDecrease_button_clicked);
  connect(m_stepNormButton,  &QPushButton::clicked, this, &CalibrateXRFResultsDialog::on_stepNorm_button_clicked);
  //connect cancel button to Qt internal close (reject) mechanism
  connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
  //connect ok button to internal slot
  connect(m_acceptButton, &QPushButton::clicked, this, &CalibrateXRFResultsDialog::on_accept_button_clicked);

  plot_fit();
  display_solver_results();
}

QWidget* CalibrateXRFResultsDialog::layout_plot_tab()
{
  QWidget* plotPage = new QWidget(m_mainTabWidget);

  //Main dialog vertical layout
  QVBoxLayout* plotVLayout = new QVBoxLayout(plotPage);

  //Horizontal layout for main plot.
  QHBoxLayout* mainPlotHLayout = new QHBoxLayout();
  m_XRFPlot = new QCustomPlot(this);
  QFont labelFontX = m_XRFPlot->xAxis->labelFont();
  labelFontX.setPointSize(11);
  QFont labelFontY = m_XRFPlot->yAxis->labelFont();
  labelFontY.setPointSize(11);
  m_XRFPlot->yAxis->setLabelFont(labelFontY);
  m_XRFPlot->xAxis->setTickLabels(false);
  m_XRFPlot->xAxis->setLabel("");
  m_XRFPlot->yAxis->setLabel("Counts");
  m_XRFPlot->setInteractions(QCP::iSelectItems | QCP::iRangeDrag | QCP::iRangeZoom);
  mainPlotHLayout->addWidget(m_XRFPlot);

  //Horizontal layout for residuals plot.
  QHBoxLayout* residualsPlotHLayout = new QHBoxLayout();
  m_residualsPlot = new QCustomPlot(this);
  m_residualsPlot->xAxis->setLabelFont(labelFontX);
  m_residualsPlot->yAxis->setLabelFont(labelFontY);
  m_residualsPlot->xAxis->setLabel("Energy (keV)");
  m_residualsPlot->yAxis->setLabel("Residual");
  m_residualsPlot->setInteractions(QCP::iSelectItems | QCP::iRangeDrag | QCP::iRangeZoom);
  residualsPlotHLayout->addWidget(m_residualsPlot);

  // Make plot areas align
  auto* marginGroup = new QCPMarginGroup(m_XRFPlot);
  m_XRFPlot->axisRect()->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);
  m_residualsPlot->axisRect()->setMarginGroup(QCP::msLeft | QCP::msRight, marginGroup);

  // Sync top/bottom secondary axes too
  connect(m_XRFPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),
          m_XRFPlot->xAxis2, SLOT(setRange(QCPRange)));
  connect(m_residualsPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),
          m_residualsPlot->xAxis2, SLOT(setRange(QCPRange)));

  // Sync main and residual x ranges
  connect(m_XRFPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),
          m_residualsPlot->xAxis, SLOT(setRange(QCPRange)));
  connect(m_residualsPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),
          m_XRFPlot->xAxis, SLOT(setRange(QCPRange)));

  // Replot the other widget when one range changes
  connect(m_XRFPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),
          m_residualsPlot, SLOT(replot()));

  connect(m_residualsPlot->xAxis, SIGNAL(rangeChanged(QCPRange)),
          m_XRFPlot, SLOT(replot()));

  m_XRFPlot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  m_residualsPlot->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  //Put the H layouts in mainplot V layout
  plotVLayout->addItem(new QSpacerItem(1, 20, QSizePolicy::Minimum, QSizePolicy::Fixed));
  plotVLayout->addLayout(mainPlotHLayout, 3);
  plotVLayout->addLayout(residualsPlotHLayout, 1);

  return plotPage;
}

QWidget* CalibrateXRFResultsDialog::layout_results_tab()
{
  QWidget* fitResultsPage = new QWidget(m_mainTabWidget);
  //Create the main vertical layout
  QVBoxLayout* fitResultsVLayout = new QVBoxLayout(fitResultsPage);

  //Horizontal layout for Algorithm & Convergence title.
  QHBoxLayout* algoAndConvTitleHLayout = new QHBoxLayout();
  QLabel* algoAndConvTitleLabel = new QLabel;
  algoAndConvTitleLabel->setText("Solver Convergence and Fit Quality");
  algoAndConvTitleLabel->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  m_algoAndConvButton = new QuestionButton;
  algoAndConvTitleHLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  algoAndConvTitleHLayout->addWidget(algoAndConvTitleLabel);
  algoAndConvTitleHLayout->addWidget(m_algoAndConvButton);
  algoAndConvTitleHLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  algoAndConvTitleHLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for termination line.
  QHBoxLayout* terminationHLayout = new QHBoxLayout();
  QLabel* algoLabel = new QLabel;
  algoLabel->setText("Algorithm Type :");
  m_algoField = new NonUpdatableLabel;
  m_algoField->setFixedWidth(175); 
  QLabel* terminationLabel = new QLabel;
  terminationLabel->setText("Termination Type :");
  m_terminationField = new NonUpdatableLabel;
  m_terminationField->setFixedWidth(175); 
  QLabel* chiSqLabel = new QLabel;
  chiSqLabel->setText("&chi;<sup>2</sup> :");
  m_chiSqField = new NonUpdatableLabel;
  m_chiSqField->setFixedWidth(100); 
  m_chiSqButton = new QuestionButton;
  terminationHLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  terminationHLayout->addWidget(algoLabel);
  terminationHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  terminationHLayout->addWidget(m_algoField);
  terminationHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  terminationHLayout->addWidget(terminationLabel);
  terminationHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  terminationHLayout->addWidget(m_terminationField);
  terminationHLayout->addItem(new QSpacerItem(20, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  terminationHLayout->addWidget(chiSqLabel);
  terminationHLayout->addWidget(m_chiSqButton);
  terminationHLayout->addWidget(m_chiSqField);
  terminationHLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for message line.
  QHBoxLayout* messageHLayout = new QHBoxLayout();
  QLabel* msgLabel = new QLabel;
  msgLabel->setText("Message :");
  m_msgField = new NonUpdatableLabel;
  m_msgField->setFixedWidth(732); 
  messageHLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  messageHLayout->addWidget(msgLabel);
  messageHLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  messageHLayout->addWidget(m_msgField);
  messageHLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for cost line.
  QHBoxLayout* costHLayout = new QHBoxLayout();
  QLabel* initCostLabel = new QLabel;
  initCostLabel->setText("Initial Cost :");
  m_initCostField = new NonUpdatableLabel;
  m_initCostField->setFixedWidth(130); 
  m_initCostButton = new QuestionButton;
  QLabel* finalCostLabel = new QLabel;
  finalCostLabel->setText("Final Cost :");
  m_finalCostField = new NonUpdatableLabel;
  m_finalCostField->setFixedWidth(130); 
  m_finalCostButton = new QuestionButton;
  QLabel* stepSuccessLabel = new QLabel;
  stepSuccessLabel->setText("Step Success :");
  m_stepSuccessField = new NonUpdatableLabel;
  m_stepSuccessField->setFixedWidth(130); 
  m_stepSuccessButton = new QuestionButton;
  costHLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  costHLayout->addWidget(initCostLabel);
  costHLayout->addWidget(m_initCostButton);
  costHLayout->addItem(new QSpacerItem(51, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  costHLayout->addWidget(m_initCostField);
  costHLayout->addItem(new QSpacerItem(10, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  costHLayout->addWidget(finalCostLabel);
  costHLayout->addWidget(m_finalCostButton);
  costHLayout->addItem(new QSpacerItem(42, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  costHLayout->addWidget(m_finalCostField);
  costHLayout->addItem(new QSpacerItem(10, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  costHLayout->addWidget(stepSuccessLabel);
  costHLayout->addWidget(m_stepSuccessButton);
  costHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  costHLayout->addWidget(m_stepSuccessField);
  costHLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for gradient line.
  QHBoxLayout* gradientHLayout = new QHBoxLayout();
  QLabel* gradMaxlLabel = new QLabel;
  gradMaxlLabel->setText("Gradient Max Norm :");
  m_gradMaxField = new NonUpdatableLabel;
  m_gradMaxField->setFixedWidth(130); 
  m_gradMaxButton = new QuestionButton;
  QLabel* relDecreaseLabel = new QLabel;
  relDecreaseLabel->setText("Relative Decrease :");
  m_relDecreaseField = new NonUpdatableLabel;
  m_relDecreaseField->setFixedWidth(130); 
  m_relDecreaseButton = new QuestionButton;
  QLabel* stepNormLabel = new QLabel;
  stepNormLabel->setText("Step Norm :");
  m_stepNormField = new NonUpdatableLabel;
  m_stepNormField->setFixedWidth(130); 
  m_stepNormButton = new QuestionButton;
  gradientHLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  gradientHLayout->addWidget(gradMaxlLabel);
  gradientHLayout->addWidget(m_gradMaxButton);
  gradientHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  gradientHLayout->addWidget(m_gradMaxField);
  gradientHLayout->addItem(new QSpacerItem(10, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  gradientHLayout->addWidget(relDecreaseLabel);
  gradientHLayout->addWidget(m_relDecreaseButton);
  gradientHLayout->addItem(new QSpacerItem(3, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  gradientHLayout->addWidget(m_relDecreaseField);
  gradientHLayout->addItem(new QSpacerItem(10, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  gradientHLayout->addWidget(stepNormLabel);
  gradientHLayout->addWidget(m_stepNormButton);
  gradientHLayout->addItem(new QSpacerItem(13, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  gradientHLayout->addWidget(m_stepNormField);

  gradientHLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for the Eqn header
  QHBoxLayout* eqnHeaderHLayout = new QHBoxLayout();
  QLabel* eqnHeader = new QLabel;
  eqnHeader->setText("Fit Results for the Backscattered Bremsstrahlung Continuum");
  eqnHeader->setStyleSheet("color: #005975; font-weight: bold; font-size: 13px;");
  eqnHeaderHLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  eqnHeaderHLayout->addWidget(eqnHeader);

  //Horizontal layout for the Eqn 
  //Horizontal layout for the Eqn 
  QHBoxLayout* eqnHLayout = new QHBoxLayout();
  QLabel* XRFBremmEqn = new QLabel;
  XRFBremmEqn->setTextFormat(Qt::RichText);
  XRFBremmEqn->setWordWrap(false);
  XRFBremmEqn->setSizePolicy(QSizePolicy::MinimumExpanding,
                           QSizePolicy::Preferred);

  XRFBremmEqn->setText(
  "<table cellspacing='0' cellpadding='0' border='0'>"

    // ============================================================
    // Equation line 1
    // ============================================================
    "<tr>"
      "<td>"
        "<table cellspacing='0' cellpadding='0' border='0'>"
          "<tr>"

            // Prefix and large opening parenthesis
            "<td valign='middle' style='padding-right:3px;'>"
              "B<sub>brem</sub> = C &times;"
            "</td>"
            "<td valign='middle' "
                "style='font-family:serif; font-size:32px; padding-right:2px;'>"
              "("
            "</td>"

            // B4
            "<td valign='middle'>B<sub>4</sub> &times; sech</td>"
            "<td valign='middle' style='font-family:serif; font-size:28px;'>"
              "("
            "</td>"
            "<td valign='middle' style='padding:0 1px;'>"
              "<table cellspacing='0' cellpadding='0' border='0'>"
                "<tr>"
                  "<td align='center' "
                      "style='border-bottom:1px solid #005975; padding:0 2px;'>"
                    "E &minus; 85.283"
                  "</td>"
                "</tr>"
                "<tr>"
                  "<td align='center'>B<sub>3</sub></td>"
                "</tr>"
              "</table>"
            "</td>"
            "<td valign='middle' style='font-family:serif; font-size:28px;'>"
              ")"
            "</td>"
            "<td valign='middle' style='padding:0 4px;'>+</td>"

            // B5
            "<td valign='middle'>B<sub>5</sub> &times; sech</td>"
            "<td valign='middle' style='font-family:serif; font-size:28px;'>"
              "("
            "</td>"
            "<td valign='middle' style='padding:0 1px;'>"
              "<table cellspacing='0' cellpadding='0' border='0'>"
                "<tr>"
                  "<td align='center' "
                      "style='border-bottom:1px solid #005975; padding:0 2px;'>"
                    "E &minus; 82.893"
                  "</td>"
                "</tr>"
                "<tr>"
                  "<td align='center'>B<sub>3</sub></td>"
                "</tr>"
              "</table>"
            "</td>"
            "<td valign='middle' style='font-family:serif; font-size:28px;'>"
              ")"
            "</td>"
            "<td valign='middle' style='padding:0 4px;'>+</td>"

            // B7
            "<td valign='middle'>B<sub>7</sub> &times; sech</td>"
            "<td valign='middle' style='font-family:serif; font-size:28px;'>"
              "("
            "</td>"
            "<td valign='middle' style='padding:0 1px;'>"
              "<table cellspacing='0' cellpadding='0' border='0'>"
                "<tr>"
                  "<td align='center' "
                      "style='border-bottom:1px solid #005975; padding:0 2px;'>"
                    "(E &minus; 85.283)<sup>2</sup>"
                  "</td>"
                "</tr>"
                "<tr>"
                  "<td align='center'>B<sub>6</sub></td>"
                "</tr>"
              "</table>"
            "</td>"
            "<td valign='middle' style='font-family:serif; font-size:28px;'>"
              ")"
            "</td>"
            "<td valign='middle' style='padding:0 4px;'>+</td>"

            // B8
            "<td valign='middle'>B<sub>8</sub> &times; sech</td>"
            "<td valign='middle' style='font-family:serif; font-size:28px;'>"
              "("
            "</td>"
            "<td valign='middle' style='padding:0 1px;'>"
              "<table cellspacing='0' cellpadding='0' border='0'>"
                "<tr>"
                  "<td align='center' "
                      "style='border-bottom:1px solid #005975; padding:0 2px;'>"
                    "(E &minus; 82.893)<sup>2</sup>"
                  "</td>"
                "</tr>"
                "<tr>"
                  "<td align='center'>B<sub>6</sub></td>"
                "</tr>"
              "</table>"
            "</td>"
            "<td valign='middle' style='font-family:serif; font-size:28px;'>"
              ")"
            "</td>"
            "<td valign='middle'>+</td>"

          "</tr>"
        "</table>"
      "</td>"
    "</tr>"

    // Small vertical separation between lines
    "<tr><td height='3'></td></tr>"

    // ============================================================
    // Equation line 2
    // ============================================================
    "<tr>"
      "<td style='padding-left:80px;'>"
        "<table cellspacing='0' cellpadding='0' border='0'>"
          "<tr>"

            // B10
            "<td valign='middle'>B<sub>10</sub> &times; sech</td>"
            "<td valign='middle' style='font-family:serif; font-size:28px;'>"
              "("
            "</td>"
            "<td valign='middle' style='padding:0 1px;'>"
              "<table cellspacing='0' cellpadding='0' border='0'>"
                "<tr>"
                  "<td align='center' "
                      "style='border-bottom:1px solid #005975; padding:0 2px;'>"
                    "(E &minus; 85.283)<sup>3</sup>"
                  "</td>"
                "</tr>"
                "<tr>"
                  "<td align='center'>B<sub>9</sub></td>"
                "</tr>"
              "</table>"
            "</td>"
            "<td valign='middle' style='font-family:serif; font-size:28px;'>"
              ")"
            "</td>"
            "<td valign='middle' style='padding:0 4px;'>+</td>"

            // B11
            "<td valign='middle'>B<sub>11</sub> &times; sech</td>"
            "<td valign='middle' style='font-family:serif; font-size:28px;'>"
              "("
            "</td>"
            "<td valign='middle' style='padding:0 1px;'>"
              "<table cellspacing='0' cellpadding='0' border='0'>"
                "<tr>"
                  "<td align='center' "
                      "style='border-bottom:1px solid #005975; padding:0 2px;'>"
                    "(E &minus; 82.893)<sup>3</sup>"
                  "</td>"
                "</tr>"
                "<tr>"
                  "<td align='center'>B<sub>9</sub></td>"
                "</tr>"
              "</table>"
            "</td>"
            "<td valign='middle' style='font-family:serif; font-size:28px;'>"
              ")"
            "</td>"
            "<td valign='middle' style='padding:0 4px;'>+</td>"

            // B13
            "<td valign='middle'>B<sub>13</sub> &times; sech</td>"
            "<td valign='middle' style='font-family:serif; font-size:28px;'>"
              "("
            "</td>"
            "<td valign='middle' style='padding:0 1px;'>"
              "<table cellspacing='0' cellpadding='0' border='0'>"
                "<tr>"
                  "<td align='center' "
                      "style='border-bottom:1px solid #005975; padding:0 2px;'>"
                    "(E &minus; 85.283)<sup>4</sup>"
                  "</td>"
                "</tr>"
                "<tr>"
                  "<td align='center'>B<sub>12</sub></td>"
                "</tr>"
              "</table>"
            "</td>"
            "<td valign='middle' style='font-family:serif; font-size:28px;'>"
              ")"
            "</td>"
            "<td valign='middle' style='padding:0 4px;'>+</td>"

            // B14
            "<td valign='middle'>B<sub>14</sub> &times; sech</td>"
            "<td valign='middle' style='font-family:serif; font-size:28px;'>"
              "("
            "</td>"
            "<td valign='middle' style='padding:0 1px;'>"
              "<table cellspacing='0' cellpadding='0' border='0'>"
                "<tr>"
                  "<td align='center' "
                      "style='border-bottom:1px solid #005975; padding:0 2px;'>"
                    "(E &minus; 82.893)<sup>4</sup>"
                  "</td>"
                "</tr>"
                "<tr>"
                  "<td align='center'>B<sub>12</sub></td>"
                "</tr>"
              "</table>"
            "</td>"
            "<td valign='middle' style='font-family:serif; font-size:28px;'>"
              ")"
            "</td>"

            // Close C × (...)
            "<td valign='middle' "
                "style='font-family:serif; font-size:32px; padding-left:2px;'>"
              ")"
            "</td>"

          "</tr>"
        "</table>"
      "</td>"
    "</tr>"

    "</table>"
  );

  XRFBremmEqn->setStyleSheet(
    "QLabel {"
    "  color: #005975;"
    "  font-family: monospace;"
    "  font-size: 13px;"
    "}"
  );

  eqnHLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  eqnHLayout->addWidget(XRFBremmEqn);

  //Horizontal layout for C value.
  QHBoxLayout* CValHLayout = new QHBoxLayout();
  QLabel* CLabel = new QLabel;
  CLabel->setText("C:    ");
  m_CField = new NonUpdatableLabel;
  m_CField->setFixedWidth(150); 
  CValHLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  CValHLayout->addWidget(CLabel);
  CValHLayout->addItem(new QSpacerItem(2, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  CValHLayout->addWidget(m_CField);
  CValHLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));

  //Horizontal layout for B values line 1.
  QHBoxLayout* BVals1HLayout = new QHBoxLayout();
  QLabel* B3Label = new QLabel;
  B3Label->setText("B<sub>3</sub> :  ");
  m_B3Field = new NonUpdatableLabel;
  m_B3Field->setFixedWidth(150); 
  QLabel* B4Label = new QLabel;
  B4Label->setText("B<sub>4</sub> :  ");
  m_B4Field = new NonUpdatableLabel;
  m_B4Field->setFixedWidth(150); 
  QLabel* B5Label = new QLabel;
  B5Label->setText("B<sub>5</sub> :  ");
  m_B5Field = new NonUpdatableLabel;
  m_B5Field->setFixedWidth(150); 
  QLabel* B6Label = new QLabel;
  B6Label->setText("B<sub>6</sub> :  ");
  m_B6Field = new NonUpdatableLabel;
  m_B6Field->setFixedWidth(150); 
  BVals1HLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  BVals1HLayout->addWidget(B3Label);
  BVals1HLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  BVals1HLayout->addWidget(m_B3Field);
  BVals1HLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  BVals1HLayout->addWidget(B4Label);
  BVals1HLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  BVals1HLayout->addWidget(m_B4Field);
  BVals1HLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  BVals1HLayout->addWidget(B5Label);
  BVals1HLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  BVals1HLayout->addWidget(m_B5Field);
  BVals1HLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  BVals1HLayout->addWidget(B6Label);
  BVals1HLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  BVals1HLayout->addWidget(m_B6Field);
  BVals1HLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for B values line 2.
  QHBoxLayout* BVals2HLayout = new QHBoxLayout();
  QLabel* B7Label = new QLabel;
  B7Label->setText("B<sub>7</sub> :  ");
  m_B7Field = new NonUpdatableLabel;
  m_B7Field->setFixedWidth(150); 
  QLabel* B8Label = new QLabel;
  B8Label->setText("B<sub>8</sub> :  ");
  m_B8Field = new NonUpdatableLabel;
  m_B8Field->setFixedWidth(150); 
  QLabel* B9Label = new QLabel;
  B9Label->setText("B<sub>9</sub> :  ");
  m_B9Field = new NonUpdatableLabel;
  m_B9Field->setFixedWidth(150); 
  QLabel* B10Label = new QLabel;
  B10Label->setText("B<sub>10</sub> :");
  m_B10Field = new NonUpdatableLabel;
  m_B10Field->setFixedWidth(150); 
  BVals2HLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  BVals2HLayout->addWidget(B7Label);
  BVals2HLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  BVals2HLayout->addWidget(m_B7Field);
  BVals2HLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  BVals2HLayout->addWidget(B8Label);
  BVals2HLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  BVals2HLayout->addWidget(m_B8Field);
  BVals2HLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  BVals2HLayout->addWidget(B9Label);
  BVals2HLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  BVals2HLayout->addWidget(m_B9Field);
  BVals2HLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  BVals2HLayout->addWidget(B10Label);
  BVals2HLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  BVals2HLayout->addWidget(m_B10Field);
  BVals2HLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Horizontal layout for B values line 3.
  QHBoxLayout* BVals3HLayout = new QHBoxLayout();

  QLabel* B11Label = new QLabel;
  B11Label->setText("B<sub>11</sub> :");
  m_B11Field = new NonUpdatableLabel;
  m_B11Field->setFixedWidth(150); 
  QLabel* B12Label = new QLabel;
  B12Label->setText("B<sub>12</sub> :");
  m_B12Field = new NonUpdatableLabel;
  m_B12Field->setFixedWidth(150); 
  QLabel* B13Label = new QLabel;
  B13Label->setText("B<sub>13</sub> :");
  m_B13Field = new NonUpdatableLabel;
  m_B13Field->setFixedWidth(150); 
  QLabel* B14Label = new QLabel;
  B14Label->setText("B<sub>14</sub> :");
  m_B14Field = new NonUpdatableLabel;
  m_B14Field->setFixedWidth(150); 
  BVals3HLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  BVals3HLayout->addWidget(B11Label);
  BVals3HLayout->addItem(new QSpacerItem(4, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  BVals3HLayout->addWidget(m_B11Field);
  BVals3HLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  BVals3HLayout->addWidget(B12Label);
  BVals3HLayout->addItem(new QSpacerItem(4, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  BVals3HLayout->addWidget(m_B12Field);
  BVals3HLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  BVals3HLayout->addWidget(B13Label);
  BVals3HLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  BVals3HLayout->addWidget(m_B13Field);
  BVals3HLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum));
  BVals3HLayout->addWidget(B14Label);
  BVals3HLayout->addItem(new QSpacerItem(5, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));
  BVals3HLayout->addWidget(m_B14Field);
  BVals3HLayout->addItem(new QSpacerItem(25, 1, QSizePolicy::Fixed, QSizePolicy::Minimum));

  //Put the H layouts in main V layout
  fitResultsVLayout->addItem(new QSpacerItem(1, 20, QSizePolicy::Minimum, QSizePolicy::Fixed));
  fitResultsVLayout->addLayout(algoAndConvTitleHLayout);
  fitResultsVLayout->addLayout(terminationHLayout);
  fitResultsVLayout->addLayout(messageHLayout);
  fitResultsVLayout->addItem(new QSpacerItem(1, 20, QSizePolicy::Minimum, QSizePolicy::Fixed));
  fitResultsVLayout->addLayout(costHLayout);
  fitResultsVLayout->addLayout(gradientHLayout);
  fitResultsVLayout->addItem(new QSpacerItem(1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding));
  fitResultsVLayout->addLayout(eqnHeaderHLayout);
  fitResultsVLayout->addLayout(eqnHLayout);
  fitResultsVLayout->addItem(new QSpacerItem(1, 20, QSizePolicy::Minimum, QSizePolicy::Fixed));
  fitResultsVLayout->addLayout(CValHLayout);
  fitResultsVLayout->addLayout(BVals1HLayout);
  fitResultsVLayout->addLayout(BVals2HLayout);
  fitResultsVLayout->addLayout(BVals3HLayout);

  return fitResultsPage;
}

CalibrateXRFResultsDialog::~CalibrateXRFResultsDialog()
{
  KDevLogger::getInstance().logEvent("CalibrateXRFResultsDialog object destructed");
}


void CalibrateXRFResultsDialog::plot_fit()
{
  
  QVector<double> dataX(KResultsManager::getInstance().m_XRFCalResults.m_fitROIEnergies.begin(),     KResultsManager::getInstance().m_XRFCalResults.m_fitROIEnergies.end());
  QVector<double> dataY(KResultsManager::getInstance().m_XRFCalResults.m_fitROIDataCounts.begin(),   KResultsManager::getInstance().m_XRFCalResults.m_fitROIDataCounts.end());
  QVector<double> modelY(KResultsManager::getInstance().m_XRFCalResults.m_fitROIModelCounts.begin(), KResultsManager::getInstance().m_XRFCalResults.m_fitROIModelCounts.end());

  QVector<double> residualsX(KResultsManager::getInstance().m_XRFCalResults.m_residualsX.begin(), KResultsManager::getInstance().m_XRFCalResults.m_residualsX.end());
  QVector<double> residualsY(KResultsManager::getInstance().m_XRFCalResults.m_residualsY.begin(), KResultsManager::getInstance().m_XRFCalResults.m_residualsY.end());
  
  //QVector<double> continuumCounts(KResultsManager::getInstance().m_XRFCalResults.m_continuumCounts.begin(), KResultsManager::getInstance().m_XRFCalResults.m_continuumCounts.end());
  //QVector<double> coincCounts(m_xrfCalResults.m_coincCounts.begin(), m_xrfResults.m_coincCounts.end());
  
  //double maxYValue = KMath::maxCounts(KFileManager::getInstance().m_xrfRefData.m_counts);
  //double maxXValue = KMath::maxCounts(KFileManager::getInstance().m_xrfRefData.m_energies);

  if (m_XRFPlot) 
  {
    //Need max counts to set plot scale.
    //not checking to make sure there is data since this already done
    //when the file was loaded.
    //plot the data
    m_XRFPlot->addGraph();
    m_XRFPlot->graph(0)->setData(dataX, dataY);
    m_XRFPlot->graph(0)->setLineStyle(QCPGraph::lsStepCenter);
    m_XRFPlot->graph(0)->setPen(QPen(Qt::blue));
    //plot the fit curve
    m_XRFPlot->addGraph();
    m_XRFPlot->graph(1)->setData(dataX, modelY);
    m_XRFPlot->graph(1)->setLineStyle(QCPGraph::lsLine);
    m_XRFPlot->graph(1)->setPen(QPen(Qt::red));
    //plot the continuum
    //m_XRFPlot->addGraph();
    //m_XRFPlot->graph(2)->setData(dataX, continuumCounts);
    //m_XRFPlot->graph(2)->setLineStyle(QCPGraph::lsLine);
    //m_XRFPlot->graph(2)->setPen(QPen(Qt::green));
    //plot the coinc summing
   // m_XRFPlot->addGraph();
   // m_XRFPlot->graph(2)->setData(dataX, coincCounts);
   // m_XRFPlot->graph(2)->setLineStyle(QCPGraph::lsLine);
   // m_XRFPlot->graph(2)->setPen(QPen(Qt::gray));
    m_XRFPlot->rescaleAxes();
    m_XRFPlot->replot();
    
  }
  else
  {
    //FRY: should change this to send error back to runamangaer? FRY//
    std::cout<< "OOPS! Something went wrong." << std::endl; 
  }

  if (m_residualsPlot) 
  {
    if(!residualsX.empty() && !residualsY.empty() && 
        residualsX.size() == residualsY.size())
    //Need max counts to set plot scale.
    //not checking to make sure there is data since this already done
    //when the file was loaded.
    //plot the data
    m_residualsPlot->addGraph();
    m_residualsPlot->graph(0)->setData(residualsX,residualsY);
    m_residualsPlot->graph(0)->setLineStyle(QCPGraph::lsStepCenter);
    m_residualsPlot->graph(0)->setPen(QPen(Qt::red));
    m_residualsPlot->rescaleAxes();
    m_residualsPlot->replot();
    
  }
  else
  {
    //FRY: should change this to send error back to runamangaer? FRY//
    std::cout<< "OOPS! Something went wrong." << std::endl; 
  }
}

void CalibrateXRFResultsDialog::display_solver_results()
{
  m_algoField->setText(QString::fromStdString(KResultsManager::getInstance().m_XRFCalResults.m_ceresResults.m_linearSolverType));
  m_terminationField->setText(QString::fromStdString(KResultsManager::getInstance().m_XRFCalResults.m_ceresResults.m_terminationType));
  m_msgField->setText(QString::fromStdString(KResultsManager::getInstance().m_XRFCalResults.m_ceresResults.m_message));
  m_initCostField->setText(QString::number(KResultsManager::getInstance().m_XRFCalResults.m_ceresResults.m_initialCost));
  m_finalCostField->setText(QString::number(KResultsManager::getInstance().m_XRFCalResults.m_ceresResults.m_finalCost));
  m_chiSqField->setText(QString::number(KResultsManager::getInstance().m_XRFCalResults.m_ceresResults.m_reducedChiSquared));

  m_gradMaxField->setText(QString::number(KResultsManager::getInstance().m_XRFCalResults.m_ceresResults.m_gradMaxNorm));
  m_relDecreaseField->setText(QString::number(KResultsManager::getInstance().m_XRFCalResults.m_ceresResults.m_relDecrease));
  m_stepSuccessField->setText(QString::number(KResultsManager::getInstance().m_XRFCalResults.m_ceresResults.m_stepSucess));
  m_stepNormField->setText(QString::number(KResultsManager::getInstance().m_XRFCalResults.m_ceresResults.m_stepNorm));

  m_CField->setText(QString::number(KResultsManager::getInstance().m_XRFCalResults.m_C));
  m_B3Field->setText(QString::number(KResultsManager::getInstance().m_XRFCalResults.m_b3));
  m_B4Field->setText(QString::number(KResultsManager::getInstance().m_XRFCalResults.m_b4));
  m_B5Field->setText(QString::number(KResultsManager::getInstance().m_XRFCalResults.m_b5));
  m_B6Field->setText(QString::number(KResultsManager::getInstance().m_XRFCalResults.m_b6));
  m_B7Field->setText(QString::number(KResultsManager::getInstance().m_XRFCalResults.m_b7));
  m_B8Field->setText(QString::number(KResultsManager::getInstance().m_XRFCalResults.m_b8));
  m_B9Field->setText(QString::number(KResultsManager::getInstance().m_XRFCalResults.m_b9));
  m_B10Field->setText(QString::number(KResultsManager::getInstance().m_XRFCalResults.m_b10));
  m_B11Field->setText(QString::number(KResultsManager::getInstance().m_XRFCalResults.m_b11));
  m_B12Field->setText(QString::number(KResultsManager::getInstance().m_XRFCalResults.m_b12));
  m_B13Field->setText(QString::number(KResultsManager::getInstance().m_XRFCalResults.m_b13));
  m_B14Field->setText(QString::number(KResultsManager::getInstance().m_XRFCalResults.m_b14));
}

void CalibrateXRFResultsDialog::on_algoAndConv_button_clicked()
{
  const QString message = R"(
  <p><b>Possible Convergence Patterns: </b></p>
  <p></p>
  <table border="1" cellspacing="0" cellpadding="6">
  <tr>
    <th>Gradient Max Norm</th>
    <th>Step Norm</th>
    <th>Relative Decrease</th>
    <th>Likely Interpretation</th>
  </tr>
  <tr>
    <td>Small</td>
    <td>Small</td>
    <td>Any/noisy</td>
    <td>Probably converged</td>
  </tr>
  <tr>
    <td>Large</td>
    <td>Small</td>
    <td>Poor</td>
    <td>Stalled, constrained, or poorly scaled</td>
  </tr>
  <tr>
    <td>Large</td>
    <td>Large</td>
    <td>Positive</td>
    <td>Still making progress</td>
  </tr>
  <tr>
    <td>Small</td>
    <td>Large</td>
    <td>Positive</td>
    <td>Flat or weakly identified direction</td>
  </tr>
  <tr>
    <td>Large</td>
    <td>Large</td>
    <td>Negative</td>
    <td>Poor trial step or inaccurate local model</td>
  </tr>
  <tr>
    <td>Small</td>
    <td>Small</td>
    <td>Near 1</td>
    <td>Strong numerical convergence</td>
  </tr>
  </table>

  <p>See User's Manual for a more detailed description.</p>
  )";

  CustomMsgBox msgBox(message);
  int ret = msgBox.exec();
}

void CalibrateXRFResultsDialog::on_initCost_button_clicked()
{
  CustomMsgBox msgBox(tr("<p><b>The value of the objective/cost function before optimization begins.</b></p>"
                         "<p>Initial and final cost values establish the absolute baseline for the optimization, "
                         "showing exactly how much the solver improved the model from its starting assumptions."
                         "Tracking these values helps you immediately understand the optimization trajectory.</p>"
                         "<p><b>Large drop:</b> Confirms the initial guesses were highly inaccurate but the solver successfully "
                         "navigated to a far superior configuration. </p>"
                         "<p><b>Small drop:</b> Signals either an already optimal starting point, a completely frozen/constrained system, "
                         "or a solver stuck instantly in a local trap.</p>"
                         "<p>See User's Manual for a more detailed description.</p>"));
  int ret = msgBox.exec();
}

void CalibrateXRFResultsDialog::on_finalCost_button_clicked()
{
  CustomMsgBox msgBox(tr("<p><b>The value of the objective/cost function after the final iteration.</b></p>"
                         "<p>Initial and final cost values establish the absolute baseline for the optimization, "
                         "showing exactly how much the solver improved the model from its starting assumptions."
                         "Tracking these values helps you immediately understand the optimization trajectory. </p>"
                         "<p><b>Large drop:</b> Confirms the initial guesses were highly inaccurate but the solver successfully "
                         "navigated to a far superior configuration. </p>"
                         "<p><b>Small drop:</b> Signals either an already optimal starting point, a completely frozen/constrained system, "
                         "or a solver stuck instantly in a local trap.</p>"
                         "<p>See User's Manual for a more detailed description.</p>"));


  int ret = msgBox.exec();
}

void CalibrateXRFResultsDialog::on_stepSuccess_button_clicked()
{
  CustomMsgBox msgBox(tr("<p><b>Success of the last recorded iteration</b></p>"
                         "<p>May be a failed or rejected trial step, depending on why Ceres terminated</p>"
                         "<p>See User's Manual for a more detailed description.</p>"));

  int ret = msgBox.exec();
}

void CalibrateXRFResultsDialog::on_chiSq_button_clicked()
{
  const QString message = R"(
  <b>The reduced chi-squared statistic.</b>
  <table align="center" cellspacing="0" cellpadding="2">
    <tr>
      <td rowspan="2" valign="middle" style="font-size:14px;">
        &chi;<sup>2</sup><sub>&nu;</sub>
        =
        &chi;<sup>2</sup> / &nu;
        =
      </td>
  
      <td align="center" style="font-size:14px;">
        2 &times; Final Cost
      </td>
    </tr>
  
    <tr>
      <td align="center"
          style="font-size:14px; border-top:1px solid black;">
        N &minus; M
      </td>
    </tr>
  </table>
  
  <p>Where:</p>
  

    N = Number of residuals (data measurements);
    M = Number of free, optimized parameters;
    &nu; = N &minus; M = Degrees of Freedom (DoF)

  <ul>
  <li> <b>&chi;<sup>2</sup> &asymp; 1.0 Ideal Fit. </b> The residual errors match your expected measurement noise perfectly.</li>
  <li> <b>&chi;<sup>2</sup> &#8811; 1.0 Underfitting or Underestimated Noise. </b>The model fails to capture the data variance, 
  or the data is very noisy.</li>
  <li> <b>&chi;<sup>2</sup> &#8810; 1.0 Overfitting or Overestimated Noise. </b> The model is fitting random noise, 
  or the error bars are artificially inflated.</li>
  </ul>

  <p>See User's Manual for a more detailed description.</p>
  )";

  CustomMsgBox msgBox(message);
  int ret = msgBox.exec();
}

void CalibrateXRFResultsDialog::on_gradMax_button_clicked()
{
  const QString message = R"(

  <p><b>The largest absolute component of the cost gradient.</b></p>

  <ul>
  <li> <b>Large gradient max norm </b> At least one parameter direction can still substantially reduce the cost.</li>
  <li> <b>Small gradient max norm</b> The cost is locally flat with respect to every free parameter. The solver is near a stationary point.</li>
  <li> <b>Near zero </b> Necessary for a well-converged optimum, assuming the derivatives are correct.</li>
  </ul>

  <p>See User's Manual for a more detailed description.</p>
  )";

  CustomMsgBox msgBox(message);
  int ret = msgBox.exec();
}

void CalibrateXRFResultsDialog::on_relDecrease_button_clicked()
{
  const QString message = R"(

  <p><b>How closely the actual improvement matched the improvement Ceres predicted for that step.</b></p>

  <ul>
  <li> <b>Near 1 </b> The local model predicted the cost reduction accurately.</li>
  <li> <b>Between 0 and 1</b> Cost decreased, but less than predicted</li>
  <li> <b>Greater than 1 </b> Cost decreased more than predicted</li>
  <li> <b>Near 0 </b> The step produced almost none of the predicted improvement</li>
  <li> <b>Negative </b> The actual cost increased</li>
  </ul>

  <p>See User's Manual for a more detailed description.</p>
  )";

  CustomMsgBox msgBox(message);
  int ret = msgBox.exec();
}

void CalibrateXRFResultsDialog::on_stepNorm_button_clicked()
{
  const QString message = R"(

  <p><b>How far the fitted parameters moved during the latest iteration.</b></p>

  <ul>
  <li> <b>Large </b> Parameters are still changing significantly.</li>
  <li> <b>Decreasing</b> The solver is settling into a solution.</li>
  <li> <b>Very small </b> Further parameter updates are negligible.</li>
  </ul>

  <p>See User's Manual for a more detailed description.</p>
  )";

  CustomMsgBox msgBox(message);
  int ret = msgBox.exec();
}

void CalibrateXRFResultsDialog::on_accept_button_clicked()
{
  KDevLogger::getInstance().logEvent("User accepted XRF calibration results.");
  save_cal_values();
  accept();
}

void CalibrateXRFResultsDialog::save_cal_values()
{
  KFileManager::getInstance().m_workingParameterSet.m_backScattC   = KResultsManager::getInstance().m_XRFCalResults.m_C;
  KFileManager::getInstance().m_workingParameterSet.m_backScattb3  = KResultsManager::getInstance().m_XRFCalResults.m_b3;
  KFileManager::getInstance().m_workingParameterSet.m_backScattb4  = KResultsManager::getInstance().m_XRFCalResults.m_b4;
  KFileManager::getInstance().m_workingParameterSet.m_backScattb5  = KResultsManager::getInstance().m_XRFCalResults.m_b5;
  KFileManager::getInstance().m_workingParameterSet.m_backScattb6  = KResultsManager::getInstance().m_XRFCalResults.m_b6;
  KFileManager::getInstance().m_workingParameterSet.m_backScattb7  = KResultsManager::getInstance().m_XRFCalResults.m_b7;
  KFileManager::getInstance().m_workingParameterSet.m_backScattb8  = KResultsManager::getInstance().m_XRFCalResults.m_b8;
  KFileManager::getInstance().m_workingParameterSet.m_backScattb9  = KResultsManager::getInstance().m_XRFCalResults.m_b9;
  KFileManager::getInstance().m_workingParameterSet.m_backScattb10 = KResultsManager::getInstance().m_XRFCalResults.m_b10;
  KFileManager::getInstance().m_workingParameterSet.m_backScattb11 = KResultsManager::getInstance().m_XRFCalResults.m_b11;
  KFileManager::getInstance().m_workingParameterSet.m_backScattb12 = KResultsManager::getInstance().m_XRFCalResults.m_b12;
  KFileManager::getInstance().m_workingParameterSet.m_backScattb13 = KResultsManager::getInstance().m_XRFCalResults.m_b13;
  KFileManager::getInstance().m_workingParameterSet.m_backScattb14 = KResultsManager::getInstance().m_XRFCalResults.m_b14;
  KFileManager::getInstance().save_parameter_set();
  std::string message = "Saved XRF instrument calibration to " +  KFileManager::getInstance().m_workingParameterSet.m_setName;
  KDevLogger::getInstance().logEvent(message);
}
