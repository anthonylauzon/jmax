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
	
    write(FtsClientProtocol.int_type_code);
    s = Integer.toString(value);
	
    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));
  }

  /** Send an Int passed got as a string */

  final void sendInt(String s) throws java.io.IOException 
  {
    write(FtsClientProtocol.int_type_code);
    
    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));
  }
 

  /** Send an Int passed got as a String Buffer */

  final void sendInt(StringBuffer s) throws java.io.IOException 
  {
    write(FtsClientProtocol.int_type_code);
    
    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));
  }
 

  final void sendFloat(Float fo) throws java.io.IOException 
  {
    String s;

    write(FtsClientProtocol.float_type_code);
    s = fo.toString();

    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));
  }

  final void sendFloat(float value) throws java.io.IOException 
  {
    String s;

    write(FtsClientProtocol.float_type_code);
    s = String.valueOf(value);

    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));
  }

  /** Send a float got as a string */

  final void sendFloat(String s) throws java.io.IOException 
  {
    write(FtsClientProtocol.float_type_code);

    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));
  }

  /** Send a float got as a string buffer */

  final void sendFloat(StringBuffer s) throws java.io.IOException 
  {
    write(FtsClientProtocol.float_type_code);

    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));
  }



  final void sendString(String s) throws java.io.IOException 
  {
    write(FtsClientProtocol.string_start_code);

    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));

    write(FtsClientProtocol.string_end_code);
  }

  final void sendString(StringBuffer s) throws java.io.IOException 
  {
    write(FtsClientProtocol.string_start_code);

    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));

    write(FtsClientProtocol.string_end_code);
  }
  
  final void sendObject(FtsObject obj) throws java.io.IOException 
  {
    int value;
    String s;

    if (obj != null)
      value = obj.getObjectId();
    else
      value = 0;

    write(FtsClientProtocol.object_type_code);
    s = Integer.toString(value);
		
    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));
  }


  final void sendObject(int id) throws java.io.IOException 
  {
    int value;
    String s;

    value = id;

    write(FtsClientProtocol.object_type_code);
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

    write(FtsClientProtocol.connection_type_code);
    s = Integer.toString(value);
		
    for (int i = 0; i < s.length(); i++)
      write(s.charAt(i));
  }


  final void sendConnection(int id) throws java.io.IOException 
  {
    int value;
    String s;

    value = id;

    write(FtsClientProtocol.connection_type_code);
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

    write( FtsClientProtocol.data_type_code);
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
    write(FtsClientProtocol.end_of_message_code);
    flush();
  }


  /******************************************************************************/
  /*                                                                            */
  /*                             INPUT PARSER                                   */
  /*                                                                            */
  /******************************************************************************/

  // Token types for the parser

  public static final int intValue     = 1;
  public static final int floatValue   = 2;
  public static final int objectValue  = 3;
  public static final int connectionValue  = 4;
  public static final int dataValue    = 5;
  public static final int stringValue  = 6;
  public static final int endOfMessage = 7;

  final int nextStatus(int c) throws FtsQuittedException
  {
    if (c == FtsClientProtocol.int_type_code)
      return intValue;
    else if (c == FtsClientProtocol.float_type_code)
      return floatValue;
    else if (c == FtsClientProtocol.object_type_code)
      return objectValue;
    else if (c == FtsClientProtocol.connection_type_code)
      return connectionValue;
    else if (c == FtsClientProtocol.data_type_code)
      return dataValue;
    else if (c == FtsClientProtocol.string_start_code)
      return stringValue;
    else if (FtsClientProtocol.isEom(c))
      return endOfMessage;
    else 
      return endOfMessage;
  }

  /**
   * Parse the messages from FTS.
   * It produces a FtsMessage object that represent the received message. <p>
   * Return the command of the message.
   *
   * @exception ircam.jmax.fts.FtsQuittedExcepetion For some reason the FTS server quitted.
   */

  StringBuffer s = new StringBuffer();
  int status = endOfMessage; // the parser status, usually next arg type

  private final void skipToEnd()
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    int c;

    while (status != endOfMessage)
      {
	c = read();

	if (FtsClientProtocol.tokenStartingChar(c))
	  status = nextStatus(c);
      }
  }


  public final boolean endOfArguments()
  {
    return status == endOfMessage;
  }

  public final int getNextType()
  {
    return status;
  }

  public final int getCommand()
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    int c;

    if (status != endOfMessage)
      skipToEnd();

    c = read();
    status = nextStatus(read());
    return c;
  }


  public final int getNextIntArgument()
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    int r = 0;
    int sign = 1;
    int c;

    c = read();

    if (c == '-')
      {
	sign = -1;
	c = read();
      }

    while (! FtsClientProtocol.tokenStartingChar(c))
      {
	r = r * 10 + (c - '0');
	c = read();
      }
    status = nextStatus(c);

    return r * sign;
  }


  public final float getNextFloatArgument()
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    boolean pointDone = false;
    int divider = 1;
    int sign = 1;
    int intPart = 0;
    int decimalPart = 0;
    int c;

    c = read();

    if (c == '-')
      {
	sign = -1;
	c = read();
      }

    while (! FtsClientProtocol.tokenStartingChar(c))
      {
	if (c == '.')
	  {
	    pointDone = true;
	  }
	else if (pointDone)
	  {
	    decimalPart = decimalPart * 10 + (c - '0');
	    divider = divider * 10;
	  }
	else
	  {
	    intPart = intPart * 10 + (c - '0');
	  }

	c = read();
      }

    status = nextStatus(c);

    return (float) (sign * ((double) intPart + ((double) decimalPart / (double) divider)));
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
    return FtsRemoteDataID.get(getNextIntArgument());
  }


  public final String getNextStringArgument()
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    String str;
    int c;

    s.setLength(0);

    /* The loop assume we have a valid status */

    c = read();

    while (c != FtsClientProtocol.string_end_code)
      {
	s.append((char)c);
	c = read();
      }

    str = s.toString();

    // Skip the end of string token
    // and read the next type token

    c =  read();
    status = nextStatus(c);

    return str;    
  }


  public final Object getNextArgument()
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    switch (status)
      {
      case intValue:
	return new Integer(getNextIntArgument());

      case floatValue:
	return new Float(getNextFloatArgument());

      case objectValue:
	return getNextObjectArgument();

      case connectionValue:
	return getNextConnectionArgument();

      case dataValue:
	return getNextDataArgument();

      case stringValue:
	return getNextStringArgument();

      default:
	return null;
      }
  }
}









