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
    setPrettyName("patcher");
  }

  /** Build an empty instance of FtsPatcherDocument, i.e. an instance
   * corresponding to an empty, new, patcher
   */

  public MaxDocument newDocument()
  {
    // Build a new FtsObject, a patcher 0 in 0 out

    FtsContainerObject patcher;

    try
      {
	patcher = (FtsContainerObject) Fts.makeFtsObject(Fts.getServer().getRootObject(), "patcher", "unnamed 0 0");

	// Put some geometrical property for the window, so we can see it.

	patcher.put("wx", 100);
	patcher.put("wy", 100);
	patcher.put("ww", 500);
	patcher.put("wh", 480);

	// Put a new empty patch in edit mode

	patcher.put("initialMode", "edit");

	FtsPatcherDocument document = new FtsPatcherDocument();

	document.setRootData(patcher);
	document.setName(MaxWindowManager.getWindowManager().makeUniqueWindowTitle("untitled")); // temp name

	return document;
      }
    catch (FtsException e)
      {
	return null;
      }
  }

  public boolean canMakeNewDocument()
  {
    return true;
  }

  /** For the moment, we don't do a patch document from a data;
    in other terms, we cannot yet do a "saveCopyAs" on a subpatch
    Soon ...
    */
}





