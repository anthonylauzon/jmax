package ircam.jmax.editors.explode;

import ircam.jmax.mda.*;
import ircam.jmax.fts.*;

/**
 * the MaxDataEditorFactory specialized to build explode editors
 */
public class ExplodeFactory implements MaxDataEditorFactory {

  /**
   * overrides MaxDataEditorFactory.canEdit()
   */
  public boolean canEdit( MaxData maxData)
  {
    return maxData instanceof ExplodeRemoteData;
  }
  
  /**
   * overrides MaxDataEditorFactory.newEditor()
   */
  public MaxDataEditor newEditor( MaxData maxData) 
  {
    return new ExplodeDataEditor((ExplodeRemoteData) maxData);
  }
}




