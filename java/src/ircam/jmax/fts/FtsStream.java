//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
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
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.fts;

import java.util.*;
import java.io.*;

import ircam.jmax.utils.*;

/**
 * This class provide some basic mechanism to handle 
 *  the physical connection to FTS. <br>
 *  It manage the parsing and unparsing of the message to the
 *  input and output stream, and the listener thread on the 
 *  FTS to Ermes stream.
 *  This class is abstract, and must be specialized in order
 *  to implement the physical streams.
 * 
 *  @see FtsClientProtocol
 */

abstract public class FtsStream
{
  FtsServer server;

  /** Set the server. */
  void setServer(FtsServer server)
  {
    this.server = server;
  }

  /** Close a connection. (connections are opened by the constructor) */
  abstract void close();

  /** Check if the connection is open. */
  abstract boolean isOpen();

  /** Abstract method to send a char; since we can use datagram sockets or other
    means I/O is not necessarly done thru streams */
  abstract protected void write(int data) throws java.io.IOException;

  /** Abstract method to receive a char; since we can use datagram sockets or other
    means I/O is not necessarly done thru streams */
  abstract protected int read() throws java.io.IOException, FtsQuittedException;

  /** Abstract method to Ask for an explicit output flush ; since we
    can use datagram sockets or other means I/O is not necessarly done
    thru streams */
  abstract void flush() throws java.io.IOException;


  /******************************************************************************/
  /*                                                                            */
  /*                             OUTPUT PARSER                                  */
  /*                                                                            */
  /******************************************************************************/

  /* Send a message in the connection.various methods to 
    send partial data and complete messages.
    */

  /** Send the command code of a message */    
  final void sendCmd(int command) throws java.io.IOException 
  {
    write(command);
  }

  /** Send an Int passes as an Integer */
  final void sendInt(Integer io) throws java.io.IOException 
  {
    sendInt(io.intValue());
  }

  /** Send an Int passes as an int*/
  final void sendInt(int value) throws java.io.IOException 
  {
    String s;
	
    write(FtsClientProtocol.int_type);
    s = Integer.toString(value);
	
    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));
  }

  /** Send an Int passes as a string */
  final void sendInt(String s) throws java.io.IOException 
  {
    write(FtsClientProtocol.int_type);
    
    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));
  }
 

  /** Send an Int passed  as a String Buffer */
  final void sendInt(StringBuffer s) throws java.io.IOException 
  {
    write(FtsClientProtocol.int_type);
    
    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));
  }


  /** Send a Float passed as a Float */
  final void sendFloat(Float fo) throws java.io.IOException 
  {
    String s;

    write(FtsClientProtocol.float_type);
    s = fo.toString();

    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));
  }


  /** Send a Float passed as a float */
  final void sendFloat(float value) throws java.io.IOException 
  {
    String s;

    write(FtsClientProtocol.float_type);
    s = String.valueOf(value);

    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));
  }

  /** Send a float got as a string */
  final void sendFloat(String s) throws java.io.IOException 
  {
    write(FtsClientProtocol.float_type);

    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));
  }

  /** Send a float got as a string buffer */
  final void sendFloat(StringBuffer s) throws java.io.IOException 
  {
    write(FtsClientProtocol.float_type);

    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));
  }


  /** Send a string, passed as a String */
  final void sendString(String s) throws java.io.IOException 
  {
    write(FtsClientProtocol.string_start);

    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));

    write(FtsClientProtocol.string_end);
  }


  /** Send a string, passed as a StringBuffer */
  final void sendString(StringBuffer s) throws java.io.IOException 
  {
    write(FtsClientProtocol.string_start);

    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));

    write(FtsClientProtocol.string_end);
  }
  

  /** Send an Object id */
  final void sendObject(FtsObject obj) throws java.io.IOException 
  {
    int value;
    String s;

    if (obj != null)
      value = obj.getObjectId();
    else
      value = 0;

    write(FtsClientProtocol.object_type);
    s = Integer.toString(value);
		
    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));
  }


  /** Send an Object id */
  final void sendObject(int id) throws java.io.IOException 
  {
    int value;
    String s;

    value = id;

    write(FtsClientProtocol.object_type);
    s = Integer.toString(value);
		
    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));
  }


  /** Send a Connection id */
  final void sendConnection(FtsConnection connection) throws java.io.IOException 
  {
    int value;
    String s;

    if (connection != null)
      value = connection.getConnectionId();
    else
      value = 0;

    write(FtsClientProtocol.connection_type);
    s = Integer.toString(value);
		
    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));
  }

  /** Send a Connection id */
  final void sendConnection(int id) throws java.io.IOException 
  {
    int value;
    String s;

    value = id;

    write(FtsClientProtocol.connection_type);
    s = Integer.toString(value);
		
    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));
  }


  /** Send a remote data id */
  final void sendRemoteData( FtsRemoteData data) throws java.io.IOException 
  {
    int value;
    String s;

    if ( data != null)
      value = data.getId();
    else
      value = 0;

    write( FtsClientProtocol.data_type);
    s = Integer.toString(value);
		
    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));
  }

  /** Send a value */
  final void sendValue(Object o) throws java.io.IOException 
  {
    if (o instanceof Integer)
      {
	sendInt((Integer) o);
      }
    else if (o instanceof Float)
      {
	sendFloat((Float) o);
      }
    else if (o instanceof FtsObject)
      {
	sendObject((FtsObject) o);
      }
    else if (o instanceof String)
      {
	sendString((String)o);
      }
    else if (o instanceof FtsRemoteData)
      {
	sendRemoteData((FtsRemoteData) o);
      }
    else
      {
	// (fd) May be should say something ???
      }
  }
  
  /** Send an atom */
  final void sendFtsAtom(FtsAtom a) throws java.io.IOException 
  {
    switch (a.type)
      {
      case FtsAtom.INT:
	sendInt(a.intValue);
	break;
	
      case FtsAtom.FLOAT:
	sendFloat(a.floatValue);
	break;
	
      case FtsAtom.OBJECT:
	sendObject(a.objectValue);
	break;
	
      case FtsAtom.STRING:
	sendString(a.stringValue);
	break;
      }
  }
  
  /** Send multiple values */
  final void sendVector(MaxVector args) throws java.io.IOException 
  {
    for (int narg = 0; narg < args.size(); narg++)
      {
	Object o = args.elementAt(narg);

	if (o != null)
	  sendValue(o);
      }
  }


  /** Send multiple values */  
  final void sendArray(Object[] args) throws java.io.IOException 
  {
    for (int narg = 0; narg < args.length; narg++)
      {
	Object o = args[narg];

	if (o != null)
	  sendValue(o);
      }
  }

  /** Send multiple values */  
  final void sendArray(FtsAtom[] values, int from , int howMany) throws java.io.IOException 
  {
    for (int i = 0; i < howMany; i++)
      sendFtsAtom(values[from + i]);
  }

  /** Send multiple int values */  
  final void sendArray(int[] values, int from , int howMany) throws java.io.IOException 
  {
    for (int i = 0; i < howMany; i++)
      sendInt(values[from + i]);
  }

  /** Send multiple float values */
  final void sendArray(float[] values, int from, int howMany) throws java.io.IOException 
  {
    for (int i = 0; i < howMany; i++)
      sendFloat(values[from + i]);
  }

  /** Send the end of message mark */
  final void sendEom() throws java.io.IOException 
  {
    write(FtsClientProtocol.end_of_message);
    flush();
  }


  /******************************************************************************/
  /*                                                                            */
  /*                             INPUT PARSER                                   */
  /*                                                                            */
  /******************************************************************************/

  /**
   * Parse the messages from FTS.
   * It produces a FtsMessage object that represent the received message. <p>
   * Return the command of the message.
   *
   * @exception ircam.jmax.fts.FtsQuittedExcepetion For some reason the FTS server quitted.
   */

  private String[] symbolCache = new String[1024];
  private StringBuffer s = new StringBuffer();
  private int status = FtsClientProtocol.end_of_message; // the parser status, usually next arg type

  private final void reallocateSymbolCache(int idx)
  {
    int size;
    String[] newCache;

    size = symbolCache.length;

    while (idx < size)
      size = 2 * size;

    newCache = new String[size];
    System.arraycopy(newCache, 0, symbolCache, 0, symbolCache.length);

    symbolCache = newCache;
  }

  private final void skipToEnd()
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    int c;

    while (status != FtsClientProtocol.end_of_message)
      {
	c = read();

	if (FtsClientProtocol.tokenStartingChar(c))
	  status = c;
      }
  }


  /** Check if there are more arguments in the current message */
  public final boolean endOfArguments()
  {
    return status == FtsClientProtocol.end_of_message;
  }

  /** Check if the next arguments in the current message  is an int */
  public final boolean nextIsInt()
  {
    return status == FtsClientProtocol.int_type;
  }

  /** Check if the next arguments in the current message  is a float */
  public final boolean nextIsFloat()
  {
    return status == FtsClientProtocol.float_type;
  }


  /** Check if the next arguments in the current message  is a symbol */
  public final boolean nextIsSymbol()
  {
    return ((status == FtsClientProtocol.symbol_type) ||
	    (status == FtsClientProtocol.symbol_cached_type) ||
	    (status == FtsClientProtocol.symbol_and_def_type));
  }


  /** Check if the next arguments in the current message  is a string */
  public final boolean nextIsString()
  {
    return ((status == FtsClientProtocol.symbol_type) ||
	    (status == FtsClientProtocol.symbol_and_def_type) || 
	    (status == FtsClientProtocol.string_start));
  }

  /** Check if the next arguments in the current message  is an object */
  public final boolean nextIsObject()
  {
    return status == FtsClientProtocol.object_type;
  }

  /** Check if the next arguments in the current message  is a connection  */
  public final boolean nextIsConnection()
  {
    return status == FtsClientProtocol.connection_type;
  }


  /** Check if the next arguments in the current message  is a remote data   */
  public final boolean nextIsData()
  {
    return status == FtsClientProtocol.data_type;
  }


  /** Get the command code of the next message */
  public final int getCommand()
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    int c;

    if (status != FtsClientProtocol.end_of_message)
      skipToEnd();

    c = read();
    status = read();

    return c;
  }


  /** Get the next argument of the current message as an int.
     The caller is responsable to check that the next argument
     have the good type before calling this method */
  public final int getNextIntArgument()
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    int r = 0;

    r = r | ((read() & 0xff) << 24);
    r = r | ((read() & 0xff) << 16);
    r = r | ((read() & 0xff) << 8);
    r = r | ((read() & 0xff) << 0);

    status = read();

    return r;
  }

  /** Get the next argument of the current message as a float.
     The caller is responsable to check that the next argument
     have the good type before calling this method */
  public final float getNextFloatArgument()
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    int r = 0;

    r = r | ((read() & 0xff) << 24);
    r = r | ((read() & 0xff) << 16);
    r = r | ((read() & 0xff) << 8);
    r = r | ((read() & 0xff) << 0);

    status = read();
    return Float.intBitsToFloat(r);
  }

  /** Get the next argument of the current message as an fts object.
     The caller is responsable to check that the next argument
     have the good type before calling this method */
  public final FtsObject getNextObjectArgument()
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    return server.getObjectByFtsId(getNextIntArgument());
  }


  /** Get the next argument of the current message as an fts connection
     The caller is responsable to check that the next argument
     have the good type before calling this method */
  public final FtsConnection getNextConnectionArgument()
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    return server.getConnectionByFtsId(getNextIntArgument());
  }

  /** Get the next argument of the current message as a remote data
     The caller is responsable to check that the next argument
     have the good type before calling this method */
  public final FtsRemoteData getNextDataArgument()
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    return server.getRemoteTable().get(getNextIntArgument());
  }


  private final String getNextString()
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    String str;
    int c;

    s.setLength(0);

    /* The loop assume we have a valid status */
    c = read();

    while (c != FtsClientProtocol.string_end)
      {
	s.append((char)c);
	c = read();
      }

    str = s.toString();

    // Skip the end of string token
    // and read the next type token

    status = read();

    return str;    
  }

  /** Get the next argument of the current message as a symbol
     The caller is responsable to check that the next argument
     have the good type before calling this method.
     A symbol is an interned string, send by fts using the symbol cache;
     strings are generally not sent as symbol, unless we want the interned/cached
     version */
  public final String getNextSymbolArgument()
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    if (status == FtsClientProtocol.symbol_cached_type)
      {
	int idx;
	idx = getNextIntArgument();
	return symbolCache[idx];
      }
    else if (status == FtsClientProtocol.symbol_and_def_type)
      {
	int idx = getNextIntArgument();
	String str = getNextString().intern();

	if (idx >= symbolCache.length)
	  reallocateSymbolCache(idx);

	symbolCache[idx] = str; 

	return str;
      }
    else if (status == FtsClientProtocol.symbol_type)
      {
	return getNextString().intern();
      }
    else
      return "";
  }


  /** Get the next argument of the current message as a string
     The caller is responsable to check that the next argument
     have the good type before calling this method.
     */
  public final String getNextStringArgument()
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    if (nextIsSymbol())
      return getNextSymbolArgument();
    else
      return getNextString();
  }


  /** Get the next argument of the current message.
    Call this method when you don't have expect a specific type;
    less efficent for basic type because it needs to allocate Integer and Float objects.
   */
  public final Object getNextArgument()
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    switch (status)
      {
      case FtsClientProtocol.int_type:
	return new Integer(getNextIntArgument());

      case FtsClientProtocol.float_type:
	return new Float(getNextFloatArgument());

      case FtsClientProtocol.object_type:
	return getNextObjectArgument();

      case FtsClientProtocol.connection_type:
	return getNextConnectionArgument();

      case FtsClientProtocol.data_type:
	return getNextDataArgument();

      case FtsClientProtocol.string_start:
	return getNextStringArgument();

      case FtsClientProtocol.symbol_cached_type:
      case FtsClientProtocol.symbol_and_def_type:
      case FtsClientProtocol.symbol_type:
	return getNextSymbolArgument();

      default:
	return null;
      }
  }

  private FtsAtom args[] = null;
  private int argsCount = 0;

  private void growArgsArray()
  {
    int oldLength;

    if (args == null)
      {
	oldLength = 0;
	args = new FtsAtom[128];
      }
    else
      {
	FtsAtom newArgs[];

	oldLength = args.length;
	newArgs = new FtsAtom[2 * oldLength];
	System.arraycopy( args, 0, newArgs, 0, oldLength);
	
	args = newArgs;
      }

    for ( int i = oldLength; i < args.length; i++)
      args[i] = new FtsAtom();
  }

  public final int getNumberOfArgs()
  {
    return argsCount;
  }

  /**
   * Returns the next arguments as an array of FtsAtom. This array is a member of class FtsStream.
   *
   * @return   the message arguments as an array of FtsAtom
   * @exception java.io.IOException if an error occured during reading
   * @exception FtsQuittedException if the server has quitted
   * @exception java.io.InterruptedIOException if the read thread was interrupted
   */
  public final FtsAtom[] getArgs()
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    argsCount = 0;

    if (args == null)
      growArgsArray();

    while ( ! endOfArguments())
      {
	switch (status)
	  {
	  case FtsClientProtocol.int_type:
	    args[ argsCount ].type = FtsAtom.INT;
	    args[ argsCount ].intValue = getNextIntArgument();
	    break;

	  case FtsClientProtocol.float_type:
	    args[ argsCount ].type = FtsAtom.FLOAT;
	    args[ argsCount ].floatValue = getNextFloatArgument();
	    break;

	  case FtsClientProtocol.object_type:
	    args[ argsCount ].type = FtsAtom.OBJECT;
	    args[ argsCount ].objectValue = getNextObjectArgument();
	    break;

	  case FtsClientProtocol.string_start:
	    args[ argsCount ].type = FtsAtom.STRING;
	    args[ argsCount ].stringValue = getNextStringArgument();
	    break;

	  case FtsClientProtocol.symbol_cached_type:
	  case FtsClientProtocol.symbol_and_def_type:
	  case FtsClientProtocol.symbol_type:
	    args[ argsCount ].type = FtsAtom.STRING;
	    args[ argsCount ].stringValue = getNextSymbolArgument();
	    break;

	  default:
	    args[ argsCount ].type = FtsAtom.VOID;
	    break;
	  }

	argsCount++;

	if ( argsCount >= args.length)
	  growArgsArray();
      }

    return args;
  }
}

