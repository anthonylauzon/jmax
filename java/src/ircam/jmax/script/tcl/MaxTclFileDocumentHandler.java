 //
// jMax
// Copyright (C) 1999 by IRCAM
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.script.tcl; 

// ^^^^ The loadDocument method should be changed to take into account
// ^^^^ the loading context (the Fts instance); usual scripting and multiserver
// ^^^^ problem.

import java.io.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.script.*;
import ircam.jmax.utils.*;


/** An instance of this document handler can load MaxDocument from
 *  a tcl file obeyng the "jmax" command conventions
 * 
 * A TCL Document file is a file that start with the jmax command
 * at the *beginning* of the first line.
 */

public class MaxTclFileDocumentHandler extends MaxDocumentHandler
{
  public MaxTclFileDocumentHandler()
  {
    super();
  }

  /** We can load from a file start with the "jmax " string*/

  public boolean canLoadFrom(File file)
  {
    if (super.canLoadFrom(file))
      {
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

  /** Make the real document */

    /* FIXME should use loadScriptedDocument from the Interpreter interface */

   protected MaxDocument loadDocument(MaxContext context, File file) throws MaxDocumentException
  {
    Interpreter interp = MaxApplication.getInterpreter();
    Object result;
    try
      {
	result = interp.load(file);
      }
    catch (ScriptException e)
      {
	throw new MaxDocumentException("Tcl error: " + e.getMessage());
      }

    try
      {
	  /* MaxDocument document = (MaxDocument) ReflectObject.get(interp, interp.getResult()); */
	MaxDocument document = (MaxDocument) result;

	document.setDocumentFile(file);
	document.setDocumentHandler(this);

	return document;
      }
    catch (Exception e)
      {
	throw new MaxDocumentException("Format error loading file " + file);
      }
  }

  /** Save: basic support for saving a tcl file in the "jmax" format 
   * jmax <doc-type> [<version>] <name> <info> { <doc-body> }
   * It use a "IndentOutputStream", but the document method should make
   * sure that they can save to a normal stream, also.
   * This version do not save a version number.
   * For the moment (??), we only know how to save TCL to files.
   */

  public void saveDocument(MaxDocument document, File file) throws MaxDocumentException
  {
    if (document instanceof MaxTclDocument)
      {
	/* Open the stream, put the "jmax" header, and then save
	   the patcher inside
	   */

	try
	  {
	    FileOutputStream stream  = new FileOutputStream(file);
	    IndentedPrintWriter pw = new IndentedPrintWriter(stream); // should be an "indent print writer"
	    
	    pw.println("jmax " + document.getDocumentType().getName() +
		       " " + document.getName() +
		       " {} {");
	
	    pw.indentMore();
	    ((MaxTclDocument) document).saveContentAsTcl(pw);
	    pw.indentLess();
	    pw.println();
	    pw.println("}");
	    pw.close();
	  }
	catch (IOException e)
	  {
	    throw new MaxDocumentException("I/O error saving " + document + " to " + file);
	  }
      }
    else
      throw new MaxDocumentException("Cannot save a " + document.getDocumentType() + " as TCL file");
  }

  /** Return true if this Document Handler can save a given document
    to the given file.
    */

  public boolean canSaveTo(MaxDocument document, File file)
  {
    return false;

    // return (
    // (document instanceof MaxTclDocument) &&
    // super.canSaveTo(document, file));
  }

  public String getDescription()
  {
    return "Tcl Documents";
  }
}



