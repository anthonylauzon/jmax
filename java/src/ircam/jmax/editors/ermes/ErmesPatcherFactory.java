
package ircam.jmax.editors.ermes;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
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
    ErmesSketchWindow aSketchWindow = new ErmesSketchWindow((FtsContainerObject) theData);
    String mode;

    /* To set the initial state: set to edit mode only if the
       initialMode property of a patcher is set and it is set
       to something different than "run" (usually, "edit" :)
       */

    mode = (String) ((FtsContainerObject)theData).get("initialMode");

    if ((mode == null) || mode.equals("run"))
      aSketchWindow.setRunMode(true);

    return aSketchWindow;
  }
}






