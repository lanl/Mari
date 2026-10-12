#include "KSpeSpec.h"


KSpeSpec::KSpeSpec()
{
  //nothing to do
}

KSpeSpec::~KSpeSpec()
{
  //nothing to do
}


void KSpeSpec::parseAcqDate(int a_lineNum) 
{
  std::string acqDate = m_lines.at(a_lineNum);
  KFileManager::getInstance().m_KEDHist.m_acqDate = acqDate;
}

void KSpeSpec::parseLTRT(int a_lineNum) 
{
  std::string timeLine = m_lines.at(a_lineNum);
  size_t space_pos = timeLine.find(' ');
  std::string liveTime = timeLine.substr(0, space_pos);
  std::string realTime = (space_pos != std::string::npos) ? timeLine.substr(space_pos + 1) : "";
  KFileManager::getInstance().m_KEDHist.m_liveTime = std::stoi(liveTime);
  KFileManager::getInstance().m_KEDHist.m_realTime = std::stoi(realTime);
}

void KSpeSpec::parseEnergyCal(int a_lineNum) 
{
  std::string eCalLine = m_lines.at(a_lineNum);
  size_t space_pos = eCalLine.find(' ');
  std::string offset = eCalLine.substr(0, space_pos);
  std::string slope = (space_pos != std::string::npos) ? eCalLine.substr(space_pos + 1) : "";
  KFileManager::getInstance().m_KEDHist.m_offset = std::stod(offset);
  KFileManager::getInstance().m_KEDHist.m_slope = std::stod(slope);
}

void KSpeSpec::parseSpectrum(int a_lineNum) 
{
  std::string specLine = m_lines.at(a_lineNum);
  size_t space_pos = specLine.find(' ');
  int startChannel = std::stoi(specLine.substr(0, space_pos));
  int endChannel = std::stoi((space_pos != std::string::npos) ? specLine.substr(space_pos + 1) : "");
  KFileManager::getInstance().m_KEDHist.m_startChannel = startChannel;
  KFileManager::getInstance().m_KEDHist.m_endChannel = endChannel;
  int numChannels = endChannel - startChannel;
  std::vector<int> channelContents;
  for(int i=0; i<numChannels; i++)
  {
    std::string line = m_lines.at(a_lineNum + i);
    line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
    channelContents.push_back(std::stoi(line));
  }
  KFileManager::getInstance().m_KEDHist.m_binContents = channelContents;
}


int KSpeSpec::readInFile(std::string a_filename)
{
  std::ifstream inputFile(a_filename.c_str());
  std::string line;
  //First load in the line data
  if (inputFile.is_open())
  {
    while (std::getline(inputFile, line)) 
    {
      m_lines.push_back(line);
    }
  }
  else
  {
    std::cerr << "Unable to open file." << std::endl;
    return 1;
  }
  inputFile.close();
  //Now loop over data and call correct function
  std::map<std::string, std::function<void(int)>> command_handlers;
  std::string flag = "$";
  int flagLineNum = 0;
  std::string command;
  command_handlers["$DATE_MEA:"] = std::bind(&KSpeSpec::parseAcqDate, this, std::placeholders::_1);
  command_handlers["$MEAS_TIM:"] = std::bind(&KSpeSpec::parseLTRT, this, std::placeholders::_1);
  command_handlers["$DATA:"] = std::bind(&KSpeSpec::parseSpectrum, this, std::placeholders::_1);
  command_handlers["$ENER_FIT:"] = std::bind(&KSpeSpec::parseEnergyCal, this, std::placeholders::_1);
  for(const std::string line : m_lines)
  {
    flagLineNum += 1;
    if (line.find(flag) != std::string::npos) 
    {
      std::string strippedLine = line;//strip all spaces, tabs, newlines
      strippedLine.erase(std::remove_if(strippedLine.begin(), strippedLine.end(), ::isspace), strippedLine.end());
      command = strippedLine;
      // Find and execute the corresponding handler
      auto it = command_handlers.find(command);//iterator to map key
      if (it != command_handlers.end()) 
     {
       it->second(flagLineNum); // Call the function associated with the command
     } 
    }
  }
  return 0;
}

int KSpeSpec::testSpecUtils(std::string a_filename)
{
   using namespace std;
   using namespace SpecUtils;
  SpecFile spec;
   const bool loaded = spec.load_file(a_filename.c_str(), ParserType::Auto );
   if( !loaded )
     return -1;
 
   const set<int> &sample_numbers = spec.sample_numbers();
   const vector<string> &detector_names = spec.detector_names();
   
  shared_ptr<const Measurement> meas = spec.measurement(1, detector_names[0]);


   KFileManager::getInstance().m_KEDHist.m_liveTime = meas->live_time();
  KFileManager::getInstance().m_KEDHist.m_realTime = meas->real_time();

//   for( const int sample_number : sample_numbers )
//   {
//     for( const string &det_name : detector_names )
//     {
//       shared_ptr<const Measurement> meas = spec.measurement( sample_number, det_name );
//       if( !meas )
//         continue;  //Its possible every sample number will have a #Measurement for each detector
//       
//       cout << "Sample " << sample_number << " Det " << det_name << " has real time "
//            << meas->real_time() << "s, and live time: " << meas->live_time() << "s";
//       
//       if( meas->contained_neutron() )
//         cout << " with " << meas->neutron_counts_sum() << " neutrons";
// 
//       const shared_ptr<const vector<float>> &gamma_counts = meas->gamma_counts();
//       if( gamma_counts && gamma_counts->size() )
//       {
//         cout << " with " << gamma_counts->size() << " gamma channels with "
//              << meas->gamma_count_sum() << " total gammas";
//         
//         const shared_ptr<const vector<float>> &channel_energies = meas->channel_energies();
//         if( channel_energies ) //Will be null if input file didnt give a valid energy calibration
//         {
//           cout << " and energy range " << channel_energies->front() << " to "
//                << channel_energies->back() << " keV";
//         }//if( we have energy calibration info )
//       }//if( we have gamma data )
// 
//       cout << endl;
//     }//for( loop over detectors )
//   }//for( loop over sample numbers )
}
/*
int main()
 {
   SpecFile spec;
   const bool loaded = spec.load_file( "/path/to/file.n42", ParserType::Auto );
   if( !loaded )
     return -1;
 
   const set<int> &sample_numbers = spec.sample_numbers();
   const vector<string> &detector_names = spec.detector_names();
 
   for( const int sample_number : sample_numbers )
   {
     for( const string &det_name : detector_names )
     {
       shared_ptr<const Measurement> meas = spec.measurement( sample_number, det_name );
       if( !meas )
         continue;  //Its possible every sample number will have a #Measurement for each detector
       
       cout << "Sample " << sample_number << " Det " << det_name << " has real time "
            << meas->real_time() << "s, and live time: " << meas->live_time() << "s";
       
       if( meas->contained_neutron() )
         cout << " with " << meas->neutron_counts_sum() << " neutrons";
 
       const shared_ptr<const vector<float>> &gamma_counts = meas->gamma_counts();
       if( gamma_counts && gamma_counts->size() )
       {
         cout << " with " << gamma_counts->size() << " gamma channels with "
              << meas->gamma_count_sum() << " total gammas";
         
         const shared_ptr<const vector<float>> &channel_energies = meas->channel_energies();
         if( channel_energies ) //Will be null if input file didnt give a valid energy calibration
         {
           cout << " and energy range " << channel_energies->front() << " to "
                << channel_energies->back() << " keV";
         }//if( we have energy calibration info )
       }//if( we have gamma data )
 
       cout << endl;
     }//for( loop over detectors )
   }//for( loop over sample numbers )
 
   //Write all samples and detectors to a PCF file; we could remove some if we wanted.
   try
   {
     spec.write_to_file( "output.pcf", sample_numbers, detector_names, SaveSpectrumAsType::Pcf );
   }catch( std::exception &e )
   {
     cerr << "Failed to write output file: " << e.what() << endl;
     return -2;
   }//try / catch
 
   return 0;
 }//main()
 */
