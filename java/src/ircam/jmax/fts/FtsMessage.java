package ircam.jmax.fts;

import java.util.*;
import java.io.*;

/**
 * An FTS message.
 * Represent a complete FTS message, coming or going to 
 * the FTS server. It is usually used only in reception, and it
 * will probabily disappear for something more memory and 
 * time efficent.
 */

final public class FtsMessage
{
  /** Message command. */

  private int command;

  /** Message Content */
  
  private int args_count = 0;
  private int args_fill = 0;
  private Object[] args = new Object[256];

  /** Get the message command. */

  public final int getCommand()
  {
    return this.command;
  }

  /** Set the message command. */

  final void setCommand(int command)
  {
    this.command = command;
  }

  /** 
   * Private method to resize the
   * arg array
   */

  final private void doubleSize()
  {
    Object[] new_args;

    new_args = new Object[2 * args.length];

    System.arraycopy(args, 0, new_args, 0, args.length);

    args = new_args;
  }

  /**
   * Clean the message content.
   *
   * @see FtsMessage#setArguments
   * @see FtsMessage#getArguments
   */

  public final void reset()
  {
    for (int i = 0; i < args_fill; i++)
      args[i] = null;

    args_fill = 0;
    args_count = 0;
  }

  /** 
   * Get the next argument, as an Object
   */

  public final Object getNextArgument()
  {
    if (args_count < args_fill)
      return args[args_count++];
    else
      return null;
  }
  
  /**
   * Get a message argument
   */

  public final Object getArgument(int i)
  {
    if (i < args_fill)
      return args[i];
    else
      return null;
  }

  /**
   * Get a the number of arguments
   */

  public final int getNumberOfArguments()
  {
    return args_fill;
  }


  /**
   * Add an argument at the end
   */

  final void addArgument(Object obj)
  {
    if (args_fill >= args.length)
      doubleSize();

    args[args_fill++] = obj;
  }

  /** 
   * print 
   */

  public String toString()
  {
    StringBuffer buf = new StringBuffer();

    buf.append("FtsMessage<" + FtsClientProtocol.getCommandName(command) + " [");

    for (int i = 0; i < args_fill ; i++)
      if (args[i] == null)
	buf.append("(null) ");
      else
	{
	  buf.append(args[i].toString());
	  buf.append(" ");
	}

    buf.append("]>");

    return buf.toString();
  }
}




