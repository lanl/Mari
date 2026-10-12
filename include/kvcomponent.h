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

#ifndef KVCOMPONENT_H
#define KVCOMPONENT_H

//Includes to be propagated to all manager and action classes
// C++ includes
#include <iostream>
//Project includes
#include "kdevlogger.h"
#include "kmath.h"

/*
Fix for JSON-Ceres-Windows compatibility issues 
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
#include "ceres/ceres.h"

/**
 * Abstract base class for all components.
 * All component object memory handled using RAII.
 **/
class KVComponent 
{


public:
  KVComponent();
  ~KVComponent();
  /** 
   * Intercomponent messaging system
   **/
  virtual void send(int a_message) = 0;
  virtual void receive(int a_message) = 0;

private:

};

#endif 