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

  private static final int qInt0 = 10;
  private static final int qInt1 = 11;
  private static final int qInt2 = 12;
  private static final int qInt3 = 13;

  private static final int qFloat0 = 20;
  private static final int qFloat1 = 21;
  private static final int qFloat2 = 22;
  private static final int qFloat3 = 23;
  private static final int qFloat4 = 24;
  private static final int qFloat5 = 25;
  private static final int qFloat6 = 26;
  private static final int qFloat7 = 27;

  private static final int qString = 30;

  private static final int qObject0 = 40;
  private static final int qObject1 = 41;
  private static final int qObject2 = 42;
  private static final int qObject3 = 43;

  private static final int qSymbolIndex0 = 50;
  private static final int qSymbolIndex1 = 51;
  private static final int qSymbolIndex2 = 52;
  private static final int qSymbolIndex3 = 53;

  private static final int qSymbolCache0 = 60;
  private static final int qSymbolCache1 = 61;
  private static final int qSymbolCache2 = 62;
  private static final int qSymbolCache3 = 63;
  private static final int qSymbolCache4 = 64;

  private final void clearAction( int input)
  {
    lval = 0;
  }

  private final void shiftAction( int input)
  {
    lval = lval << 8 | input;
  }

  private final void shiftLongAction( int input)
  {
    lval = lval << 8 | input;
  }

  private final void bufferClearAction( int input)
  {
    buffer.setLength( 0);
  }

  private final void clearAllAction( int input)
  {
    lval = 0;
    buffer.setLength( 0);
  }

  private final void bufferShiftAction( int input)
  {
    buffer.append( (char)input);
  }

  private final void endIntAction( int input)
  {
    lval = lval << 8 | input;

    if (argsCount >= 2)
      args.addInt( (int)lval);
    argsCount++;
  }

  private final void endFloatAction( int input)
  {
    lval = lval << 8 | input;

    if (argsCount >= 2)
      args.addDouble( Double.longBitsToDouble( lval) );
    argsCount++;
  }

  private final void endSymbolIndexAction( int input)
  {
    lval = lval << 8 | input;

    FtsSymbol s = symbolCache.get( (int)lval);

    if (argsCount == 1)
      selector = s;
    else
      args.addSymbol( s);

    argsCount++;
  }

  private final void endSymbolCacheAction( int input)
  {
    FtsSymbol s = FtsSymbol.get( buffer.toString());

    symbolCache.put( (int)lval, s);

    if (argsCount == 1)
      selector = s;
    else
      args.addSymbol( s);

    argsCount++;
  }

  private final void endStringAction( int input)
  {
    if (argsCount >= 2)
      args.addString( buffer.toString());
    argsCount++;
  }

  private final void endObjectAction( int input)
  {
    lval = lval << 8 | input;

    FtsObject obj = server.getObject( (int)lval);

    if (argsCount == 0)
      target = obj;
    else
      args.addObject( obj);

    argsCount++;
  }

  private final void endMessageAction( int input)
  {
    FtsObject.invokeMessageHandler( target, selector, args);
    args.clear();
    argsCount = 0;
  }

  private final void nextState( int input)
  {
    switch( currentState) {
    case 0:
      /* try to skip till end of message */
      if ( input == BinaryProtocol.END_OF_MESSAGE)
	currentState = qInitial;
    break;
    case qInitial:
      if ( input == BinaryProtocol.INT)
	{
	  currentState = qInt0;
	  clearAction( input);
	}
      else if ( input == BinaryProtocol.FLOAT)
	{
	  currentState = qFloat0;
	  clearAction( input);
	}
      else if ( input == BinaryProtocol.SYMBOL_INDEX)
	{
	  currentState = qSymbolIndex0;
	  clearAction( input);
	}
      else if ( input == BinaryProtocol.SYMBOL_CACHE)
	{
	  currentState = qSymbolCache0;
	  clearAllAction( input);
	}
      else if ( input == BinaryProtocol.STRING)
	{
	  currentState = qString;
	  bufferClearAction( input);
	}
      else if ( input == BinaryProtocol.OBJECT)
	{
	  currentState = qObject0;
	  clearAction( input);
	}
      else if ( input == BinaryProtocol.END_OF_MESSAGE)
	endMessageAction( input);
      else
	currentState = 0;
      break;
    case qInt0:
      currentState = qInt1;
      shiftAction( input);
      break;
    case qInt1:
      currentState = qInt2;
      shiftAction( input);
      break;
    case qInt2:
      currentState = qInt3;
      shiftAction( input);
      break;
    case qInt3:
      currentState = qInitial;
      endIntAction( input);
      break;
    case qFloat0:
      currentState = qFloat1;
      shiftAction( input);
      break;
    case qFloat1:
      currentState = qFloat2;
      shiftAction( input);
      break;
    case qFloat2:
      currentState = qFloat3;
      shiftAction( input);
      break;
    case qFloat3:
      currentState = qFloat4;
      endFloatAction( input);
      break;
    case qFloat4:
      currentState = qFloat5;
      shiftAction( input);
      break;
    case qFloat5:
      currentState = qFloat6;
      shiftAction( input);
      break;
    case qFloat6:
      currentState = qFloat7;
      shiftAction( input);
      break;
    case qFloat7:
      currentState = qInitial;
      endFloatAction( input);
      break;
    case qSymbolIndex0:
      currentState = qSymbolIndex1;
      shiftAction( input);
      break;
    case qSymbolIndex1:
      currentState = qSymbolIndex2;
      shiftAction( input);
      break;
    case qSymbolIndex2:
      currentState = qSymbolIndex3;
      shiftAction( input);
      break;
    case qSymbolIndex3:
      currentState = qInitial;
      endSymbolIndexAction( input);
      break;
    case qSymbolCache0:
      currentState = qSymbolCache1;
      shiftAction( input);
      break;
    case qSymbolCache1:
      currentState = qSymbolCache2;
      shiftAction( input);
      break;
    case qSymbolCache2:
      currentState = qSymbolCache3;
      shiftAction( input);
      break;
    case qSymbolCache3:
      currentState = qSymbolCache4;
      shiftAction( input);
      break;
    case qSymbolCache4:
      if ( input == 0)
	{
	  currentState = qInitial;
	  endSymbolCacheAction( input);
	}
      else
	bufferShiftAction( input);
      break;
    case qString:
      if ( input == 0)
	{
	  currentState = qInitial;
	  endStringAction( input);
	}
      else
	bufferShiftAction( input);
      break;
    case qObject0:
      currentState = qObject1;
      shiftAction( input);
      break;
    case qObject1:
      currentState = qObject2;
      shiftAction( input);
      break;
    case qObject2:
      currentState = qObject3;
      shiftAction( input);
      break;
    case qObject3:
      currentState = qInitial;
      endObjectAction( input);
      break;
    }
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

	nextState( c);

	if (currentState == 0)
	  throw new FtsClientException( "Invalid data in protocol : state=" + currentState + " input=" + c);
      }
  }

  private FtsServer server;
  private FtsServerConnection connection;

  private long lval;
  private StringBuffer buffer;

  private FtsObject target;
  private FtsSymbol selector;
  private FtsArgs args;
  private int argsCount;

  private int currentState;

  private SymbolCache symbolCache;
}
