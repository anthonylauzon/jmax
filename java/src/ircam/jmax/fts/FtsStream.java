//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
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
  /** Local Exception represeting a crash in the FTS server. */

  String name;
  FtsServer server;

  /**
   * Create a connection, storing the name.
   * The name will be used for the inputThread name.
   */

  FtsStream(String name)
  {
    this.name = name;
  }

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

  final void sendCmd(int command) throws java.io.IOException 
  {
    write(command);
  }

  final void sendInt(Integer io) throws java.io.IOException 
  {
    sendInt(io.intValue());
  }

  final void sendInt(int value) throws java.io.IOException 
  {
    String s;
	
    write(FtsClientProtocol.int_type);
    s = Integer.toString(value);
	
    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));
  }

  /** Send an Int passed got as a string */

  final void sendInt(String s) throws java.io.IOException 
  {
    write(FtsClientProtocol.int_type);
    
    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));
  }
 

  /** Send an Int passed got as a String Buffer */

  final void sendInt(StringBuffer s) throws java.io.IOException 
  {
    write(FtsClientProtocol.int_type);
    
    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));
  }
 

  final void sendFloat(Float fo) throws java.io.IOException 
  {
    String s;

    write(FtsClientProtocol.float_type);
    s = fo.toString();

    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));
  }

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



  final void sendString(String s) throws java.io.IOException 
  {
    write(FtsClientProtocol.string_start);

    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));

    write(FtsClientProtocol.string_end);
  }

  final void sendString(StringBuffer s) throws java.io.IOException 
  {
    write(FtsClientProtocol.string_start);

    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));

    write(FtsClientProtocol.string_end);
  }
  
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
  

  final void sendVector(MaxVector args) throws java.io.IOException 
  {
    for (int narg = 0; narg < args.size(); narg++)
      {
	Object o = args.elementAt(narg);

	if (o != null)
	  sendValue(o);
      }
  }

  final void sendArray(Object[] args) throws java.io.IOException 
  {
    for (int narg = 0; narg < args.length; narg++)
      {
	Object o = args[narg];

	if (o != null)
	  sendValue(o);
      }
  }


  final void sendArray(int[] values, int from , int howMany) throws java.io.IOException 
  {
    for (int i = 0; i < howMany; i++)
      sendInt(values[from + i]);
  }

  final void sendArray(float[] values, int from, int howMany) throws java.io.IOException 
  {
    for (int i = 0; i < howMany; i++)
      sendFloat(values[from + i]);
  }


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


  public final boolean endOfArguments()
  {
    return status == FtsClientProtocol.end_of_message;
  }

  
  public final boolean nextIsInt()
  {
    return status == FtsClientProtocol.int_type;
  }

  public final boolean nextIsFloat()
  {
    return status == FtsClientProtocol.float_type;
  }

  public final boolean nextIsSymbol()
  {
    return ((status == FtsClientProtocol.symbol_type) ||
	    (status == FtsClientProtocol.symbol_cached_type) ||
	    (status == FtsClientProtocol.symbol_and_def_type));
  }

  public final boolean nextIsString()
  {
    return ((status == FtsClientProtocol.symbol_type) ||
	    (status == FtsClientProtocol.symbol_and_def_type) || 
	    (status == FtsClientProtocol.string_start));
  }

  public final boolean nextIsObject()
  {
    return status == FtsClientProtocol.object_type;
  }

  public final boolean nextIsConnection()
  {
    return status == FtsClientProtocol.connection_type;
  }

  public final boolean nextIsData()
  {
    return status == FtsClientProtocol.data_type;
  }


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

  public final FtsObject getNextObjectArgument()
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    return server.getObjectByFtsId(getNextIntArgument());
  }


  public final FtsConnection getNextConnectionArgument()
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    return server.getConnectionByFtsId(getNextIntArgument());
  }


  public final FtsRemoteData getNextDataArgument()
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    return server.getRemoteTable().get(getNextIntArgument());
  }


  /* a symbol is an interned string, send by fts using the symbol cache;
     strings are generally not sent as symbol, unless we want the interned/cached
     version */

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

  public final String getNextStringArgument()
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    if (nextIsSymbol())
      return getNextSymbolArgument();
    else
      return getNextString();
  }


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
}









