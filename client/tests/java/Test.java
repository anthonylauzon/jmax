//
// FTS client library
// Copyright (C) 2001 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

import ircam.ftsclient.*;
import java.io.*;

public class Test {

  class MyCallback implements FtsMessageHandler {
    public void invoke( FtsObject obj, FtsArgs args)
    {
      if (argc == 1 && args.isInt(0))
	System.out.println( "Callback (1) " + args.getInt( 0));
    }
  }

  Test( String option)
  {
    try
      {
	String[] argv = new String[10];
	int argc = 0;
	boolean pipe = option.equals( "--pipe");

	System.err.println( "Using a " + ((pipe) ? "pipe" : "socket") + " connection");

	argv[argc++] = "/u/worksta/dechelle/projects/jmax/3.0.0/jmax-install/bin/fts";	
	argv[argc++] = "--no-watchdog";
	if (pipe)
	  argv[argc++] = "--stdio";

	FtsProcess fts = new FtsProcess( argc, argv);

	FtsServerConnection connection;

	if (pipe)
	  connection = new FtsPipeConnection( fts);
	else
	  connection = new FtsSocketConnection();

	FtsServer server = new FtsServer( connection);

	FtsArgs args = new FtsArgs();

	args.clear();
	args.add( "/u/worksta/dechelle/projects/jmax/3.0.0/jmax/client/tests/patches/test-bus-label.jmax");
	server.getRoot().send( "load_jmax_file", args);

	args.clear();
	args.add( "BBB");
	args.add( 1);
	FtsObject o1 = new FtsObject( server, null, "client_controller", args);

	args.clear();
	args.add( "LLL");
	FtsObject o2 = new FtsObject( server, null, "client_controller", args);

	MyCallback mc = new MyCallback();

	FtsObject.registerMessageHandler( o1.getClass(), "int", mc);
	FtsObject.registerMessageHandler( o1.getClass(), mc);

	int count = 42;

	while( true)
	  {
	    o1.send( count++);

	    Thread.sleep( 3000);
	  }
      }
    catch( Exception e)
      {
	e.printStackTrace();
      }
  }

  public static void main( String args[])
  {
    new Test( args[0]);
  }
}
