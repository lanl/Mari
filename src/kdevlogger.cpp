#include "kdevlogger.h"


KDevLogger::KDevLogger() : m_logFile("KDeveloperLog.txt") 
{
  m_logFile << "                    DEVELOPER'S LOG FILE" << std::endl;
  m_logFile << "This file is intended for general debugging." << std::endl;
  m_logFile << "All user actions, class object construction/destruction," << std::endl;
  m_logFile << "and internal messages are printed here." << std::endl;
  m_logFile << "For question's concerning this file, please contact Dr. Tyler Nagel: tnagel@lanl.gov" << std::endl;
  m_logFile << std::endl;
  std::time_t clockTime = std::time(nullptr);
  if(clockTime!=-1)
  {
    m_logFile << "File created on: " << std::asctime(std::localtime(&clockTime)) << std::endl;
  }
  m_logFile << "--------------------------------------------------------------------------------" << std::endl;
  logEvent("Singleton KDevLogger object constructed");
}

KDevLogger::~KDevLogger()
{
  logEvent("Singleton KDevLogger object destructed");
  m_logFile.close();
}


KDevLogger& KDevLogger::getInstance()
{
  static KDevLogger instance;
  return instance;
}

void KDevLogger::logEvent(std::string a_event)
{
  if(m_logFile.is_open())
  {
    m_logFile << a_event << std::endl;
  }
  //!: Comment out this line prior to release.
  std::cout << a_event <<std::endl;
}

