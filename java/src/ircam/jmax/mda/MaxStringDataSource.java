package ircam.jmax.mda;


/**
 * Class added by enzo.
 * It simplify the creation of MaxDataSources from tcl *string* buffers
 * (example: clipboard data)
 */

public class MaxStringDataSource extends MaxDataSource
{
  private StringBuffer itsString;

  public MaxStringDataSource(StringBuffer theString)
  {
    this.itsString = theString;
  }

  public StringBuffer getString()
  {
    return itsString;
  }

  //  public String toString()
  //{
  // return "MaxStringDataSource(" + itsBuffer.toString() + ")";
  //}
}
