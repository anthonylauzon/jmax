package ircam.jmax.mda; 


/** A String Data Handler is an object able to load a Max Data instance
 *  from a generic *input* stream data source.
 * (class added by enzo)
 */

abstract public class MaxStringDataHandler extends MaxDataHandler
{

  public MaxData loadInstance(MaxDataSource source) throws MaxDataException
  {
    MaxStringDataSource streamSource = (MaxStringDataSource) source;
    StringBuffer string = streamSource.getString();
    MaxData obj;

    obj = makeInstance(string);
    
    obj.setDataHandler(this);
    obj.setDataSource(source);

    return obj;
  }

  public boolean canSaveTo(MaxDataSource theSource) {
    return true;
  }
  /** Actually build and return an Max Data from a string */

  abstract protected MaxData makeInstance(StringBuffer string) throws MaxDataException;
}




