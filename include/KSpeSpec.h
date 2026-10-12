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

#ifndef KSPESPEC_H
#define KSPESPEC_H

//Project includes
#include "kfilemanager.h"
#include "SpecUtils/SpecFile.h"
//C++ includes
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>

class KSpeSpec
{
public:
  KSpeSpec();
  ~KSpeSpec();


void parseAcqDate (int a_lineNum);
void parseLTRT (int a_lineNum);
void parseEnergyCal (int a_lineNum);
void parseSpectrum (int a_lineNum);
int readInFile(std::string a_filename);

int testSpecUtils(std::string a_filename);

private:
  std::vector<std::string> m_lines;

};

#endif 