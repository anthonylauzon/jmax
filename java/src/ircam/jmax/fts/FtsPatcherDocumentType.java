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
package ircam.jmax.fts;

import java.util.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/** Class defining the fts patch type (named patch);
 */

public class FtsPatcherDocumentType extends MaxDocumentType
{
  public FtsPatcherDocumentType()
  {
    super("patcher");
    setPrettyName("Patcher");
  }

  /** Build an empty instance of FtsPatcherDocument, i.e. an instance
   * corresponding to an empty, new, patcher
   */

  public MaxDocument newDocument(MaxContext context)
  {
    // Should give an error !!

    if (! (context instanceof Fts))
      return null;

    Fts fts = (Fts) context;

    // Build a new FtsObject, a patcher 0 in 0 out

    FtsObject patcher;

    try
      {
	patcher = fts.makeFtsObject(fts.getServer().getRootObject(), "jpatcher");

	// Put a new empty patch in edit mode

	FtsPatcherDocument document = new FtsPatcherDocument(context);

	patcher.updateData();
	fts.sync();

	// Put some default geometrical property for the window.

	FtsPatcherData data = (FtsPatcherData) 	patcher.getData();

	data.setWindowX(100);
	data.setWindowY(100);
	data.setWindowWidth(500);
	data.setWindowHeight(480);
	data.setEditMode(FtsPatcherData.EDIT_MODE);

	document.setRootData(data);
	document.setName(MaxWindowManager.getWindowManager().makeUniqueWindowTitle("untitled")); // temp name

	return document;
      }
    catch (FtsException e)
      {
	System.err.println("Fts Exception " + e + "creating Patcher Document ?? ");
	return null;
      }
  }

  public boolean canMakeNewDocument()
  {
    return true;
  }

  public boolean canMakeSubDocumentFile(MaxData data)
  {
    return true;
  }
}





