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

package ircam.fts.client;

class BinaryProtocolDecoder {

  private static final int qInitial = 1;
  private static final int qInt0 = 2;
  private static final int qInt1 = 3;
  private static final int qInt2 = 4;
  private static final int qInt3 = 5;
  private static final int qFloat0 = 6;
  private static final int qFloat1 = 7;
  private static final int qFloat2 = 8;
  private static final int qFloat3 = 9;
  private static final int qString = 10;
  private static final int qObject0 = 11;
  private static final int qObject1 = 12;
  private static final int qObject2 = 13;
  private static final int qObject3 = 14;
  private static final int qSymbolIndex0 = 15;
  private static final int qSymbolIndex1 = 16;
  private static final int qSymbolIndex2 = 17;
  private static final int qSymbolIndex3 = 18;
  private static final int qSymbolCache0 = 19;
  private static final int qSymbolCache1 = 20;
  private static final int qSymbolCache2 = 21;
  private static final int qSymbolCache3 = 22;
  private static final int qSymbolCache4 = 23;

  private void clearAction( int input)
  {
    ival = 0;
  }

  private void shiftAction( int input)
  {
    ival = ival << 8 | input;
  }

  private void bufferClearAction( int input)
  {
    buffer.setLength( 0);
  }

  private void clearAllAction( int input)
  {
    ival = 0;
    buffer.setLength( 0);
  }

  private void bufferShiftAction( int input)
  {
    buffer.append( (char)input);
  }

  private void endIntAction( int input)
  {
    ival = ival << 8 | input;

    if (argsCount >= 2)
      args.addInt( ival);
    argsCount++;
  }

  private void endFloatAction( int input)
  {
    ival = ival << 8 | input;

    if (argsCount >= 2)
      args.addFloat( Float.intBitsToFloat(ival) );
    argsCount++;
  }

  private void endSymbolIndexAction( int input)
  {
    ival = ival << 8 | input;

    FtsSymbol s = symbolCache.get( ival);

    if (argsCount == 1)
      selector = s;
    else
      args.addSymbol( s);

    argsCount++;
  }

  private void endSymbolCacheAction( int input)
  {
    FtsSymbol s = FtsSymbol.get( buffer.toString());

    symbolCache.put( ival, s);

    if (argsCount == 1)
      selector = s;
    else
      args.addSymbol( s);

    argsCount++;
  }

  private void endStringAction( int input)
  {
    if (argsCount >= 2)
      args.addString( buffer.toString());
    argsCount++;
  }

  private void endObjectAction( int input)
  {
    ival = ival << 8 | input;

    FtsObject obj = server.getObject( ival);

    if (argsCount == 0)
      target = obj;
    else
      args.addObject( obj);

    argsCount++;
  }

  private void endMessageAction( int input)
  {
    FtsObject.invokeMessageHandler( target, selector, args);
    args.clear();
    argsCount = 0;
  }

  private int next( int state, int input)
  {
    int newState = 0;

    switch( state) {
    case 0:
      /* try to skip till end of message */
      if ( input == BinaryProtocol.END_OF_MESSAGE)
	newState = qInitial;
    break;
    case qInitial:
      if ( input == BinaryProtocol.INT)
	{
	  newState = qInt0;
	  clearAction( input);
	}
      else if ( input == BinaryProtocol.FLOAT)
	{
	  newState = qFloat0;
	  clearAction( input);
	}
      else if ( input == BinaryProtocol.SYMBOL_INDEX)
	{
	  newState = qSymbolIndex0;
	  clearAction( input);
	}
      else if ( input == BinaryProtocol.SYMBOL_CACHE)
	{
	  newState = qSymbolCache0;
	  clearAllAction( input);
	}
      else if ( input == BinaryProtocol.STRING)
	{
	  newState = qString;
	  bufferClearAction( input);
	}
      else if ( input == BinaryProtocol.OBJECT)
	{
	  newState = qObject0;
	  clearAction( input);
	}
      else if ( input == BinaryProtocol.END_OF_MESSAGE)
	{
	  newState = qInitial;
	  endMessageAction( input);
	}
      break;
    case qInt0:
      newState = qInt1;
      shiftAction( input);
      break;
    case qInt1:
      newState = qInt2;
      shiftAction( input);
      break;
    case qInt2:
      newState = qInt3;
      shiftAction( input);
      break;
    case qInt3:
      newState = qInitial;
      endIntAction( input);
      break;
    case qFloat0:
      newState = qFloat1;
      shiftAction( input);
      break;
    case qFloat1:
      newState = qFloat2;
      shiftAction( input);
      break;
    case qFloat2:
      newState = qFloat3;
      shiftAction( input);
      break;
    case qFloat3:
      newState = qInitial;
      endFloatAction( input);
      break;
    case qSymbolIndex0:
      newState = qSymbolIndex1;
      shiftAction( input);
      break;
    case qSymbolIndex1:
      newState = qSymbolIndex2;
      shiftAction( input);
      break;
    case qSymbolIndex2:
      newState = qSymbolIndex3;
      shiftAction( input);
      break;
    case qSymbolIndex3:
      newState = qInitial;
      endSymbolIndexAction( input);
      break;
    case qSymbolCache0:
      newState = qSymbolCache1;
      shiftAction( input);
      break;
    case qSymbolCache1:
      newState = qSymbolCache2;
      shiftAction( input);
      break;
    case qSymbolCache2:
      newState = qSymbolCache3;
      shiftAction( input);
      break;
    case qSymbolCache3:
      newState = qSymbolCache4;
      shiftAction( input);
      break;
    case qSymbolCache4:
      if ( input == 0)
	{
	  newState = qInitial;
	  endSymbolCacheAction( input);
	}
      else
	bufferShiftAction( input);
      break;
    case qString:
      if ( input == 0)
	{
	  newState = qInitial;
	  endStringAction( input);
	}
      else
	bufferShiftAction( input);
      break;
    case qObject0:
      newState = qObject1;
      shiftAction( input);
      break;
    case qObject1:
      newState = qObject2;
      shiftAction( input);
      break;
    case qObject2:
      newState = qObject3;
      shiftAction( input);
      break;
    case qObject3:
      newState = qInitial;
      endObjectAction( input);
      break;
    }

    return newState;
  }

  BinaryProtocolDecoder( FtsServer server)
  {
    this.server = server;
    connection = server.getConnection();

    buffer = new StringBuffer();
    args = new FtsArgs();
    argsCount = 0;

    symbolCache = new SymbolCache();

    currentState = qInitial;
  }

  void decode( byte[] data, int offset, int length) throws FtsClientException
  {
    for ( int i = offset; i < length; i++)
      {
	int c = data[i];

	if ( c < 0)
	  c += 256;

	currentState = next( currentState, c);

	if (currentState == 0)
	  throw new FtsClientException( "Invalid data in protocol : state=" + currentState + " input=" + c);
      }
  }

  private FtsServer server;
  private FtsServerConnection connection;

  private int ival;
  private StringBuffer buffer;

  private FtsObject target;
  private FtsSymbol selector;
  private FtsArgs args;
  private int argsCount;

  private int currentState;

  private SymbolCache symbolCache;
}
