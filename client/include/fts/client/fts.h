/*
 * FTS client library
 * Copyright (C) 2001 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

#include <fts/client/types.h>

/**
 * An instance of class Fts represents a running FTS that
 * can execute either in a thread, in a process on in a 
 * remote process started by rsh or ssh.
 *
 * This class does not directly link to the FTS dynamic library,
 * but uses the dynamic linker, so that you can link a client
 * with the FTS client library (this one, libftsclient.so or 
 * FTSCLIENT.DLL) without having installed the FTS library (FTS.DLL
 * or libfts.so)
 */
class FTSCLIENT_API Fts {
 public:
  virtual ~Fts() {}
  virtual void run( FtsArgs &args) throw( FtsClientException) = 0;
  void run() throw( FtsClientException) { FtsArgs args; run( args); }
};

class FTSCLIENT_API FtsProcess : public Fts {
 public:
  FtsProcess( const char *path = 0);
  virtual ~FtsProcess() {}
  virtual void run( FtsArgs &args) throw( FtsClientException);

  pipe_t getInputPipe() {
    return _in;
  }

  pipe_t getOutputPipe() {
    return _out;
  }

 private:
  void findDefaultPath() throw( FtsClientException);

  const char *_path;
  pipe_t _in;
  pipe_t _out;

#ifndef WIN32
  int _childPid;
#endif
};

class FTSCLIENT_API FtsPlugin : public Fts {
 public:
  FtsPlugin();
  virtual ~FtsPlugin();
  virtual void run( int argc, const char **argv) throw( FtsClientException);

 private:

#ifdef WIN32
  static DWORD WINAPI main(LPVOID data);
#else
  static void* main(void* data);
#endif

  /* utility function to load the fts library */
  static library_t openLibrary(char* name);
  static void closeLibrary(library_t lib);
  static library_symbol_t getSymbol(library_t lib, char* name);

  void (*init_function)(int argc, char **argv);
  void (*run_function)(void);
  void (*halt_function)(void);

  library_t library;
  thread_t thread;
  int argc;
  char **argv;
};

