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
  final static int INSTANTIATION_ERROR = 1;
  final static int DOTPAT_ERROR = 2;
  final static int TPA_ERROR = 3;

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
      case INSTANTIATION_ERROR:
	return "Instantiation Error " + ((description == null) ? "" : description);
      case DOTPAT_ERROR:
	return "Syntax Error " + ((description == null) ? "" : description);
      case TPA_ERROR:
	return "Syntax Error " + ((description == null) ? "" : description);
      default:
	return "FTS error";
      }
  }
}
