package ircam.jmax.mda; 

import tcl.lang.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;
import java.io.*;

/** An instance of this data handler can load MaxData from
 *  a tcl string obeyng the "jmax" command conventions
 * 
 * A TCL Data string starts with the jmax command
 * at the *beginning* of the first line.
 * Class added by Enzo.
 */

public class MaxTclStringDataHandler extends MaxStringDataHandler
{
  public MaxTclStringDataHandler()
  {
    super();
  }

  /** We can load from a stream that starts with the "jmax " string*/

  protected boolean canLoadFrom(StringBuffer string)
  {
    try
      {
	StringReader sr = new StringReader(string.toString());

	char buf[] = new char[5];
    
	sr.read(buf);
	sr.close();
	
	if ((new String(buf)).equals("jmax "))
	  return true;
	else
	  return false;
      }
    catch (IOException e)
      {
	return false;
      }
  }

  /** Make the real instance */

  protected MaxData makeInstance(StringBuffer string) throws MaxDataException
  {
    Interp interp = MaxApplication.getTclInterp();

    try
      {
	interp.eval(string.toString());
      }
    catch (TclException e)
      {
	throw new MaxDataException("Tcl error: " + interp.getResult());
      }

    try
      {
	return (MaxData) ReflectObject.get(interp, interp.getResult());
      }
    catch (TclException e)
      {
	throw new MaxDataException("Format error loading string ");
      }
  }

  /** Save: basic support for saving a tcl file in the "jmax" format 
   * jmax <doc-type> [<version>] <name> <info> { <doc-body> }
   * It use a "IndentOutputStream", but the data method should make
   * sure that they can save to a normal stream, also.
   * This version do not save a version number...
   */

  public void saveInstance(MaxData instance) throws MaxDataException
  {
    /* Open the stream, put the "jmax" header, and then save
       the patcher inside. Concatenate the resulting string..
       */

    StringBuffer string = ((MaxStringDataSource) instance.getDataSource()).getString();

    try
      {
	StringWriter stream  = new StringWriter();
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
	string.append(stream.toString());//WARN
      }
    catch (Exception e)
      {
	e.printStackTrace();
	//throw new MaxDataException("I/O error saving " + instance);
      }
  }
}







