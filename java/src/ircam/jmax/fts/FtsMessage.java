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

public class FtsMessage
{
  /** Message command. */

  int command;

  /** Get the message command. */

  public int getCommand()
  {
    return this.command;
  }

  /** Set the message command. */

  public void setCommand(int command)
  {
    this.command = command;
  }

  /** Message Content */

  Vector args;

  /**
   * Clean the message content.
   *
   * @see FtsMessage#setArguments
   * @see FtsMessage#getArguments
   */

  public void reset()
  {
    args = null;
  }

  /**
   * Get the message content. 
   *
   * @see FtsMessage#setArguments
   * @see FtsMessage#reset
   */

  public Vector getArguments()
  {
    return args;
  }

  /**
   * Set the message content. 
   * The message content is a Vector, that should include
   * Only Integer, Float and String. <br>
   * The vector is created by the message when read from a stream,
   * or put by the user; don't worry about management, let the java gc
   * do it ...
   *
   * @see FtsMessage#getArguments
   * @see FtsMessage#reset
   */

  public void  setArguments(Vector args)
  {
    this.args = args;
  }

  /** Write the message to a Connection. */

  void writeTo(FtsPort outputStream) throws java.io.IOException
  {
    // Write first the message properties

    outputStream.sendCmd(command);

    // Write the message body

    if (args != null)
      outputStream.sendVector(args);

    // Write the end of the message

    outputStream.sendEom();
  }

  /** 
   * print 
   */

  public String toString()
  {
    return " " + command + args.toString();
  }

}




