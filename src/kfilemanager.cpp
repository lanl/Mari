#include "kfilemanager.h"

KFileManager::KFileManager()
{
  KDevLogger::getInstance().logEvent("Singleton KFileManager object constructed");
  m_fileCreated = false;
  read_in_parameter_set_database();
}

KFileManager::~KFileManager()
{
  KDevLogger::getInstance().logEvent("Singleton KFileManager object destructed");
}

KFileManager& KFileManager::getInstance()
{
  static KFileManager instance;
  return instance;
}


void KFileManager::send(int a_message)
{
}

void KFileManager::receive(int a_message)
{
}

int KFileManager::load_with_SpecUtils(KSpectralData& a_dataset)
{
  using namespace std;
  using namespace SpecUtils;
  SpecFile spec;
  const bool loaded = spec.load_file(a_dataset.m_filenameAndPath.c_str(), ParserType::Auto );
  if(!loaded)
  {
    KDevLogger::getInstance().logEvent("***Failed to parse spectrum: " + a_dataset.m_filename + " with SpecUtils***");
    emit errorMessage("*** Failed to load spectrum: " + QString::fromStdString(a_dataset.m_filename) + " ***");
    a_dataset.clear_data();
    return -1;
  }
  else
  {
    KDevLogger::getInstance().logEvent("Parsed spectrum: " + a_dataset.m_filename + " with SpecUtils");
  }
  const set<int>& sample_numbers = spec.sample_numbers();
  const vector<string>& detector_names = spec.detector_names();
  a_dataset.m_measurement = spec.measurement(1, detector_names[0]);
  //Need to make sure the loaded spectrum actually has data in it
  vector<double> counts;
  vector<size_t> channels;
  vector<double> energies;
  std::shared_ptr<const EnergyCalibration> eCalObject = a_dataset.m_measurement->energy_calibration();
  for(size_t i=0; i<a_dataset.m_measurement->num_gamma_channels(); ++i)
  {
    channels.push_back(i);
    counts.push_back(a_dataset.m_measurement->gamma_channel_content(i));
    double tempEnergy = eCalObject->energy_for_channel(static_cast<double>(i));
    energies.push_back(tempEnergy);
  }
  a_dataset.m_counts = counts;
  a_dataset.m_originalCounts = counts;
  a_dataset.m_channels = channels;
  a_dataset.m_energies = energies;
  float countsSum = accumulate(counts.begin(), counts.end(), 0.);
  a_dataset.m_coeffs = eCalObject->coefficients();
  //This flag is used to determine the plot state
  if(channels.size() > 0 && countsSum > 0)
  {
    a_dataset.m_isLoaded = true;
  }
  else
  {
    a_dataset.m_isLoaded = false;
  }
  //reset flag that contols file loading
  a_dataset.m_loadMe = false;
  KDevLogger::getInstance().logEvent("Spectrum: " + a_dataset.m_filename + " loaded into memory ");
  return 0;
}

const int KFileManager::boolsToInt(const bool a_bool1, const bool a_bool2, const bool a_bool3)
{
  int stateNum = a_bool1*2*2 + a_bool2*2 + a_bool3;
  return  stateNum;
}

const int KFileManager::calc_KED_state_number()
{
  const int state = boolsToInt(m_kedSampData.m_isLoaded,
                               m_kedBkgdData.m_isLoaded,
                               m_kedRefData.m_isLoaded);
  return state;
}

const int KFileManager::calc_XRF_state_number()
{
  const int state = boolsToInt(m_xrfSampData.m_isLoaded,
                               m_xrfBkgdData.m_isLoaded,
                               m_xrfRefData.m_isLoaded);
  return state;
}

int KFileManager::calc_KED_plot_number()
{
  int numKEDPlots = 0;
  if(m_kedSampData.m_isLoaded == true) numKEDPlots+=1;
  if(m_kedBkgdData.m_isLoaded == true) numKEDPlots+=1;
  if(m_kedRefData.m_isLoaded == true) numKEDPlots+=1;
  return numKEDPlots;
}

int KFileManager::calc_XRF_plot_number()
{
  int numXRFPlots = 0;
  if(m_xrfSampData.m_isLoaded == true) numXRFPlots+=1;
  if(m_xrfBkgdData.m_isLoaded == true) numXRFPlots+=1;
  if(m_xrfRefData.m_isLoaded  == true) numXRFPlots+=1;
  return numXRFPlots;
}

std::array<float,3> KFileManager::update_KED_energy_calibration(const int a_checkedID, 
                                                                const float a_peakLocActual, 
                                                                const float a_peakLocDesired)
{
  KSpectralData& specData = get_KED_spec_data_from_checkedID(a_checkedID);
  KEnergyCalibration energyCalObj;
  std::array<float,3> coeffs = energyCalObj.calibrate_from_mouse_drag(specData, a_peakLocActual, a_peakLocDesired);
  return coeffs;
}

void KFileManager::update_KED_energy_calibration(const int a_checkedID, 
                                                 const int a_param,
                                                 const float a_newLinear)
{
  KSpectralData& specData = get_KED_spec_data_from_checkedID(a_checkedID);
  KEnergyCalibration energyCalObj;
  if(a_checkedID == -1)
  {
    return;
  }
  energyCalObj.calibrate_from_box(specData, a_param, a_newLinear);
}

std::array<float,3> KFileManager::update_XRF_energy_calibration(const int a_checkedID, 
                                                                const float a_peakLocActual, 
                                                                const float a_peakLocDesired)
{
  KSpectralData& specData = get_XRF_spec_data_from_checkedID(a_checkedID);
  KEnergyCalibration energyCalObj;
  std::array<float,3> coeffs = energyCalObj.calibrate_from_mouse_drag(specData, a_peakLocActual, a_peakLocDesired);
  return coeffs;
}

void KFileManager::update_XRF_energy_calibration(const int a_checkedID, 
                                                 const int a_param,
                                                 const float a_newLinear)
{
  KSpectralData& specData = get_XRF_spec_data_from_checkedID(a_checkedID);
  KEnergyCalibration energyCalObj;
  if(a_checkedID == -1)
  {
    return;
  }
  energyCalObj.calibrate_from_box(specData, a_param, a_newLinear);
}

void KFileManager::scale_KED_y_data(const int a_checkedID, const double a_scaler)
{
  KSpectralData& specData = get_KED_spec_data_from_checkedID(a_checkedID);
  if(a_checkedID == -1)
  {
    return;
  }
  for(int i=0; i < specData.m_counts.size(); i++)
  {
    specData.m_counts.at(i) = specData.m_originalCounts.at(i)*a_scaler;
  }
}

void KFileManager::scale_XRF_y_data(const int a_checkedID, const double a_scaler)
{
  KSpectralData& specData = get_XRF_spec_data_from_checkedID(a_checkedID);
  if(a_checkedID == -1)
  {
    return;
  }
  for(int i=0; i < specData.m_counts.size(); i++)
  {
    specData.m_counts.at(i) = specData.m_originalCounts.at(i)*a_scaler;
  }
}


KSpectralData& KFileManager::get_KED_spec_data_from_checkedID(const int a_checkedID)
{
  switch(a_checkedID)
  {
    case 0:
    {
      return m_kedSampData;
    }
    case 1:
    {
      return m_kedBkgdData;
    }
    case 2:
    {
      return m_kedRefData;
    }
    default: 
    {
      return m_kedSampData;
    }
  }
}

KSpectralData& KFileManager::get_XRF_spec_data_from_checkedID(const int a_checkedID)
{
  switch(a_checkedID)
  {
    case 0:
    {
      return m_xrfSampData;
    }
    case 1:
    {
      return m_xrfBkgdData;
    }
    case 2:
    {
      return m_xrfRefData;
    }
    default: 
    {
      return m_xrfSampData;
    }
  }
}


void KFileManager::save_parameter_set()
{
  //Determine of this is a new parameter set or an edit of an existing one
  auto it = std::find_if(m_parameterDatabase.begin(), m_parameterDatabase.end(), 
      [this](const parameter_data::KParameterData& set) 
      {return set.m_setName == m_openParamSetName;});
  //The set already exists, so it's an edit. Overwrite the old one
  if (it != m_parameterDatabase.end()) 
  {
    *it = m_workingParameterSet;
    std::string message = "Saved edited parameter set: " +  it->m_setName;
    KDevLogger::getInstance().logEvent(message);
  } 
  //The set doesn't exist, so it's a new one. Add it to the database
  else
  {
    // Keep the name before we modify the vector
    const std::string newName = m_workingParameterSet.m_setName;
    m_parameterDatabase.push_back(m_workingParameterSet);
    // The element we just added is now the last one
    std::string message = "Saved new parameter set: " + newName;
    KDevLogger::getInstance().logEvent(message);
  }
  write_database_to_disk();
}

void KFileManager::write_database_to_disk()
{
  //sort the database alphabetically
  std::sort(m_parameterDatabase.begin(), m_parameterDatabase.end(),
            [](const parameter_data::KParameterData& a, 
               const parameter_data::KParameterData& b) 
            {
              return a.m_setName < b.m_setName;
            });

  //convert vector of structs to vector of jsons
  //ADL finds the to_json function
  json jsonDatabase = m_parameterDatabase;

  fs::path dir_path = "C:/Users/389692/repos/hked/HKEDApp"; //FRY NEED TO UN-HARDCODE FRY
  fs::path file_path = dir_path / "ParameterSets.json";
  std::ofstream output_file(file_path);
  if (!output_file.is_open()) 
  {
    std::string message = "Error: Could not open the ParameterSets.json file for writing.";
    KDevLogger::getInstance().logEvent(message);
    WarningMsgBox msgBox(QString::fromStdString(message));
    int ret = msgBox.exec();
    return;
  }

  //close and dump JSON.
  output_file << jsonDatabase.dump(4) << '\n';
  output_file.close();

  //check for success
  if (output_file.fail()) 
  {
    std::string message = "Error: An error occurred during the JSON file write operation";
    KDevLogger::getInstance().logEvent(message);
    WarningMsgBox msgBox(QString::fromStdString(message));
    int ret = msgBox.exec();
  } 
  else 
  {
    KDevLogger::getInstance().logEvent("Wrote JSON data to ParameterSets.json");
  }
}

void KFileManager::read_in_parameter_set_database()
{
  fs::path file_path = "C:/Users/389692/repos/hked/HKEDApp/ParameterSets.json";//xxxUNHARDCODExxx
  //make sure the file exists
  if (!fs::exists(file_path)) 
  {
    std::ofstream outFile(file_path);
    outFile << "[]" << '\n';
    outFile.close();
    m_fileCreated = true;
    KDevLogger::getInstance().logEvent("ParameterSets.json not found. File created");
    WarningMsgBox msgBox("Couldn't find ParameterSets.json. Created a blank file \n You must create a parameter set before running an analysis");
    int ret = msgBox.exec();
  } 
  //Now that the file definitely exist, we can open it
  std::ifstream input_file(file_path);
  if (!input_file.is_open()) 
  {
    KDevLogger::getInstance().logEvent("Error: Could not open ParameterSets.json");
  }
  else
  {
    KDevLogger::getInstance().logEvent("Successfully opened ParameterSets.json");
  }
  json data;
  try 
  {
    data = json::parse(input_file);
    std::cout << "Parsed successfully" << std::endl;
  } 
  catch (const json::parse_error& e) 
  {
    QString message = "Failed to parse parameter set database.";
    KDevLogger::getInstance().logEvent(e.what());
    //std::cerr << "Error byte position: " << e.byte << std::endl;
    WarningMsgBox msgBox(message);
    int ret = msgBox.exec();
    m_fileCreated = true;//prevent empty database dialog window
    return;
  }

  //Automatically convert the JSON array into a vector of KParameterData item
  //ADL finds the from_json function
  m_parameterDatabase = data.get<std::vector<parameter_data::KParameterData>>();

  //clear the vector in case we are ediitng right after new
  m_listofParamSets.clear();
  for (int i=0; i<m_parameterDatabase.size(); i++) 
  {
    m_listofParamSets.push_back(m_parameterDatabase.at(i).m_setName);
  }
}

void KFileManager::load_previous_parameter_set(const std::string &a_paramSet)
{
  //look for the parameter set name that was stored in QSettings
  //the last time the app was closed
  auto it = std::find_if(m_parameterDatabase.begin(), m_parameterDatabase.end(), 
      [a_paramSet](const parameter_data::KParameterData& set) 
      {return set.m_setName == a_paramSet;});
  
  if (it != m_parameterDatabase.end()) 
  {
    m_workingParameterSet = *it;
    std::string message = "Loaded previous parameter set: " +  it->m_setName;
    KDevLogger::getInstance().logEvent(message);
  } 
  //If it isn't found, try to find the EXAMPLE parameter set
  else 
  {
    auto it = std::find_if(m_parameterDatabase.begin(), m_parameterDatabase.end(), 
    [](const parameter_data::KParameterData& set) 
    {return set.m_setName == "EXAMPLE";});
    if (it != m_parameterDatabase.end()) 
    {
      m_workingParameterSet = *it;
      std::string message = "Couldn't find previous parameter set: " +  
                              a_paramSet + ". loaded EXAMPLE instead.";
      KDevLogger::getInstance().logEvent(message);
    }
    //If EXAMPLE isn't found...
    else
    { 
      //load the first one in the JSON file
      if(m_parameterDatabase.size()>=1)
      {
        KDevLogger::getInstance().logEvent("Error: Couldn't load previous or EXAMPLE parameter set. Loaded first one instead");
        m_workingParameterSet = m_parameterDatabase.at(0);
      }
      else
      {
        if(!m_fileCreated)
        {
          //Nothing in the database. Don't load anything and inform the user
          KDevLogger::getInstance().logEvent("Error: Couldn't load any parameter set");
          WarningMsgBox msgBox("Couldn't load a parameter set. \n You must create one before running an analysis");
          int ret = msgBox.exec();
        }
      }
    }
  }
}

void KFileManager::load_parameter_set(const std::string& a_paramSet)
{
  //look for the parameter set name in the database
  auto it = std::find_if(m_parameterDatabase.begin(), m_parameterDatabase.end(), 
      [a_paramSet](const parameter_data::KParameterData& set) 
      {return set.m_setName == a_paramSet;});

  if (it != m_parameterDatabase.end()) 
  {
    m_workingParameterSet = *it;
    std::string message = "Loaded parameter set: " +  it->m_setName;
    KDevLogger::getInstance().logEvent(message);
    m_openParamSetName = a_paramSet;
  } 
  else 
  {
    std::string message = "Error loading parameter set: " + a_paramSet;
    KDevLogger::getInstance().logEvent(message);
    WarningMsgBox msgBox(QString::fromStdString(message));
    int ret = msgBox.exec();
  }
}