package ircam.jmax.editors.ermes;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
import ircam.jmax.utils.*;

//
// The factory of patcher editors...
//
public class ErmesPatcherFactory implements MaxDataEditorFactory {

  public boolean canEdit(MaxData data)
  {
    return data instanceof FtsContainerObject;
  }

  //
  // Creates a new instance of patcher editor starting from the MaxDocument to edit
  //

  public MaxDataEditor newEditor(MaxData theData) 
  {
    return new ErmesDataEditor( (FtsContainerObject)theData);
  }
}
