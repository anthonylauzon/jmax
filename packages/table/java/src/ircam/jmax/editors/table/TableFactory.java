package ircam.jmax.editors.table;

import ircam.jmax.mda.*;


public class TableFactory implements MaxDataEditorFactory {
  
  public MaxDataEditor newEditor(MaxData theData) {
    Tabler aTabler = new Tabler(theData);
    
    return aTabler;
  }
}



