package ircam.jmax.editors.qlist;

import ircam.jmax.mda.*;
import ircam.jmax.fts.*;


public class QListFactory implements MaxDataEditorFactory {

  public boolean canEdit(MaxData data)
  {
    return data instanceof FtsAtomList;
  }
  
  public MaxDataEditor newEditor(MaxData theData) {
    return new QList(theData);
  }
}



