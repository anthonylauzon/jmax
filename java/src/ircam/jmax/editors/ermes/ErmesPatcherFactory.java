
package ircam.jmax.editors.ermes;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
/**
 * The factory of patcher editors...
 */
public class ErmesPatcherFactory implements MaxDataEditorFactory {
  /**
   * creates a new instance of patcher editor satrting from the MaxData to edit
   */
  public MaxDataEditor newEditor(MaxData theData) {
    ErmesSketchWindow aSketchWindow = MaxApplication.NewPatcherWindow((FtsContainerObject)theData.getContent());
    return aSketchWindow;
  }
}



