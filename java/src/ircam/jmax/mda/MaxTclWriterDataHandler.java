package ircam.jmax.mda; 

import java.io.*;
import tcl.lang.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;


/** An instance of this data handler can write MaxData to
 *  a tcl syntax output writer; tipically used to save embedded
 *  data objects in .tpa
 * 
 * Of course, this handler can only read files.
 */

public class MaxTclWriterDataHandler extends MaxDataHandler
{
  public MaxTclWriterDataHandler()
  {
    super();
  }

  /** We can load from a file start with the "jmax " string*/

  public boolean canLoadFrom(MaxDataSource source)
  {
    return false;
  }

  /** Make the real instance */

  protected MaxData loadInstance(MaxDataSource source) throws MaxDataException
  {
    throw new MaxDataException("Cannot load instances");
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
    if ((instance instanceof MaxTclData) && (instance.getDataSource() instanceof MaxWriterDataSource))
      {
	Writer writer = (( MaxWriterDataSource) instance.getDataSource()).getWriter();

	/* Open the stream, put the "jmax" header, and then save
	   the patcher inside
	   */

	IndentedPrintWriter pw = new IndentedPrintWriter(writer); // should be an "indent print writer"
	    
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
    else
      throw new MaxDataException("Cannot save a " + instance.getDataType() + " as TCL");
  }

  /** Return true if this Data Handler can save a given instance
    to the given source.
    */

  public boolean canSaveTo(MaxDataSource source, MaxData instance)
  {
    return super.canSaveTo(source) && instance instanceof MaxTclData;
  }
}



