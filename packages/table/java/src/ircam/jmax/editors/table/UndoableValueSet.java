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

package ircam.jmax.editors.table;

import java.awt.*;
import java.util.*;

import javax.swing.undo.*;

/**
 * An UndoableEdit to undo a single value set in a table */
public class UndoableValueSet extends AbstractUndoableEdit {
 
  /**
   * Constructor with the data model, the index, and the value
   * that is going to be stored */
  public UndoableValueSet(TableDataModel tm, int index, int oldValue)
  {
    this.tm = tm;
    this.index = index;
    this.undoValue = oldValue;
  }  

  /**
   * Called to check if this edit can absorbe the given,
   * subsequent, edit */
  public boolean addEdit(UndoableEdit anEdit)
  {


    if (anEdit instanceof UndoableValueSet) 
      if ( ((UndoableValueSet) anEdit).index == index) 
	{
	  // 'this' is a previous action on the same index then anEdit,
	  // and in the same undoable section. We can absorbe the new one!
	  return true;
	}
      else 
	{
	  return false; //else don't absorbe
	}
    else
      {
	return false;
      }
  }


  /** 
   * The name of the edit */
  public String getPresentationName()
  {
    return "point set";
  }
  
  /**
   * Undo the trasformation */
  public void undo()
  {
    redoValue = tm.getValue(index);
    tm.setValue(index, undoValue);
  }

  /**
   * Redo the trasformation */
  public void redo()
  {
    undoValue = tm.getValue(index);
    tm.setValue(index, redoValue);
  }

  //--- Fields
  TableDataModel tm;
  int index;
  int undoValue;
  int redoValue;

  boolean alive = true;
}
