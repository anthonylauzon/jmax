package ircam.jmax.fts;

/** The generic exception for fts; include
 * a pointer to an FtsError, that can be delivered
 * also with an asynchronious mechanism, and that contain
 * the error information
 */

public class FtsException extends Exception
{
  FtsError error;

  FtsException(FtsError error)
  {
    super(error.toString());
    this.error = error;
  }

  public FtsError getFtsError()
  {
    return error;
  }

  public String toString()
  {
    return error.toString();
  }
}
