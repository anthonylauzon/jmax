//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.editors.qlist;

import ircam.jmax.mda.*;
import ircam.jmax.fts.*;


public class QListFactory implements MaxDataEditorFactory {

  public boolean canEdit(MaxData data)
  {
    return data instanceof FtsAtomList;
  }
  
  public MaxDataEditor newEditor(MaxData theData) {
    return new QListDataEditor((FtsAtomList) theData);
  }
}



