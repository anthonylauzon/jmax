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

abstract class FtsPort
{
  FtsMessage portMsg = new FtsMessage();

  /** Local Exception represeting a crash in the FTS server. */

  class FtsQuittedException extends Exception
  {
    FtsQuittedException()
    {
      super("FTS Quitted");
    }
  }

  String name;
  FtsServer server;

  /**
   * Create a connection, storing the name.
   * The name will be used for the inputThread name.
   */

  FtsPort(String name)
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

  private static final int blank_token   = 0;
  private static final int int_token     = 1;
  private static final int float_token   = 3;
  private static final int object_token  = 4;
  private static final int connection_token  = 5;
  private static final int data_token  = 7;
  private static final int string_token  = 8;
  private static final int end_token     = 9;

  static int tokenCode(int c)
  {
    if (c == FtsClientProtocol.int_type_code)
      return int_token;
    else if (c == FtsClientProtocol.float_type_code)
      return float_token;
    else if (c == FtsClientProtocol.object_type_code)
      return object_token;
    else if (c == FtsClientProtocol.connection_type_code)
      return connection_token;
    else if (c == FtsClientProtocol.data_type_code)
      return data_token;
    else if (c == FtsClientProtocol.string_start_code)
      return string_token;
    else if (FtsClientProtocol.isBlank(c))
      return blank_token;
    else if (FtsClientProtocol.isEom(c))
      return end_token;
    else 
      return blank_token;
  }

  /**
   * Parse the messages from FTS.
   * It produces a FtsMessage object that represent the received message. <p>
   *
   * @exception ircam.jmax.fts.FtsQuittedExcepetion For some reason the FTS server quitted.
   */

  StringBuffer s = new StringBuffer();

  FtsMessage receiveMessage() throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    int c;
    int type;
    int command;

    s.setLength(0);
    portMsg.reset();

    // read command

    command   = read();

    if (command == -1)
      throw new FtsQuittedException();

    /* Parsing values fields;
       A nice finite state automata.
     
       Blanks and tabs and newlines are allowed inside a message,
       and considered as token terminators.
       */

    int status = blank_token;

    c =  read();

    if (c == -1)
      throw new FtsQuittedException();

    while (status != end_token)
      {
	switch (status)
	  {

	  case blank_token:
	    if (FtsClientProtocol.tokenStartingChar(c))
	      status = tokenCode(c);
	    else
	      return null;		// ERRORE
	    break;

	    /*------------------*/

	  case int_token:

	    if (FtsClientProtocol.tokenStartingChar(c))
	      {
		status = tokenCode(c);
		portMsg.addArgument(new Integer(Integer.parseInt(s.toString())));
		s.setLength(0);
	      }
	    else
	      s.append((char)c);
	    break;

	    /*------------------*/

	  case float_token:

	    if (FtsClientProtocol.tokenStartingChar(c))
	      {
		status = tokenCode(c);
		try
		  {
		    portMsg.addArgument(new Float(s.toString()));
		  }
		catch (java.lang.NumberFormatException e)
		  {
		    portMsg.addArgument(new Float(Float.NaN));
		  }

		s.setLength(0);
	      }
	    else
	      s.append((char)c);
	    break;

	    /*------------------*/

	  case object_token:

	    if (FtsClientProtocol.tokenStartingChar(c))
	      {
		status = tokenCode(c);
		portMsg.addArgument(server.getObjectByFtsId(Integer.parseInt(s.toString())));
		s.setLength(0);
	      }
	    else
	      s.append((char)c);
	    break;

	    /*------------------*/

	  case connection_token:

	    if (FtsClientProtocol.tokenStartingChar(c))
	      {
		status = tokenCode(c);
		portMsg.addArgument(server.getConnectionByFtsId(Integer.parseInt(s.toString())));
		s.setLength(0);
	      }
	    else
	      s.append((char)c);
	    break;

	    /* ----------------- */

	  case data_token:

	    if (FtsClientProtocol.tokenStartingChar(c))
	      {
		status = tokenCode(c);
		portMsg.addArgument(FtsRemoteDataID.get( Integer.parseInt(s.toString())));
		s.setLength(0);
	      }
	    else
	      s.append((char)c);
	    break;

	    /*------------------*/
	  
	  case string_token:
	    if ((c == FtsClientProtocol.string_end_code)  || FtsClientProtocol.isEom(c))
	      {
		portMsg.addArgument(s.toString());
		s.setLength(0);
		status = blank_token;
	      }
	    else
	      s.append((char)c);
	  break;
	  }

	if (status != end_token)
	  {
	    c = read();

	    if (c == -1)
	      throw new FtsQuittedException();
	  }
      }

    portMsg.setCommand(command);

    return portMsg;
  }

  /** Abstract method to send a char; since we can use datagram sockets or other
    means I/O is not necessarly done thru streams */

  abstract protected void write(int data) throws java.io.IOException;

  /** Abstract method to receive a char; since we can use datagram sockets or other
    means I/O is not necessarly done thru streams */

  abstract protected int read() throws java.io.IOException;

  /** Abstract method to Ask for an explicit output flush ; since we
    can use datagram sockets or other means I/O is not necessarly done
    thru streams */

  abstract void flush() throws java.io.IOException;
}



