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

#include <stdlib.h>
#include <stdio.h>
#include <iostream.h>
#include <unistd.h>
#include <signal.h>
#include <fts/ftsclient.h>

class ReceiveCallback1 : public FtsCallback {
public:
  virtual void invoke( FtsArgs *args);
  virtual void invoke( const char *selector, FtsArgs *args) {}
};

void ReceiveCallback1::invoke( FtsArgs *args)
{
  if (args->isInt( 0))
    cout << "callback " << args->getInt( 0) << endl;
}

class ReceiveCallback2 : public FtsCallback {
public:
  virtual void invoke( FtsArgs *args) {}
  virtual void invoke( const char *selector, FtsArgs *args);
};

void ReceiveCallback2::invoke( const char *selector, FtsArgs *args)
{
  cout << "callback 2: selector = " << selector << " args = " << *args << endl;
}

static FtsServer *server;
static FtsObject *o1, *o2;

static void clean_exit( int sig)
{
  if (sig)
    cerr << "Exiting on signal " << sig << endl;

  server->shutdown();
  server->disconnect();

  delete o1;
  delete o2;
  delete server;

  exit( 1);
}

main( int ac, char **av)
{
  FtsProcess *fts;

  signal( SIGINT, clean_exit);
  signal( SIGTERM, clean_exit);

  try
    {
      if (ac < 2)
	{
	  fprintf( stderr, "usage: %s [--pipe|--socket] patch\n", av[0]);
	  return -1;
	}

      int pipe = !strcmp( av[1], "--pipe");

      cerr << "Using a " << ((pipe) ? "pipe" : "socket") << " connection" << endl;

      FtsArgs args;

//        args.add( "-e");
//        args.add( "gdb");
//        args.add( "-exec");
//        args.add( "/u/worksta/dechelle/projects/jmax/3.0.0/jmax-install/bin/fts");
//        args.add( "-x");
//        args.add( "/u/worksta/dechelle/.gdbinit-fts");
//        fts = new FtsProcess( "xterm", args);

      args.add( "--no-watchdog");
      if (pipe)
	args.add( "--stdio");

      fts = new FtsProcess( "/u/worksta/schnell/projects/jmax-install/bin/fts", args);

      FtsServerConnection *connection;
      if (pipe)
	connection = new FtsPipeConnection( fts);
      else
	connection = new FtsSocketConnection();

      server = new FtsServer( connection);

//        sleep(10);
      cout << "loading patch" << endl;

      // load a patch
      args.clear();
      args.add( av[2]);
      server->getRoot()->send( "load_jmax_file", args);

      args.clear();
      args.add( "BBB");
      args.add( 1);
      o1 = new FtsObject( server, 0, "client_controller", args);
      o1->install( "int", new ReceiveCallback1());
      o1->install( new ReceiveCallback2());

      args.clear();
      args.add( "LLL");
      o2 = new FtsObject( server, 0, "client_controller", args);
      o2->install( "int", new ReceiveCallback1());
      o2->install( new ReceiveCallback2());

//        for (;;)
//  	{
	  int i;

//  	  cout << "Value ?" << endl;

	  cin >> i;
//  	  if ( cin.eof() )
//  	    break;

      char c;
      cin >> c;

      server->shutdown();
      server->wait();
    }
  catch( FtsClientException e)
    {
      e.print( cerr);
      cerr << endl;
    }

  exit( 0);
}
