package ircam.jmax.mda; 

import java.io.*;
import tcl.lang.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;


/** An instance of this data handler can load MaxData from
 *  a tcl file obeyng the "jmax" command conventions
 * Now support only files.
 * 
 * A TCL Data file is a file that start with the jmax command
 * at the *beginning* of the first line.
 */

public class MaxTclFileDataHandler extends MaxDataHandler
{
  public MaxTclFileDataHandler()
  {
    super();
  }

  /** We can load from a file start with the "jmax " string*/

  public boolean canLoadFrom(MaxDataSource source)
  {
    if ((source instanceof MaxFileDataSource) && super.canLoadFrom(source))
      {
	File file = ((MaxFileDataSource) source).getFile();

	try
	  {
	    FileReader fr = new FileReader(file);

	    char buf[] = new char[5];
    
	    fr.read(buf);
	    fr.close();

	    if ((new String(buf)).equals("jmax "))
	      return true;
	    else
	      return false;
	  }
	catch (FileNotFoundException e)
	  {
	    return false;
	  }
	catch (IOException e)
	  {
	    return false;
	  }
      }
    else
      return false;
  }

  /** Make the real instance */

   protected MaxData loadInstance(MaxDataSource source) throws MaxDataException
  {
    File file = ((MaxFileDataSource) source).getFile();
    Interp interp = MaxApplication.getTclInterp();

    try
      {
	interp.evalFile(file.getPath());
      }
    catch (TclException e)
      {
	throw new MaxDataException("Tcl error: " + interp.getResult());
      }

    try
      {
	MaxData data = (MaxData) ReflectObject.get(interp, interp.getResult());

	data.setDataSource(source);
	data.setDataHandler(this);

	return data;
      }
    catch (TclException e)
      {
	throw new MaxDataException("Format error loading file " + file);
      }
  }

  /** Save: basic support for saving a tcl file in the "jmax" format 
   * jmax <doc-type> [<version>] <name> <info> { <doc-body> }
   * It use a "IndentOutputStream", but the data method should make
   * sure that they can save to a normal stream, also.
   * This version do not save a version number.
   * For the moment (??), we only know how to save TCL to files.
   */

  public void saveInstance(MaxData instance) throws MaxDataException
  {
    if ((instance instanceof MaxTclData) && (instance.getDataSource() instanceof MaxFileDataSource))
      {
	/* Open the stream, put the "jmax" header, and then save
	   the patcher inside
	   */

	File file = ((MaxFileDataSource) instance.getDataSource()).getFile();

	try
	  {
	    FileOutputStream stream  = new 	FileOutputStream(file);
	    IndentedPrintWriter pw = new IndentedPrintWriter(stream); // should be an "indent print writer"
	    
	    pw.println("jmax " + instance.getDataType().getName() +
		       " " + instance.getName() +
		       " {" + instance.getInfo() +
		       "} {");
	
	    pw.indentMore();
	    ((MaxTclData) instance).saveContentAsTcl(pw);
	    pw.indentLess();
	    pw.println();
	    pw.println("}");
	    pw.close();
	  }
	catch (IOException e)
	  {
	    throw new MaxDataException("I/O error saving " + instance + " to " + file);
	  }

	{
	  /* Now, if the "bmax" system property is set, we do a wild
	     binary format save; highly experimental */

	  String bmaxSave = MaxApplication.getProperty("bmax");

	  if ((bmaxSave != null) && bmaxSave.equals("yes") && (instance instanceof ircam.jmax.fts.FtsPatchData))
	    ((ircam.jmax.fts.FtsPatchData) instance).saveBmax("/u/worksta/dececco/tmp/patch.bmax");
	}
      }
    else
      throw new MaxDataException("Cannot save a " + instance.getDataType() + " as TCL file");
  }

  /** Return true if this Data Handler can save a given instance
    to the given source.
    */

  public boolean canSaveTo(MaxDataSource source, MaxData instance)
  {
    return super.canSaveTo(source) && instance instanceof MaxTclData;
  }
}



