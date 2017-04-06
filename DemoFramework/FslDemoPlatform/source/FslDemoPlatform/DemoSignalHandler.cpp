/****************************************************************************************************************************************************
* Copyright (c) 2014 Freescale Semiconductor, Inc.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*    * Redistributions of source code must retain the above copyright notice,
*      this list of conditions and the following disclaimer.
*
*    * Redistributions in binary form must reproduce the above copyright notice,
*      this list of conditions and the following disclaimer in the documentation
*      and/or other materials provided with the distribution.
*
*    * Neither the name of the Freescale Semiconductor, Inc. nor the names of
*      its contributors may be used to endorse or promote products derived from
*      this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
* INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
* OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
****************************************************************************************************************************************************/


#include "DemoSignalHandler.hpp"
#include <cstdlib>
#if defined(__QNXNTO__)
#include <signal.h>
#else
#include <csignal>
#endif

namespace
{
  volatile sig_atomic_t gTerminationRequested = 0;
}

namespace Fsl {
namespace DemoFramework
{
  bool HasExitRequest()
  {
    return gTerminationRequested != 0;
  }


  void SetExitRequest()
  {
    gTerminationRequested = true;
  }
}
}

// From late c11 standard draft
// - The functions in the standard library are not guaranteed to be reentrant and may modify
//   objects with static or thread storage duration (188)
//
// 188) a signal handler cannot, in general, call standard library functions.

// If the signal occurs other than as the result of calling the abort or raise function, the
// behavior is undefined if the signal handler refers to any object with static or thread
// storage duration that is not a lock - free atomic object other than by assigning a value to an
// object declared as volatile sig_atomic_t, or the signal handler calls any function
// in the standard library other than the abort function, the _Exit function, the
// quick_exit function, or the signal function with the first argument equal to the
// signal number corresponding to the signal that caused the invocation of the handler.
// Furthermore, if such a call to the signal function results in a SIG_ERR return, the
// value of errno is indeterminate (252).
//
// Undefined behavior:
// - A signal occurs other than as the result of calling the abort or raise function, and
//   the signal handler refers to an object with static or thread storage duration that is not a
//   lock-free atomic object other than by assigning a value to an object declared as
//   volatile sig_atomic_t, or calls any function in the standard library other
//   than the abort function, the _Exit function, the quick_exit function, or the
//   signal function(for the same signal number) (7.14.1.1).
// - A signal is generated by an asynchronous signal handler (7.14.1.1).
// - The signal function is used in a multi-threaded program (7.14.1.1).
//

// 252) If any signal is generated by an asynchronous signal handler, the behavior is undefined.

// Forward references: the abort function (7.22.4.1), the exit function (7.22.4.4), the
// _Exit function(7.22.4.5), the quick_exit function(7.22.4.7)

// From late c++11 standard draft:

// When the processing of the abstract machine is interrupted by receipt of a signal, the values of objects which
// are neither
// - of type volatile std::sig_atomic_t nor
// - lock - free atomic objects(29.4)
// are unspecified during the execution of the signal handler, and the value of any object not in either of these
// two categories that is modified by the handler becomes undefined
//
// - All signal handlers shall have C linkage.
//
// - A POF (plain old function) is a function that uses only features from this common subset,
//   and that does not directly or indirectly use any function that is not a POF, except that it
//   may use functions defined in Clause 29 that are not member functions
//
// - A POF that could be used as a signal handler in a conforming C program does not produce
//   undefined behavior when used as a signal handler in a C++ program. The behavior of any other
//   function used as a signal handler in a C++ program is implementation defined (226)
//
// 226) In particular, a signal handler using exception handling is very likely to have problems.
//      Also, invoking std::exit may cause destruction of objects, including those of the
//      standard library implementation, which, in general, yields undefined behavior in
//      a signal handler(see 1.9)
extern "C" void DemoFrameworkSignalHandler(int /*signum*/)
{
  // WARNING: code inside this should more or less behave as if you are creating a interrupt handler
  //          so be EXTREMELY careful here!!!
  gTerminationRequested = 1;
}