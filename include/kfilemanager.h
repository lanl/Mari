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

#ifndef KFILEMANAGER_H
#define KFILEMANAGER_H

//Qt includes
#include <QtCore/QObject>
#include <QDebug>
//Project includes
#include "kvmanager.h"
#include "kenergycalibration.h"
#include "kspectraldata.h"
#include "kparameterdata.h"
#include "custommsgbox.h"
//External includes
#include "SpecUtils/SpecFile.h"
#include "SpecUtils/EnergyCalibration.h"
/*
Fix for JSON-Windows compatibility issues 
If any TU includes things like <Windows.h>, <math.h>, or some third-party 
header that drags them in, MSVC/CRT compatibility macros can leak into 
your compile and break std::isinf. 
The cleanest fix is to undef them in a common header that is included early
*/
#include <cmath>
#ifdef isinf
#undef isinf
#endif
#ifdef isnan
#undef isnan
#endif
#include <nlohmann/json.hpp>
//C++ includes
#include <memory>
#include <numeric>
#include <algorithm>

namespace fs = std::filesystem;
using json = nlohmann::json;

class KFileManager : public QObject, public KVManager
{

  Q_OBJECT //call Q_OBJECT meta object macro
  
public:
  /**
   * Global access getter for the single instance
   * Meyer's Singleton
   **/
  static KFileManager& getInstance();

  /**
   * Need to delete the copy constructor and assignment operators
   * to prevent copying
   **/
  KFileManager(const KFileManager&) = delete;
  KFileManager& operator=(const KFileManager&) = delete;

  /**
   * Obey abstract base class contract and override pure virtual functions
   **/
  virtual void send(int a_message) override;
  virtual void receive(int a_message) override;

  /**
   * loads in a spectrum using the SpecUtils library
   **/
  int load_with_SpecUtils(KSpectralData& a_dataset);

  /**
   * Intrinsic state number is determined from the m_isLoaded flags
   * bool1 = samp, bool2 = bkgd, bool3 = ref
   * 000 = none;        state = 0
   * 100 = samp only;   state = 4
   * 010 = bkgd only;   state = 2
   * 001 = ref only;    state = 1
   * 110 = samp + bkgd; state = 6
   * 101 = samp + ref;  state = 5
   * 011 = bkgd + ref;  state = 3
   * 111 = all;         state = 7
   * State can be changed later to affect the plot.
   * This will not affect the intrinsic state number, which can be easily recalled
   **/
  const int boolsToInt(const bool a_bool1, const bool a_bool2, const bool a_bool3);

  /**
   * Convert KED state bools to KED state integer.
   * Does SUM_{i=0}(bool_i*2^i)
   **/
  const int calc_KED_state_number();

  /**
   * Convert XRF state bools to XRF state integer.
   * Uses Does SUM_{i=0}(bool_i*2^i)
   **/
  const int calc_XRF_state_number();

  /**
   * Calc number of KED plots in memory. Different from state number
   **/
  int calc_KED_plot_number();

  /**
   * Calc number of XRF plots in memory. Different from state number
   **/
  int calc_XRF_plot_number();

  /**
   * Updates spectrum energy calibration when user right-clicks and
   * drags on the plot
   **/
  std::array<float,3> update_KED_energy_calibration(const int a_checkedID, 
                                                    const float a_peakLocActual, 
                                                    const float a_peakLocDesired);

  /**
   * Overload. 
   * Updates spectrum energy calibration when user changes value
   * in one of QLineEdit ECal boxes
   **/
  void update_KED_energy_calibration(const int a_checkedID, 
                                     const int a_param,
                                     const float a_newLinear);

  /**
   * Updates spectrum energy calibration when user right-clicks and
   * drags on the plot
   **/
  std::array<float,3> update_XRF_energy_calibration(const int a_checkedID, 
                                                    const float a_peakLocActual, 
                                                    const float a_peakLocDesired);

  /**
   * Overload. 
   * Updates spectrum energy calibration when user changes value
   * in one of QLineEdit ECal boxes
   **/
  void update_XRF_energy_calibration(const int a_checkedID, 
                                     const int a_param,
                                     const float a_newLinear);

  /**
   * Scales the y data based on slider position
   **/
  void scale_KED_y_data(const int a_checkedID, const double a_scaler);
  void scale_XRF_y_data(const int a_checkedID, const double a_scaler);
  /**
   * Returns KSpectralData based on which ECal radio button is selected
   **/
  KSpectralData& get_KED_spec_data_from_checkedID(const int a_checkedID);
  KSpectralData& get_XRF_spec_data_from_checkedID(const int a_checkedID);

  /**
   * Saves the working parameter set data to the database
   **/
  void save_parameter_set();

  /**
   * Write thhe edited paramter set database to the JSON file
   **/
  void write_database_to_disk();

  /**
   * On app startup, reads in the .json file to determine what 
   * parameter sets exit. 
   * Waits until a parameter set is selected via analyze combo box or 
   * param edit combo box to read data into m_workingParameterSet.
   * */
  void read_in_parameter_set_database();

  /**
   * Loads the prevoius session working parameter set from QSettings.
   **/
  void load_previous_parameter_set(const std::string &a_paramSet);

  /**
   * Loads a parameter set from the database.
   **/
  void load_parameter_set(const std::string &a_paramSet);

  //Uploaded spectrum data in use
  KSpectralData m_kedSampData;
  KSpectralData m_kedBkgdData;
  KSpectralData m_kedRefData;
  KSpectralData m_xrfSampData;
  KSpectralData m_xrfBkgdData;
  KSpectralData m_xrfRefData;

  //parameter set in use
  parameter_data::KParameterData m_workingParameterSet;
  //Stores the original name of parameter set that was opened.
  std::string m_openParamSetName;
  //The parameter sets in the .json database
  std::vector<std::string> m_listofParamSets;

  std::vector<parameter_data::KParameterData> m_parameterDatabase;

private:
/**
 * Default constructor.
 **/
  KFileManager();

/**
 * Default destructor.
 **/
  ~KFileManager();

  bool m_fileCreated;

signals:
  void errorMessage(const QString& a_content);

};

#endif 
