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
};

class FTSCLIENT_API FtsProcess : public Fts {
 public:
  FtsProcess() throw( FtsClientException) {
    FtsArgs args;

    init( 0, args);
  }

  FtsProcess( const char *path) throw( FtsClientException) {
    FtsArgs args;

    init( path, args);
  }

  FtsProcess( FtsArgs &args) throw( FtsClientException) {
    init( 0, args);
  }

  FtsProcess( const char *path, FtsArgs &args) throw( FtsClientException) {
    init( path, args);
  }

  pipe_t getInputPipe() {
    return _in;
  }

  pipe_t getOutputPipe() {
    return _out;
  }

 private:
  void init( const char *path, FtsArgs &args) throw( FtsClientException);
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
  FtsPlugin() throw( FtsClientException) {
    FtsArgs args;

    init( args);
  }
    
  FtsPlugin( FtsArgs &args) throw( FtsClientException) {
    init( args);
  }

  ~FtsPlugin();

 private:

  void init( FtsArgs &args) throw( FtsClientException);

#ifdef WIN32
  static DWORD WINAPI main(LPVOID data);
#else
  static void* main(void* data);
#endif

  /* utility function to load the fts library */
  static library_t openLibrary(char* name);
  static void closeLibrary(library_t lib);
  static library_symbol_t getSymbol(library_t lib, char* name);

  void (*_init_function)(int argc, const char **argv);
  void (*_run_function)(void);
  void (*_halt_function)(void);

  library_t _library;
  client_thread_t _thread;
  int _argc;
  const char **_argv;
};

