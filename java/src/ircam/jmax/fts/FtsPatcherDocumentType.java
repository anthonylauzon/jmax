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

  public MaxDocument newDocument()
  {
    // Build a new FtsObject, a patcher 0 in 0 out

    FtsObject patcher;

    try
      {
	patcher = Fts.makeFtsObject(Fts.getServer().getRootObject(), "jpatcher");


	// Put a new empty patch in edit mode

	FtsPatcherDocument document = new FtsPatcherDocument();

	patcher.updateData();
	Fts.sync();

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





