package ircam.jmax.fts;

/** The generic exception for fts.
 * Very primitive at the moment.
 */

public class FtsException extends Exception
{
  FtsException(String detail)
  {
    super(detail);
  }
}
