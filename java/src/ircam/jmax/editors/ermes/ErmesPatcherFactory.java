
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
    if (theData.getName()!=null) //loading
      aSketchWindow.setRunMode(true);//bug24

    MaxApplication.itsSketchWindowList.addElement(aSketchWindow);
    MaxApplication.AddThisWindowToMenus(aSketchWindow);

    return aSketchWindow;
  }
}






