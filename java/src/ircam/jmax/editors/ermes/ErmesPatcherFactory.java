
package ircam.jmax.editors.ermes;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
/**
 * The factory of patcher editors...
 */
public class ErmesPatcherFactory implements MaxDataEditorFactory {
  /**
   * creates a new instance of patcher editor starting from the MaxData to edit
   */
  public MaxDataEditor newEditor(MaxData theData) {
    ErmesSketchWindow aSketchWindow = new ErmesSketchWindow(theData);
    //ErmesSketchWindow aSketchWindow = new ErmesSketchWindow((FtsContainerObject)theData.getContent());

    MaxApplication.itsSketchWindowList.addElement(aSketchWindow);
    if (theData.getName() != null) aSketchWindow.setTitle(theData.getName());
    else { //data has no name (did you do a "new"?)
    }
    MaxApplication.AddThisWindowToMenus(aSketchWindow);

    return aSketchWindow;
  }
}






