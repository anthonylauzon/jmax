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
package ircam.jmax.mda.tcl;

import tcl.lang.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/**
 * This tcl function support a standard syntax to create
 * Max document as tcl files; obsolete, left as an example
 * for the migration to Scheme.
 * 
 * The Command Syntax is : <p>
 *
 * <code>
 *     jmax <i> doc-type [version] name info doc-body
 * </code>
 *
 * ^^^^ The documen is created in the default server.
 * ^^^^ There should be a way to pass on the right server to scripting
 * ^^^^ Commands.
 */

class JMaxCmd implements Command
{
  /** Method implementing the TCL command */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if ((argv.length == 5) || (argv.length == 6))
      {
	String docType;
	String version; // ignored in this implementation
	String name;
	String info;
	TclObject body;
	MaxDocument document;

	if (argv.length == 5)
	  {
	    docType = argv[1].toString();
	    name    = argv[2].toString();
	    info    = argv[3].toString();
	    body    = argv[4];
	  }
	else
	  {
	    docType = argv[1].toString();
	    version = argv[2].toString();
	    name    = argv[3].toString();
	    info    = argv[4].toString();
	    body    = argv[5];
	  }

	// Create a new instance of the type

	document = Mda.getDocumentTypeByName(docType).newDocument(MaxApplication.getFts());

	if (document instanceof MaxTclDocument)
	  {
	    // Set the name and info

	    document.setName(name);

	    // Eval the body inside the document 

	    ((MaxTclDocument) document).eval(interp, body);

	    // Finally, return the document to the tcl interpreter
    
	    interp.setResult(ReflectObject.newInstance(interp, document));
	  }
	else
	  new TclException(interp, docType + " is not a tcl based jMax Document");
      }
    else
      throw new TclNumArgsException(interp, 1, argv, "doc-type [version] name info doc-body");
  }
}










  
