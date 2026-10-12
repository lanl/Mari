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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

//Qt includes
#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QMenuBar>
#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTabWidget>
#include <QLabel>
#include <QComboBox>
#include <QDebug>
#include <QTextEdit>
#include <QVector>
#include <QPrinter>
#include <QPrintDialog>
#include <QPainter>
#include <QDoubleValidator>
//External includes
#include "qcustomplot.h"
//Project Qt includes
#include "loadspectrumdialog.h"
#include "analyzedialog.h"
#include "calibratekeddialog.h"
#include "calibratexrfdialog.h"
#include "calibratexrfresultsdialog.h"
#include "newparameterdialog.h"
#include "editparameterdialog.h"
#include "deleteparameterdialog.h"
#include "custommsgbox.h"
//Project includes
#include "kvkedplotstate.h"
#include "kvxrfplotstate.h"
#include "kfilemanager.h"
#include "kkedresults.h"
#include "kxrfresults.h"
#include "kresultssummary.h"
//C++ includes
#include <chrono>
#include <format> 
#include <iostream>

/*
  Class for applciation main window
*/

class MainWindow : public QMainWindow 
{
  Q_OBJECT // Required for Qt's meta-object system

public:
  /**
   * MainWindow constructor
   * Handles GUI widget layout
   * Provide connections to plot managers
   **/
  explicit MainWindow(QWidget *parent = nullptr);

  /**
   * MainWindow destructor
   * Deletes objects that are not automatically handled by Qt
   **/
   ~MainWindow(); 

  /**
   * Updates the SpecInfo tab data fields based on loaded spectrum
   **/
  void update_state_fields(const KSpectralData& a_dataSet,
                               NonUpdatableLabel* a_filenameField, 
                               NonUpdatableLabel* a_channelField,
                               NonUpdatableLabel* a_dateField, 
                               NonUpdatableLabel* a_LTField, 
                               NonUpdatableLabel* a_RTField);

  /**
   * Some SpecInfo Widgets are disabled until a spectrum is loaded.
   * Enables these widgets based on loaded spectrum.
   **/
  void update_widget_disabling();

  /** 
   * Updates E cal boixes after radio button toggle or
   * calibration change
   **/
  void update_ked_ecal_boxes(std::array<float,3> a_coeffs);
  void update_xrf_ecal_boxes(std::array<float,3> a_coeffs);

  /**
   * Finds and returns the top-level parent of a given widget.
   * Used in testing scenarios.
   **/
  QWidget* findUltimateParent(QWidget* a_widget);

  /**
   * Getter to a QList of all widgets in the application.
   * Returns all user created and Qt internal widgets.
   * Used in testing scenarios.
   **/
  QList<QWidget*> getWidgetList() const;

public slots:
  /**
   * Main slot connected to the loadSpectrumDoalog box.
   * When the dialog box closes, it calls this function.
   **/
  void handle_graphing();

  /**
   * Sometimes we only want to remake the KED or XRF plot and not both
   **/
  void handle_KED_graphing();
  void handle_XRF_graphing();

  /** 
   * Slot connected to plot managers.
   * When a plot manager plots a spectrum, it calls this fucntion.
   * This function then delegates the actual updating to 
   * MainWindow::update_state_fields().
   **/
  void handle_state_fields_update();

  void print_error_message(const QString& a_content);

  /**
   * Print the KED results to the KED results tab
   **/
  void print_KED_results(const KKEDResults& a_kedResults);

  /**
   * Print the XRF results to the KED results tab
   **/
  void print_XRF_results(const KXRFResults& a_xrfResults);
 
   /**
   * Print the final results to the results summary tab
   **/
  void print_results_summary(const KResultsSummary& a_resultsSummary);

  /**
   * Plot the KED curve fit
   **/
  void plot_KED_fit_curve();

  /**
   * Plot the XRF curve fit
   **/
  void plot_XRF_fit_curve();

  /**
   * If the KED instrument calibration succeded, display results in dialog box
   **/
  //void display_KED_cal_results();

  /**
   * If the XRF instrument calibration succeded, display results in dialog box
   **/
  void display_XRF_cal_results();



private slots:
  /**
   * Slot for "Open" action
   **/
  void open_file();

  /**
   * Slot for "analyze" action
   **/
  void analyze();

  /**
   * Slot for "calibrate spectrum" action
   **/
  void calibrate_spectrum();

  /**
   * Slot for "calibrate KED instrument" action
   **/
  void calibrate_KED();

  /**
   * Slot for "calibrate XRF instrument" action
   **/
  void calibrate_XRF();

  /**
   * Slot for "Save" action
   **/
  void save_file();

  /**
   * Slot for "Quit" action
   **/
  void quit_app();

  /**
   * Slot for "New Paramter set" action
   **/
  void new_parameter_set();

  /**
   * Slot for "Edit Paramter set" action
   **/
  void edit_parameter_set();

  /**
   * Slot for "Delete Paramter set" action
   **/
  void delete_parameter_set();

  /**
   * Slots for "hideKED" & "hideXRF" actions
   **/
  void hide_KED_window();
  void hide_XRF_window();

  /**
   * Toggle for setting log y-axis 
   **/
  void on_KED_log_button_toggled(bool isChecked);
  void on_XRF_log_button_toggled(bool isChecked);

  /**
   * Toggle for setting lin y-axis
   **/
  void on_KED_lin_button_toggled(bool isChecked);
  void on_XRF_lin_button_toggled(bool isChecked);

  /**
   * Toggle for setting ked results plot lin/log y-axis
   **/
  void on_KED_results_lin_button_toggled(bool isChecked);
  void on_KED_results_log_button_toggled(bool isChecked);

  /**
   * Toggle for setting XRF results plot lin/log y-axis
   **/
  void on_XRF_results_lin_button_toggled(bool isChecked);
  void on_XRF_results_log_button_toggled(bool isChecked);

  /**
   * Resets the axes and scale after zooming/scaling
   **/
  void on_KED_reset_button_clicked();
  void on_XRF_reset_button_clicked();

  /**
   * Toggles to draw or undraw a graph based on check box value
   **/
  void on_KED_samp_checkBox_toggled();
  void on_KED_bkgd_checkBox_toggled();
  void on_KED_ref_checkBox_toggled();
  void on_XRF_samp_checkBox_toggled();
  void on_XRF_bkgd_checkBox_toggled();
  void on_XRF_ref_checkBox_toggled();

  /**
   * Toggles to change graph color
   **/
  void on_KED_samp_comboBox_toggled();
  void on_KED_bkgd_comboBox_toggled();
  void on_KED_ref_comboBox_toggled();
  void on_XRF_samp_comboBox_toggled();
  void on_XRF_bkgd_comboBox_toggled();
  void on_XRF_ref_comboBox_toggled();

  /**
   * Toggles for adding/removing x-axis gridlines
   **/
  void on_KED_x_grid_checkBox_toggled();
  void on_XRF_x_grid_checkBox_toggled();

  /**
   * Toggles for adding/removing y-axis gridlines
   **/
  void on_KED_y_grid_checkBox_toggled();
  void on_XRF_y_grid_checkBox_toggled();

  /**
   * Toggles for drawing U & PU K-Edge reference lines
   **/
  void on_KED_cd109_checkBox_toggled();
  void on_KED_Pu_KEdge_checkBox_toggled();
  void on_KED_U_KEdge_checkBox_toggled();

  /**
   * Toggles for drawing U & PU x-ray reference lines
   **/
  void on_XRF_Pu_xray_checkBox_toggled();
  void on_XRF_U_xray_checkBox_toggled();
  void on_XRF_fission_checkBox_toggled();

  /**
   * Toggles the drawing of the KED fitted curve
   **/
  void on_KED_fit_curve_button_toggled();

  /**
   * Resets the axes and scale after zooming/scaling for results plots
   **/
  void on_KED_results_reset_button_clicked();
  void on_XRF_results_reset_button_clicked();

  /**
   * Toggles to turn on/off subplots
   **/
  void on_KED_results_fit_checkBox_toggled();
  void on_KED_results_continuum_checkBox_toggled();
  void on_XRF_results_fit_checkBox_toggled();
  void on_XRF_results_continuum_checkBox_toggled();

  /**
   * Toggles to change which graph E cal values are for
   **/
  void on_KED_Ecal_group_toggled(int a_id, bool a_checked);
  void on_XRF_Ecal_group_toggled(int a_id, bool a_checked);
  
  /**
   * Parse mouse events when hovering over plots.
   * Used to display x,y toolip box
   **/ 
  void on_KED_mouse_move(QMouseEvent *event);
  void on_XRF_mouse_move(QMouseEvent *event);

  /**
   * Right click and drag to change energy calibration
   **/
  void on_ked_mouse_pressed(QMouseEvent *event);
  void on_ked_mouse_released(QMouseEvent *event);
  void on_xrf_mouse_pressed(QMouseEvent *event);
  void on_xrf_mouse_released(QMouseEvent *event);

  /**
   * Displays a label for the reference line clicked
   **/
  void on_KED_item_clicked(QCPAbstractItem *item, QMouseEvent *event);
  void on_XRF_item_clicked(QCPAbstractItem *item, QMouseEvent *event);

  /**
   * Changes energy calibration to new value
   **/
  void on_KED_offsetField_return();
  void on_KED_linearField_return();
  void on_KED_quadraticField_return();
  void on_XRF_offsetField_return();
  void on_XRF_linearField_return();
  void on_XRF_quadraticField_return();

  /**
   * Handle the slider scaling logic
   **/
  void on_KED_slider_changed();
  void on_XRF_slider_changed();

  /**
   * When we toggle the samp/bkgd/ref radio buttons we need to remember
   * where the slider is for the previous spectrum
   **/
  void save_KED_slider_positons(int a_prevID);
  void save_XRF_slider_positons(int a_prevID);

  /**
   * Scale plot when user changes scale by typing in the box
   **/
  void on_KED_scaler_return();
  void on_XRF_scaler_return();

signals:
  void KED_widget_status(int  a_sampColor, int  a_bkgdColor, int  a_refColor,
                         bool a_sampDraw,  bool a_bkgdDraw,  bool a_refDraw);

  void XRF_widget_status(int  a_sampColor, int  a_bkgdColor, int  a_refColor,
                         bool a_sampDraw,  bool a_bkgdDraw,  bool a_refDraw);
protected:
  void closeEvent(QCloseEvent *event) override;

private:
  /**
   * Load user settings upon app startup
   **/
  void load_settings();

  /**
   * Save user settings on app close
   **/
  void save_settings();

  /**
   * Helper to create the menu bar
   **/
  void create_menus();

  /**
   * Helper to create menu actions
   **/
  void create_actions(); 

  /**
   * Setup the GUI for the Spectrum tab in the main tab widget.
   * Sends layout back to MainWindow().
   **/
  QWidget* layout_spectrum_tab();

  /**
   * Setup the GUI for the Results tab in the main tab widget.
   * Sends layout back to MainWindow().
   **/
  QWidget* layout_results_tab();

  /**
   * Setup the GUI for the KED Results tab in the main tab widget.
   * Sends layout back to MainWindow().
   **/
  QWidget* layout_KED_results_tab();

  /**
   * Setup the GUI for the KED Results summary tab in the KED Results tab widget.
   * Sends layout back to layout_KED_results_tab().
   **/
  QWidget* layout_KED_results_plot_tab();

    /**
   * Setup the GUI for the KED fit details tab in the KED Results widget.
   * Sends layout back to layout_KED_results_tab().
   **/
  QWidget* layout_KED_results_ceres_tab();

  /**
   * Setup the GUI for the XRF Results tab in the main tab widget.
   * Sends layout back to MainWindow().
   **/
  QWidget* layout_XRF_results_tab();

  /**
   * Setup the GUI for the XRF Results summary tab in the XRF Results tab widget.
   * Sends layout back to layout_XRF_results_tab().
   **/
  QWidget* layout_XRF_results_plot_tab();

  /**
   * Setup the GUI for the XRF fit details tab in the XRF Results widget.
   * Sends layout back to layout_XRF_results_tab().
   **/
  QWidget* layout_XRF_results_ceres_tab();

  /**
   * Setup the GUI for the Results Summary tab in the main tab widget.
   * Sends layout back to MainWindow().
   **/
  QWidget* layout_results_summary_tab();

  /**
   * Setup the GUI for the Compare tab in the main tab widget.
   * Sends layout back to MainWindow().
   **/
  QWidget* layout_compare_tab();

  /**
   * Setup the GUI for the KED Spectrum Info tab in the Spectrum page tab widget.
   * Sends layout back to layout_spectrum_tab().
   **/
  QWidget* layout_KED_specInfo_tab();

  /**
   * Setup the GUI for the XRF Spectrum Info tab in the Spectrum page tab widget.
   * Sends layout back to layout_spectrum_tab().
   **/
  QWidget* layout_XRF_specInfo_tab();

  /**
   * Setup the GUI for the KED Plot Tools tab in the Spectrum page tab widget.
   * Sends layout back to layout_spectrum_tab().
   **/
  QWidget* layout_KED_plotTools_tab();

  /**
   * Setup the GUI for the KED Plot Tools tab in the Spectrum page tab widget.
   * Sends layout back to layout_spectrum_tab().
   **/
  QWidget* layout_XRF_plotTools_tab();

  /**
   * Setup the GUI for the KED plot area.
   * Sends layout back to layout_spectrum_tab().
   **/
  QWidget* layout_KED_plotting_area();

  /**
   * Setup the GUI for the XRF plot area.
   * Sends layout back to layout_spectrum_tab().
   **/
  QWidget* layout_XRF_plotting_area();

  /**
   * Do the actual the slider scaling
   **/
  void do_KED_slider_changed(int a_sliderValue);
  void do_XRF_slider_changed(int a_sliderValue);

  /**
   * Map slider value to scale value
   **/
  double map_slider_to_scaler(int a_sliderValue);

  /**
   * Map scale value to slider value
   **/
  int map_scaler_to_slider(double a_scalerValue);

  /**
   * Reset slider after spectrum is removed
   **/
  void reset_KED_slider();

  /**
   * Reset slider after spectrum is removed
   **/
  void reset_XRF_slider();

  /**
   * Reset scaler box and memory after spectrum is removed
   **/
  //void reset_KED_scaler();

  /**
   * Reset scaler box and memory after spectrum is removed
   **/
  //void reset_XRF_scaler();

  //File menu & options
  QMenu*   fileMenu; 
  QAction* openAct; // "Open" action
  QAction* analyzeAct; // "analyze" action
  QAction* calibrateAct; // "calibrate" action
  QAction* saveAct; // "Save" action
  QAction* quitAct; // "Quit" action

  //View menu & options
  QMenu*   viewMenu; 
  QAction* hideKEDAct;
  QAction* hideXRFAct;

  //Paramter set menu & options
  QMenu*   paramMenu; 
  QAction* newParamAct;
  QAction* editParamAct;
  QAction* deleteParamAct;

  //Calibrate menu & options
  QMenu*   calibrateMenu; 
  QAction* calSpectrumAct;
  QAction* calKEDAct;
  QAction* calXRFAct;

  //Tab widget for main window. Has the Spectrum Viewer, Results Tabs
  QTabWidget* m_mainTabWidget;

  //Widgets for upper and lower portions of main spec page tab.
  //Allows us to hide ked or xrf portions if desired
  QWidget* m_specPgUpperWidget;
  QWidget* m_specPgLowerWidget;

  //Various widgets. 
  //These are only the ones that need to be accessed at some point after creation.
  QCustomPlot* m_kedPlot;
  QCustomPlot* m_xrfPlot;

  NonUpdatableLabel* m_kedSampFilenameField;
  NonUpdatableLabel* m_kedSampChannelField;
  NonUpdatableLabel* m_kedSampDateField;
  NonUpdatableLabel* m_kedSampLTField;
  NonUpdatableLabel* m_kedSampRTField;
  NonUpdatableLabel* m_kedBkgdFilenameField;
  NonUpdatableLabel* m_kedBkgdChannelField;
  NonUpdatableLabel* m_kedBkgdDateField;
  NonUpdatableLabel* m_kedBkgdLTField;
  NonUpdatableLabel* m_kedBkgdRTField;
  NonUpdatableLabel* m_kedRefFilenameField;
  NonUpdatableLabel* m_kedRefChannelField;
  NonUpdatableLabel* m_kedRefDateField;
  NonUpdatableLabel* m_kedRefLTField;
  NonUpdatableLabel* m_kedRefRTField;
  UpdatableLabel* m_kedOffsetField;
  UpdatableLabel* m_kedLinearField;
  UpdatableLabel* m_kedQuadraticField;

  NonUpdatableLabel* m_xrfSampFilenameField;
  NonUpdatableLabel* m_xrfSampChannelField;
  NonUpdatableLabel* m_xrfSampDateField;
  NonUpdatableLabel* m_xrfSampLTField;
  NonUpdatableLabel* m_xrfSampRTField;
  NonUpdatableLabel* m_xrfBkgdFilenameField;
  NonUpdatableLabel* m_xrfBkgdChannelField;
  NonUpdatableLabel* m_xrfBkgdDateField;
  NonUpdatableLabel* m_xrfBkgdLTField;
  NonUpdatableLabel* m_xrfBkgdRTField;
  NonUpdatableLabel* m_xrfRefFilenameField;
  NonUpdatableLabel* m_xrfRefChannelField;
  NonUpdatableLabel* m_xrfRefDateField;
  NonUpdatableLabel* m_xrfRefLTField;
  NonUpdatableLabel* m_xrfRefRTField;
  UpdatableLabel* m_xrfOffsetField;
  UpdatableLabel* m_xrfLinearField;
  UpdatableLabel* m_xrfQuadraticField;

  QRadioButton* m_kedLinButton;
  QRadioButton* m_kedLogButton;
  QRadioButton* m_xrfLinButton;
  QRadioButton* m_xrfLogButton;
  
  QCheckBox* m_kedSampCheckBox;
  QComboBox* m_kedSampComboBox;
  QCheckBox* m_kedBkgdCheckBox;
  QComboBox* m_kedBkgdComboBox;
  QCheckBox* m_kedRefCheckBox;
  QComboBox* m_kedRefComboBox;

  QCheckBox* m_xrfSampCheckBox;
  QComboBox* m_xrfSampComboBox;
  QCheckBox* m_xrfBkgdCheckBox;
  QComboBox* m_xrfBkgdComboBox;
  QCheckBox* m_xrfRefCheckBox;
  QComboBox* m_xrfRefComboBox;

  QCheckBox* m_kedXGridCheckBox;
  QCheckBox* m_kedYGridCheckBox;
  QCheckBox* m_xrfXGridCheckBox;
  QCheckBox* m_xrfYGridCheckBox;

  QCheckBox* m_kedCd109CheckBox;
  QCheckBox* m_kedPuKEdgeCheckBox;
  QCheckBox* m_kedUKEdgeCheckBox;

  QCheckBox* m_xrfPuXrayCheckBox;
  QCheckBox* m_xrfUXrayCheckBox;
  QCheckBox* m_xrfFissionCheckBox;

  QCheckBox* m_kedFitCheckBox;
  QSlider*   m_kedSlider;
  QSlider*   m_xrfSlider;
  UpdatableLabel* m_kedScaleField;
  UpdatableLabel* m_xrfScaleField;

  QButtonGroup* m_kedEnergyCalGroup;
  QRadioButton* m_kedSampEcalButton;
  QRadioButton* m_kedBkgdEcalButton;
  QRadioButton* m_kedRefEcalButton;
  QButtonGroup* m_xrfEnergyCalGroup;
  QRadioButton* m_xrfSampEcalButton;
  QRadioButton* m_xrfBkgdEcalButton;
  QRadioButton* m_xrfRefEcalButton;

  QCPItemStraightLine* m_cd109Line;
  QCPItemStraightLine* m_puKEdgeLine;
  QCPItemStraightLine* m_uKEdgeLine;

  QCPItemStraightLine* m_puKAlpha1Line;
  QCPItemStraightLine* m_puKAlpha2Line;
  QCPItemStraightLine* m_puKAlpha3Line;
  QCPItemStraightLine* m_puKBeta1Line;
  QCPItemStraightLine* m_puKBeta3Line;
  QCPItemStraightLine* m_puKBeta2ILine;
  QCPItemStraightLine* m_puKBeta2IILine;
  QCPItemStraightLine* m_puKBeta4ILine;
  QCPItemStraightLine* m_uKAlpha1Line;
  QCPItemStraightLine* m_uKAlpha2Line;
  QCPItemStraightLine* m_uKBeta1Line;
  QCPItemStraightLine* m_uKBeta3Line;
  QCPItemStraightLine* m_uKBeta2Line;
  QCPItemStraightLine* m_uKBeta4Line;
  QCPItemStraightLine* m_eu154_123Line;
  QCPItemStraightLine* m_eu155_105Line;

  //Results Summary Page
  NonUpdatableLabel* m_resultsSummaryDateField;
  NonUpdatableLabel* m_resultsSummaryFacilityField;
  NonUpdatableLabel* m_resultsSummaryOperatorField;
  NonUpdatableLabel* m_resultsSummaryCommentField;
  NonUpdatableLabel* m_resultsSummaryKEDSampleField;
  NonUpdatableLabel* m_resultsSummaryXRFSampleField;
  NonUpdatableLabel* m_resultsSummaryKEDBkgdField;
  NonUpdatableLabel* m_resultsSummaryXRFBkgdField;
  NonUpdatableLabel* m_resultsSummaryKEDRefField;
  NonUpdatableLabel* m_resultsSummaryXRFRefField;
  NonUpdatableLabel* m_resultsSummaryParamField;
  NonUpdatableLabel* m_resultsSummaryBkgdField;
  NonUpdatableLabel* m_resultsSummaryTempField;
  NonUpdatableLabel* m_resultsSummaryReferenceField;
  NonUpdatableLabel* m_resultsSummaryAnaTypeField;
  NonUpdatableLabel* m_resultsSummaryKEDModeField;
  NonUpdatableLabel* m_resultsSummaryXRFModeField;
  NonUpdatableLabel* m_resultsSummaryUConcenField;
  NonUpdatableLabel* m_resultsSummaryNpConcenField;
  NonUpdatableLabel* m_resultsSummaryPuConcenField;
  NonUpdatableLabel* m_resultsSummaryAmConcenField;
  NonUpdatableLabel* m_resultsSummaryCmConcenField;
  NonUpdatableLabel* m_resultsSummaryUSigmaField;
  NonUpdatableLabel* m_resultsSummaryNpSigmaField;
  NonUpdatableLabel* m_resultsSummaryPuSigmaField;
  NonUpdatableLabel* m_resultsSummaryAmSigmaField;
  NonUpdatableLabel* m_resultsSummaryCmSigmaField;
  NonUpdatableLabel* m_resultsSummaryURSDField;
  NonUpdatableLabel* m_resultsSummaryNpRSDField;
  NonUpdatableLabel* m_resultsSummaryPuRSDField;
  NonUpdatableLabel* m_resultsSummaryAmRSDField;
  NonUpdatableLabel* m_resultsSummaryCmRSDField;

  //KED Results Plot Page
  QCustomPlot* m_kedResultsPlot;
  QCustomPlot* m_kedResidualsPlot;
  QRadioButton* m_kedResultsLinButton;
  QRadioButton* m_kedResultsLogButton;
  QCheckBox* m_kedResultsFitCheckBox;
  QCheckBox* m_kedResultsContCheckBox;
  QCheckBox* m_kedResultsLASCheckBox;
  QCheckBox* m_kedResultsBSCheckBox;
  QCheckBox* m_kedResultsTailCheckBox;
  QCheckBox* m_kedResultsCoincCheckBox;
  
  //KED Results Ceres page
  NonUpdatableLabel* m_KEDFitDetailsAlgoField;
  NonUpdatableLabel* m_KEDFitDetailsTermField;
  NonUpdatableLabel* m_KEDFitDetailsMsgField;
  NonUpdatableLabel* m_KEDFitDetailsInitCostField;
  NonUpdatableLabel* m_KEDFitDetailsFinalCostField;
  NonUpdatableLabel* m_KEDFitDetailsCostDropField;
  NonUpdatableLabel* m_KEDFitDetailsResidualField;
  NonUpdatableLabel* m_KEDFitDetailsJacobianField;
  NonUpdatableLabel* m_KEDFitDetailsTotalItField;
  NonUpdatableLabel* m_KEDFitDetailsSuccItField;
  NonUpdatableLabel* m_KEDFitDetailsUnsuccItField;
  NonUpdatableLabel* m_KEDFitDetailsTotalTimeField;
  NonUpdatableLabel* m_KEDFitDetailsJacobTimeField;
  NonUpdatableLabel* m_KEDFitDetailsLinSolveTimeField;

  //XRF Results Plot Page
  QCustomPlot* m_xrfResultsPlot;
  QCustomPlot* m_xrfResidualsPlot;
  QRadioButton* m_xrfResultsLinButton;
  QRadioButton* m_xrfResultsLogButton;
  QCheckBox* m_xrfResultsFitCheckBox;
  QCheckBox* m_xrfResultsContCheckBox;
  QCheckBox* m_xrfResultsLASCheckBox;
  QCheckBox* m_xrfResultsBSCheckBox;
  QCheckBox* m_xrfResultsTailCheckBox;
  QCheckBox* m_xrfResultsCoincCheckBox;
  
  //XRF Results Ceres page
  NonUpdatableLabel* m_XRFFitDetailsAlgoField;
  NonUpdatableLabel* m_XRFFitDetailsTermField;
  NonUpdatableLabel* m_XRFFitDetailsMsgField;
  NonUpdatableLabel* m_XRFFitDetailsInitCostField;
  NonUpdatableLabel* m_XRFFitDetailsFinalCostField;
  NonUpdatableLabel* m_XRFFitDetailsCostDropField;
  NonUpdatableLabel* m_XRFFitDetailsResidualField;
  NonUpdatableLabel* m_XRFFitDetailsJacobianField;
  NonUpdatableLabel* m_XRFFitDetailsTotalItField;
  NonUpdatableLabel* m_XRFFitDetailsSuccItField;
  NonUpdatableLabel* m_XRFFitDetailsUnsuccItField;
  NonUpdatableLabel* m_XRFFitDetailsTotalTimeField;
  NonUpdatableLabel* m_XRFFitDetailsJacobTimeField;
  NonUpdatableLabel* m_XRFFitDetailsLinSolveTimeField;

  QTextEdit* m_summaryTextEdit;//get rid of?
  
  //flags for controlling plot visibilty
  bool kedIsVisible = true;
  bool xrfIsVisible = true;

  // values for the right-click Energy Calibration feature
  float m_peakLocActual = 0.;
  float m_peakLocDesired = 0.;

  //Store the KED curve fit data
  KKEDResults m_kedResults;
  int m_numKEDPlots;

  //Store the XRF curve fit data
  KXRFResults m_xrfResults;
  int m_numXRFPlots;

  //store which radio button is checked
  int m_kedEnergyCalGroupPrevID = -1;
  int m_xrfEnergyCalGroupPrevID = -1;
  int m_kedEnergyCalGroupCurrentID = -1;
  int m_xrfEnergyCalGroupCurrentID = -1;

  //store the slider positon for each plot
  int m_kedSampSliderPosition = 500;
  int m_kedBkgdSliderPosition = 500;
  int m_kedRefSliderPosition  = 500;
  int m_xrfSampSliderPosition = 500;
  int m_xrfBkgdSliderPosition = 500;
  int m_xrfRefSliderPosition  = 500;

  //flag to store slider scaling singal source
  //true = slider
  //false = QLineEdit box
  bool m_kedScaleSignalSource = true;
  bool m_xrfScaleSignalSource = true;
};

#endif // MAINWINDOW_H

/*
Primary GUI color: #006b75  //teal

clicked of light gray: #343a40 //dark charcoal
accents: #b84e41 //rust
Primary text
             color: #005975;    //blue
             font-weight: bold; 
             font-size: 13px;

background color: #f8f9fa //very light gray

border radius: 8px
*/
