//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
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
// Based on Max/ISPW by Miller Puckette.
// 
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.script.tcl; 

import java.io.*;
import ircam.jmax.script.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;


/** 
 * A document handler capable to execute a document consisting of tcl commands.
 * 
 * ^^^^ As other tcl scripting related code, does not work with multiservers.
 */

public class MaxTclExeDocumentHandler extends MaxDocumentHandler
{
  public MaxTclExeDocumentHandler()
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
    Interpreter interp = MaxApplication.getInterpreter();

    try
      {
	interp.eval("sourceFile " + file.getPath());
      }
    catch (ScriptException e)
      {
	throw new MaxDocumentException("Tcl error: " + e.getMessage());
      }

    MaxDocument document = (MaxDocument) new MaxTclExeDocument(context);

    document.setDocumentFile(file);
    document.setDocumentHandler(this);

    return document;
  }

  /** We cannot save a executed document  */

  public void saveDocument(MaxDocument document, File file) throws MaxDocumentException
  {
    throw new MaxDocumentException("Cannot save a MaxTclExeDocument");
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



