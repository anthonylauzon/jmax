//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.mda.tcl;

import tcl.lang.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/**
 * This command install a named tcl function as "editor factory"
 * for a named Max Document type; this function should create an editor
 * for it.
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










  
