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

	// Put some geometrical property for the window, so we can see it.
	// Do both a put and a localPut but it should be a series of put/ask
	// pair.

	patcher.put("wx", 100);
	patcher.localPut("wx", 100);
	patcher.put("wy", 100);
	patcher.localPut("wy", 100);
	patcher.put("ww", 500);
	patcher.localPut("ww", 500);
	patcher.put("wh", 480);
	patcher.localPut("wh", 480);

	// Put a new empty patch in edit mode

	patcher.put("initialMode", "edit");
	patcher.localPut("initialMode", "edit");

	FtsPatcherDocument document = new FtsPatcherDocument();

	patcher.ask("data");
	Fts.sync();
	document.setRootData(patcher.getData());
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





