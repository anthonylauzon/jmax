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

package ircam.jmax.fts;

import java.util.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/** Class defining the max Document type "fts patch".
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





