//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.mda; 

import java.io.*;
import tcl.lang.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;


/** An instance of this document handler can load MaxDocument from
 *  a tcl file obeyng the "jmax" command conventions
 * Now support only files.
 * 
 * A TCL Document file is a file that start with the jmax command
 * at the *beginning* of the first line.
 * 
 * ^^^^ As other tcl scripting related code, do not work with multiservers.
 */

public class MaxTclExecutedDocumentHandler extends MaxDocumentHandler
{
  public MaxTclExecutedDocumentHandler()
  {
    super();
  }

  /** We can load from a file whose name end with .tcl */

  public boolean canLoadFrom(File file)
  {
    if (super.canLoadFrom(file))
      {
	return (file.getName().endsWith(".tcl") ||
		file.getName().endsWith(".env"));
      }
    else
      return false;
  }

  /** Make the real document */

  protected MaxDocument loadDocument(MaxContext context, File file) throws MaxDocumentException
  {
    Interp interp = MaxApplication.getTclInterp();

    try
      {
	interp.eval("sourceFile " + file.getPath());
      }
    catch (TclException e)
      {
	throw new MaxDocumentException("Tcl error: " + interp.getResult());
      }

    MaxDocument document = (MaxDocument) new MaxTclExecutedDocument(context);

    document.setDocumentFile(file);
    document.setDocumentHandler(this);

    return document;
  }

  public void saveDocument(MaxDocument document, File file) throws MaxDocumentException
  {
    throw new MaxDocumentException("Cannot save a MaxTclExecutedDocument");
  }

  /** Return true if this Document Handler can save a given document
    to the given file.
    */

  public boolean canSaveTo(MaxDocument document, File file)
  {
    return false;
  }

  public String getDescription()
  {
    return "Tcl Scripts";
  }
}



