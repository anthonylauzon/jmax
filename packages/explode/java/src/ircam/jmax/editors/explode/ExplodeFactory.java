package ircam.jmax.editors.explode;

import ircam.jmax.mda.*;
import ircam.jmax.fts.*;


public class ExplodeFactory implements MaxDataEditorFactory {

  public boolean canEdit( MaxData maxData)
  {
    return maxData instanceof ExplodeRemoteData;
  }
  
  public MaxDataEditor newEditor( MaxData maxData) 
  {
    return new Explode( maxData);
  }
}




