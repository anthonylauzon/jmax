
package ircam.jmax.editors.ermes;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;
/**
 * The factory of patcher editors...
 */
public class ErmesPatcherFactory implements MaxDataEditorFactory {

  public boolean canEdit(MaxData data)
  {
    return data instanceof FtsContainerObject;
  }

  /**
   * creates a new instance of patcher editor starting from the MaxDocument to edit
   */

  public MaxDataEditor newEditor(MaxData theData) {

    GlobalProbe.enterMethod( this, "newEditor"); // (fd)

    ErmesSketchWindow aSketchWindow = new ErmesSketchWindow((FtsContainerObject) theData);
    String mode;
    FtsContainerObject p;

    /* To set the initial state: set to edit mode only if the
       initialMode property of a patcher is set and it is set
       to something different than "run" (usually, "edit" :)
       */

    p = (FtsContainerObject) theData;

    mode = (String) p.get("initialMode");

    p = p.getParent();
    while ((mode == null) && (p != null))
      {
	mode = (String) p.get("editMode");
	p = p.getParent();
      }

    if ((mode == null) || mode.equals("run"))
      aSketchWindow.setRunMode(true);

    GlobalProbe.exitMethod(); // (fd)
    GlobalProbe.report(); // (fd)

    return aSketchWindow;
  }
}






