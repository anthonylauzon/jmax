package ircam.jmax.fts;

import java.util.*;
import java.io.*;

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

abstract class FtsPort implements Runnable
{
  FtsMessage portMsg = new FtsMessage();
  boolean flushing = true;
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

  InputStream in_stream = null;
  OutputStream out_stream = null;

  boolean running  = true;
  Thread inputThread;

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

  /** Open a connection. */

  void open()
  {
    // Create the streams 

    openIOStreams();

    // start the input thread

    inputThread = new Thread(this, name);
    inputThread.setPriority(Thread.MAX_PRIORITY);
    inputThread.start(); 
  }

  /** Close a connection. */

  void close()
  {
    // ask the input thread loop to close the streams and exit
    // this to avoid inconsistency in accessing the input thread.

    running = false;
  }

  /** Check if the connection is open. */

  boolean isOpen()
  {
    return (in_stream != null) && (out_stream != null);
  }

  /**
   * Create the comunication streams.
   * Must be created by the subclass,
   * i.e. the implementation of the connection If the connection
   * actually bootstrap the server, it must be bootstrapped in the
   * openIOStreams method.
   */

  abstract void openIOStreams();

  /**
   * Destroy the comunication streams.
   * Must be created by the implementation of the connection
   * If the connection actually bootstrap the server, 
   * it must be shutdown in the closeIOStreams method.
   */

  abstract void closeIOStreams();
       
  /** the main loop of the input listener thread. */

  public void run()
  {
    while (running && (in_stream !=  null) && (out_stream != null))
      {
	try
	  {
	    FtsMessage msg;
	    msg = receiveMessage();
	    if (msg != null)
	      server.dispatchMessage(msg);
	  }
	catch (java.io.InterruptedIOException e)
	  {
	    /* Ignore, just retry */
	  }
	catch (FtsQuittedException e)
	  {
	    running = false;
	  }
	catch (Exception e)
	  {
	    // Try to survive an exception
	    
	    System.err.println("System exception " + e);
	    e.printStackTrace();
	  }
      }

    // close the thread and the streams
    closeIOStreams();
  }

  /**
   * Handle the setting of the connection parameters.
   * Each subclass must specialize this methods for the connection
   * dependent parameters.
   */

  abstract void setParameter(String property, Object value);

  /**
   * Start should start the server.
   * If the subclass do not start the server at init time,
   * should start in the implementation of the start method.
   */

  abstract void start();

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
    out_stream.write(command);
  }

  final void sendInt(Integer io) throws java.io.IOException 
  {
    sendInt(io.intValue());
  }

  final void sendInt(int value) throws java.io.IOException 
  {
    String s;
    
    out_stream.write(FtsClientProtocol.int_type_code);
    s = Integer.toString(value);
    
    for (int i = 0; i < s.length(); i++)
      out_stream.write(s.charAt(i));
  }

  /** Send an Int passed got as a string */

  final void sendInt(String s) throws java.io.IOException 
  {
    out_stream.write(FtsClientProtocol.int_type_code);
    
    for (int i = 0; i < s.length(); i++)
      out_stream.write(s.charAt(i));
  }
 

  /** Send an Int passed got as a String Buffer */

  final void sendInt(StringBuffer s) throws java.io.IOException 
  {
    out_stream.write(FtsClientProtocol.int_type_code);
    
    for (int i = 0; i < s.length(); i++)
      out_stream.write(s.charAt(i));
  }
 

  final void sendFloat(Float fo) throws java.io.IOException 
  {
    String s;

    out_stream.write(FtsClientProtocol.float_type_code);
    s = fo.toString();

    for (int i = 0; i < s.length(); i++)
      out_stream.write(s.charAt(i));
  }

  final void sendFloat(float value) throws java.io.IOException 
  {
    String s;

    out_stream.write(FtsClientProtocol.float_type_code);
    s = String.valueOf(value);

    for (int i = 0; i < s.length(); i++)
      out_stream.write(s.charAt(i));
  }

  /** Send a float got as a string */

  final void sendFloat(String s) throws java.io.IOException 
  {
    out_stream.write(FtsClientProtocol.float_type_code);

    for (int i = 0; i < s.length(); i++)
      out_stream.write(s.charAt(i));
  }

  /** Send a float got as a string buffer */

  final void sendFloat(StringBuffer s) throws java.io.IOException 
  {
    out_stream.write(FtsClientProtocol.float_type_code);

    for (int i = 0; i < s.length(); i++)
      out_stream.write(s.charAt(i));
  }



  final void sendString(String s) throws java.io.IOException 
  {
    out_stream.write(FtsClientProtocol.string_start_code);
    out_stream.write(s.getBytes());
    out_stream.write(FtsClientProtocol.string_end_code);
  }

  final void sendString(StringBuffer s) throws java.io.IOException 
  {
    out_stream.write(FtsClientProtocol.string_start_code);

    for (int i = 0; i < s.length(); i++)
      out_stream.write(s.charAt(i));

    out_stream.write(FtsClientProtocol.string_end_code);
  }
  
  final void sendObject(FtsObject obj) throws java.io.IOException 
  {
    int value;
    String s;

    if (obj != null)
      value = obj.getObjectId();
    else
      value = 0;

    out_stream.write(FtsClientProtocol.object_type_code);
    s = Integer.toString(value);
		
    for (int i = 0; i < s.length(); i++)
      out_stream.write(s.charAt(i));
  }


  final void sendObject(int id) throws java.io.IOException 
  {
    int value;
    String s;

    value = id;

    out_stream.write(FtsClientProtocol.object_type_code);
    s = Integer.toString(value);
		
    for (int i = 0; i < s.length(); i++)
      out_stream.write(s.charAt(i));
  }


  final void sendConnection(FtsConnection connection) throws java.io.IOException 
  {
    int value;
    String s;

    if (connection != null)
      value = connection.getConnectionId();
    else
      value = 0;

    out_stream.write(FtsClientProtocol.connection_type_code);
    s = Integer.toString(value);
		
    for (int i = 0; i < s.length(); i++)
      out_stream.write(s.charAt(i));
  }


  final void sendConnection(int id) throws java.io.IOException 
  {
    int value;
    String s;

    value = id;

    out_stream.write(FtsClientProtocol.connection_type_code);
    s = Integer.toString(value);
		
    for (int i = 0; i < s.length(); i++)
      out_stream.write(s.charAt(i));
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
  }
  

  final void sendVector(Vector args) throws java.io.IOException 
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


  final void sendArray(int[] values, int from , int to) throws java.io.IOException 
  {
    for (int narg = from; narg < to; narg++)
      sendInt(values[narg]);
  }

  final void sendArray(float[] values, int from, int to) throws java.io.IOException 
  {
    for (int narg = from; narg < to; narg++)
      sendFloat(values[narg]);
  }


  
  final void sendEom() throws java.io.IOException 
  {
    out_stream.write(FtsClientProtocol.end_of_message_code);

    if (flushing)
      out_stream.flush();
  }


  final void sendMessage(FtsMessage msg)
  {
    if (out_stream != null)
      try
      {
	msg.writeTo(this);
      }
    catch (java.io.IOException	e)
      {
	/* Should implement a decent expection system
	   for the client library */
      }
  }

  /** Set the flushing mode */

  final void setFlushing(boolean b) 
  {
    flushing = b;
    
    try
      {
	if (out_stream != null && flushing)
	  out_stream.flush();
      }
    catch (java.io.IOException e)
      {
	// ???
      }
  }

  /** Ask for an explicit flush */

  final void flush() throws java.io.IOException
  {
    if (out_stream != null)
      out_stream.flush();
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
  private static final int string_token  = 6;
  private static final int end_token     = 7;

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

  FtsMessage receiveMessage() throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    StringBuffer s = new StringBuffer();
    int c;
    int type;
    int command;

    portMsg.reset();

    // read command
    command   = in_stream.read();

    if (command == -1)
      throw new FtsQuittedException();

    /* Parsing values fields;
       A nice finite state automata.
     
       Blanks and tabs and newlines are allowed inside a message,
       and considered as token terminators.
       */

    int status = blank_token;

    c =  in_stream.read();

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
		FtsObject obj;

		status = tokenCode(c);

		obj = server.getObjectByFtsId(Integer.parseInt(s.toString()));
		
		portMsg.addArgument(obj);
		s.setLength(0);
	      }
	    else
	      s.append((char)c);
	    break;

	    /*------------------*/

	  case connection_token:

	    if (FtsClientProtocol.tokenStartingChar(c))
	      {
		FtsConnection connection;

		status = tokenCode(c);

		connection = server.getConnectionByFtsId(Integer.parseInt(s.toString()));
		
		portMsg.addArgument(connection);
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
	    c = in_stream.read();

	    if (c == -1)
	      throw new FtsQuittedException();
	  }
      }

    portMsg.setCommand(command);

    return portMsg;
  }
}



