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

package ircam.ftsclient;

abstract class FtsProtocolDecoder {

  FtsProtocolDecoder( FtsServer server)
  {
    this.server = server;
    connection = server.getConnection();

    inputBuffer = new byte[0x10000];
  }

  /**
   * Receive messages from FTS.
   *
   * This method does a blocking read on the connection to read bytes
   * and then calls the protocol decoder.
   * This will in turn call the installed message handlers on the objects.
   */
  void run()
  {
    try
      {
	while ( true)
	  {
	    int len = connection.read( inputBuffer, 0, inputBuffer.length);
	    decode( inputBuffer, 0, len);
	  }
      }
    catch (Exception e)
      {
	System.err.println( "FtsProtocolDecoder.run: got exception " + e);
	e.printStackTrace();
	return;
      }
  }

  abstract void decode( byte[] data, int offset, int length) throws FtsClientException;

  protected FtsServer server;

  private FtsServerConnection connection;
  private byte[] inputBuffer;
}
