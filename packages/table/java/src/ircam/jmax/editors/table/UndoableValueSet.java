//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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
