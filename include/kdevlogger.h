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

#ifndef KDEVLOGGER_H
#define KDEVLOGGER_H

//Project includes
//External includes
//C++ includes
#include <iostream>
#include <fstream> 
#include <ctime>

class KDevLogger
{

public:
  /**
   * Global access getter for the single instance
   * Meyer's Singleton
   **/
  static KDevLogger& getInstance();

  /**
   * Need to delete the copy constructor and assignment operators
   * to prevent copying
   **/
  KDevLogger(const KDevLogger&) = delete;
  KDevLogger& operator=(const KDevLogger&) = delete;

  /**
   * Primary logging method
   **/
  void logEvent(std::string a_event);

private:
/**
 * Default constructor.
 **/
  KDevLogger();

/**
 * Default destructor.
 **/
  ~KDevLogger();

  std::ofstream m_logFile;

};

#endif 
