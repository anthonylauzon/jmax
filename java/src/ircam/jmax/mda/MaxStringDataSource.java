package ircam.jmax.mda;


/**
 * Class added by enzo.
 * It simplify the creation of MaxDataSources from tcl *string* buffers
 * (example: clipboard data)
 */

public class MaxStringDataSource extends MaxDataSource
{
  private StringBuffer itsBuffer;

  public MaxStringDataSource(StringBuffer theString)
  {
    this.itsBuffer = theString;
  }

  public StringBuffer getString()
  {
    return itsBuffer;
  }

  public String toString()
  {
    return itsBuffer.toString();
  }

  public String getName()
  {
    return itsBuffer.toString();
  }

  public boolean exists()
  {
    return true;
  }

  public boolean canWrite()
  {
    return true;
  }

  public boolean canRead()
  {
    return true;
  }
}
