package ircam.jmax.mda; 

import java.io.*;

/** A Data Handler is an object able to load a Max Data instance
 *  from a file data source.
 */

abstract public class MaxFileDataHandler extends MaxDataHandler
{
  /** Return true if this Data Handler can load
    from the given source; by default, return true
    if the file exists, and is readable; subclasses
    should add canLoadFrom(File file) implementation
    to do further checks on the file if needed.
    */

  public boolean canLoadFrom(MaxDataSource source)
  {
    if (source instanceof MaxFileDataSource)
      {
	File file = ((MaxFileDataSource)source).getFile();

	return file.canRead() && canLoadFrom(file);
      }
    else
      return false;
  }

  /** Return true if this Data Handler can save
    to the given source; by default, return true
    if the file is writable; subclasses
    should add canSaveTo(File file) implementation
    to do further checks on the file if needed.
    */

  public boolean canSaveTo(MaxDataSource source)
  {
    if (source instanceof MaxFileDataSource)
      {
	File file = ((MaxFileDataSource)source).getFile();

	return ((! file.exists()) || file.canWrite()) && canSaveTo(file);
      }
    else
      return false;
  }


  /** Return true if we can load from the given file.
   * By default return true, should be specialized by subclasses.
   */

  protected boolean canLoadFrom(File file)
  {
    return true;
  }


  /** Return true if we can save to the given file.
   * By default return true, should be specialized by subclasses.
   */

  protected boolean canSaveTo(File file)
  {
    return true;
  }


  /** Return true if this Data Handler can load
    from the given address */

  public MaxData loadInstance(MaxDataSource source) throws MaxDataException
  {
    MaxFileDataSource fileSource = (MaxFileDataSource) source;
    File file = fileSource.getFile();
    MaxData obj;

    obj = makeInstance(file);
    
    obj.setDataHandler(this);
    obj.setDataSource(source);

    return obj;
  }

  /** Actually build and return an Max Data from a file */

  abstract protected MaxData makeInstance(File file) throws MaxDataException;
}

