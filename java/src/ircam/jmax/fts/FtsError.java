package ircam.jmax.fts;

/** A class representing an FTS error.
 *  The error may be received from FTS or generated
 * in the application layer.
 * 
 * The error may be delivered synchroniuosly with an exception,
 * or asynchroniously thru the error FIFO.
 */

class FtsError
{
  final static int FTS_INSTANTIATION_ERROR = 1;
  final static int FTS_DOTPAT_ERROR = 2;

  String description;
  int    code;

  FtsError(int code, String description)
  {
    this.code = code;
    this.description = description;
  }

  public String toString()
  {
    switch (code)
      {
      case FTS_INSTANTIATION_ERROR:
	return "Instantiation Error " + ((description == null) ? "" : description);
      case FTS_DOTPAT_ERROR:
	return "Syntax Error " + ((description == null) ? "" : description);
      default:
	return "FTS error";
      }
  }
}
