package ircam.jmax.mda;


/**
 * Class added by enzo.
 * It simplify the creation of MaxDataSources from tcl *string* buffers
 * (example: clipboard data)

 * The problem with this is that a DataSource object is a pointer to
 * the "entity" keeping the data, and not the data itself.
 * I.e. in case of files it correspond to the file name, not to the
 * file itself, and so on; so, ok for MaxStringDataHandler, to get
 * data from strings in memory, but not ok for this implementation
 * of source, beacause it is an implementation of the content itself,
 * and not a pointer to the content.
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
}
