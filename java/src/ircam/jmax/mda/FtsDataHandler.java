package ircam.jmax.mda; 

import java.io.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;


/** An instance of this data handler can load MaxData from
 *  a tcl file obeyng the "jmax" command conventions
 * Now support only files.
 * 
 * A TCL Data file is a file that start with the jmax command
 * at the *beginning* of the first line.
 */

public class FtsDataHandler extends MaxDataHandler
{
  public FtsDataHandler()
  {
    super();
  }

  /** We can load from a file start with the "jmax " string*/

  public boolean canLoadFrom(MaxDataSource source)
  {
    if ((source instanceof MaxFtsDataSource) && super.canLoadFrom(source))
      {
	return true;
      }
    else
      return false;
  }

  /** Make the real instance; FTS data objects are smarter,
    beacause they know how to load themselves from an FTS object.
   */

  protected MaxData loadInstance(MaxDataSource source) throws MaxDataException
  {
    FtsLocation location = ((MaxFtsDataSource) source).getFtsLocation();
    FtsObject object = location.getObject();

    // FtsDataObject is an implementational API, used only 
    // from here

    if (object instanceof FtsDataObject)
      {
	if (((FtsDataObject) object).getData() != null)
	  return ((FtsDataObject) object).getData();
	else
	  {
	    MaxData data;
	    MaxDataType type;

	    // Actually, it is the object that have to know how to create the FTS
	    // data; we map it to the property API.

	    type = ((FtsDataObject)object).getObjectDataType();

	    data = type.newInstance();

	    if (data instanceof MaxFtsData)
	      {
		data.setDataSource(source);
		data.setDataHandler(this);
		((MaxFtsData) data).loadFromFtsObject();
	 
		return data;
	      }
	    else
	      throw new MaxDataException("Object " + object + " produced a non MaxFtsData MaxData");
	  }
      }
    else
      throw new MaxDataException("Object " + object + " don't have data to edit");
  }

  /** Save: basic support for saving a data in an FTS object 
   */

  public void saveInstance(MaxData instance) throws MaxDataException
  {
    if ((instance instanceof MaxFtsData) && (instance.getDataSource() instanceof MaxFtsDataSource))
      {
	/* Open the stream, put the "jmax" header, and then save
	   the patcher inside
	   */

	FtsLocation location = ((MaxFtsDataSource) instance.getDataSource()).getFtsLocation();

	FtsObject object = location.getObject();

	if (object != null)
	  {
	    ((MaxFtsData) instance).saveToFtsObject();
	  }
	else
	  throw new MaxDataException("Object " + location + " do not exists");
      }
    else
      throw new MaxDataException("Cannot save a " + instance.getDataType() + " to FTS");
  }

  /** Return true if this Data Handler can save a given instance
    to the given source.
    */

  public boolean canSaveTo(MaxDataSource source, MaxData instance)
  {
    return super.canSaveTo(source) && instance instanceof MaxFtsData;
  }
}








