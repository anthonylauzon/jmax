package ircam.jmax.editors.qlist;

import ircam.jmax.mda.*;


public class QListFactory implements MaxDataEditorFactory {
  
  public MaxDataEditor newEditor(MaxData theData) {
    QList aQList = new QList(theData);
    
    return aQList;
  }
}



