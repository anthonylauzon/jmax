package ircam.jmax.fts;

/*
 * 
 * An exception to signal that FTS quitted
 */

public class FtsQuittedException extends  Exception
{
  FtsQuittedException()
  {
    super("FTS Quitted");
  }
}
