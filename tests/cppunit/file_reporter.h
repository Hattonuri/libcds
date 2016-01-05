/*
    This file is a part of libcds - Concurrent Data Structures library

    (C) Copyright Maxim Khizhinsky (libcds.dev@gmail.com) 2006-2016

    Source code repo: http://github.com/khizmax/libcds/
    Download: http://sourceforge.net/projects/libcds/files/
    
    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this
      list of conditions and the following disclaimer.

    * Redistributions in binary form must reproduce the above copyright notice,
      this list of conditions and the following disclaimer in the documentation
      and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.     
*/

/*
 * Copyright (c) 2003, 2004
 * Zdenek Nemec
 *
 * This material is provided "as is", with absolutely no warranty expressed
 * or implied. Any use is at your own risk.
 *
 * Permission to use or copy this software for any purpose is hereby granted
 * without fee, provided the above notices are retained on all copies.
 * Permission to modify the code and to distribute modified code is granted,
 * provided the above notices are retained, and a notice that the code was
 * modified is included with the above copyright notice.
 *
 */

/* $Id$ */

#ifndef CDS_CPPUNIT_MINIFILEREPORTERINTERFACE_H_
#define CDS_CPPUNIT_MINIFILEREPORTERINTERFACE_H_

#include <stdio.h>

#include <cds/os/timer.h>

namespace CppUnitMini {
    typedef cds::OS::Timer    Timer;
    //
    // CppUnit mini file(stream) reporter
    //
    class FileReporter : public CppUnitMini::Reporter {
    private:
      FileReporter(const FileReporter&);
      FileReporter& operator=(const FileReporter&);
    public:
      // reporting to stderr
      explicit FileReporter(bool doMonitor = false):
          m_numErrors(0), m_numIgnored(0), m_numExplicit(0), m_numTests(0), _myStream(false),
          m_failed(false), m_doMonitor(doMonitor)
      { _file = stderr; }

      // reporting to the file with the given name
      explicit FileReporter(const char* file, bool doMonitor = false):
          m_numErrors(0), m_numIgnored(0), m_numExplicit(0), m_numTests(0), _myStream(true),
          m_failed(false), m_doMonitor(doMonitor)
      {
    #ifndef _STLP_USE_SAFE_STRING_FUNCTIONS
        _file = fopen(file, "w");
    #else
        fopen_s(&_file, file, "w");
    #endif
      }

      // reporting to the given file
      explicit FileReporter(FILE* stream, bool doMonitor = false):
          m_numErrors(0), m_numIgnored(0), m_numExplicit(0), m_numTests(0), _myStream(false),
          m_failed(false), m_doMonitor(doMonitor)
      { _file = stream; }

      virtual ~FileReporter() {
        if (_myStream)
          fclose(_file);
        else
          fflush(_file);
      }

      virtual void error(const char *in_macroName, const char *in_macro, const char *in_file, int in_line) {
        // Error might be called several times between 2 progress calls, we shouldn't however consider
        // that a test failed twice so we simply keep the info that test failed, number of failed tests
        // is computed later in end method.
        m_failed = true;
        fprintf(_file, "\n\n%s(%d) : %s(%s);", in_file, in_line, in_macroName, in_macro);
        fflush(_file);
      }

      virtual void message( const char *msg )
      { fprintf(_file, "\n\t%s", msg ); fflush(_file); }

      virtual void progress(const char *in_className, const char *in_shortTestName, bool ignored, bool explicitTest) {
        if (m_doMonitor)
          m_testTimer.reset();

        ++m_numTests;
        m_failed = false;
        if (ignored)
          ++m_numIgnored;
        fprintf(_file, "%s::%s", in_className, in_shortTestName);
        if (ignored) {
          const char *ignoredReason;
          if (explicitTest) {
            ++m_numExplicit;
            ignoredReason = " EXPLICIT";
          }
          else
            ignoredReason = " IGNORED";

          fprintf(_file, "%s", ignoredReason);
        }
      }

      virtual void end() {
        if (m_doMonitor) {
          fprintf(_file, " %f msec", m_testTimer.duration());
        }
        if (m_failed) {
          ++m_numErrors;
        }
        fprintf(_file, "\n");
        fflush(_file);
      }

      virtual void printSummary() {
        if (m_numErrors > 0) {
          fprintf(_file, "\nThere were errors! %d of %d tests", m_numErrors, m_numTests);
        }
        else {
          fprintf(_file, "\nOK %d tests", m_numTests);
        }

        if (m_numIgnored > 0) {
          fprintf(_file, ", %d ignored", m_numIgnored);
        }

        if (m_numExplicit > 0) {
          fprintf(_file, " (%d explicit)", m_numExplicit);
        }

        fprintf(_file, "\n\n");
        fflush(_file);
      }
    private:
      int m_numErrors;
      int m_numIgnored;
      int m_numExplicit;
      int m_numTests;
      // flag whether we own '_file' and are thus responsible for releasing it in the destructor
      bool  _myStream;
      bool m_failed;
      bool m_doMonitor;
      Timer m_testTimer;
      FILE* _file;
    };
}
#endif // #ifndef CDS_CPPUNIT_MINIFILEREPORTERINTERFACE_H_
