package ircam.jmax.editors.table;

import ircam.jmax.mda.*;
import ircam.jmax.fts.*;


public class TableFactory implements MaxDataEditorFactory {

  public boolean canEdit(MaxData data)
  {
    return data instanceof FtsIntegerVector;
  }
  
  public MaxDataEditor newEditor(MaxData theData) {
    return new TableDataEditor((FtsIntegerVector) theData);
  }
}



