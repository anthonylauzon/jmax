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

#include <iostream>
#include <typeinfo>
#include <fts/ftsclient.h>

using namespace ircam::fts::client;


class ConsoleStreamHandler: public FtsMessageHandler
{
public:
    virtual void invoke(FtsObject *obj, const FtsArgs& args)
	{
	    std::cout << " Console Stream : " 
		      << args.getString(0) 
		      << std::endl;
	}    
};



int main( int ac, char **av)
{
  FtsSocketConnection *connection;

  try
    {
      connection = new FtsSocketConnection();
    }
  catch( FtsClientException e)
    {
	std::cerr << e << std::endl;
    }

  FtsObject* rootPatcher = new FtsObject(connection, NULL, 0);
  connection->putObject(0, rootPatcher);
  
//   FtsObject* clientPatcher = new FtsObject(connection, rootPatcher, 1);
//   connection->putObject(1, clientPatcher);

//   FtsObject* console_stream = new FtsObject(connection, clientPatcher, "console_stream");
  FtsObject* console_stream = new FtsObject(connection, rootPatcher, "console_stream");

  ConsoleStreamHandler* handler = new ConsoleStreamHandler();

  FtsObject::registerMessageHandler(typeid(*console_stream), "print_line", handler);
  
  console_stream->send("set_default");
  int c;
  std::cin >> c;

  delete handler;
  delete console_stream;
  delete rootPatcher;
  delete connection;


  std::exit( 0);

  return 0;
}
