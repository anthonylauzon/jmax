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

import java.util.*;
import javax.swing.*;
import java.awt.datatransfer.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.utils.*;

/**
 * The Table selection. This class is implemented on the base of the
 * Swing's ListSelectionModel.
 * @see TableTablePanel */
public class TableSelection extends DefaultListSelectionModel implements Transferable, Cloneable{

  public TableSelection(TableDataModel model) 
  {
    this.model = model;
    setSelectionMode(ListSelectionModel.SINGLE_INTERVAL_SELECTION) ;

    itsCopy = new int[model.getSize()];
    if (flavors == null)
      flavors = new DataFlavor[1];
    flavors[0] = tableSelection;

    // the last created selection becomes the current one
    setCurrent(this);
  }
   

  /**
   * Ownership handling
   */
  public void setOwner(SelectionOwner so)
  {
    itsOwner = so;
  }

  /**
   * Ownership handling
   */
  public SelectionOwner getOwner()
  {
    return itsOwner;
  }

  /**
   * Returns the TableDataModel this selection refers to */
  public TableDataModel getModel()
  {
    return model;
  }

  /**
   * Sets the TableDataModel this selection refers to */
  public void setModel(TableDataModel m)
  {
    model = m;
  }


  /**
   * Sets the current active selection. This will send a 
   * selectionActivated message to that selection's owner, and a 
   * selectionDisactivated to the old selection's owner */
  public static void setCurrent(TableSelection s)
  {
    TableSelection temp = current;
    current = s;

    if (temp != null && temp.itsOwner != null)
      temp.itsOwner.selectionDisactivated();

    if (s != null && s.itsOwner != null)
      s.itsOwner.selectionActivated();

  }


  /** select the given object 
   */
  public void select(int index)
  {
    addSelectionInterval(index, index);
  }
  
  /** 
   * Select a subrange of the table. 
   */
  public void select(int index1, int index2)
  {
    addSelectionInterval(index1, index2);
  }

  /** 
   *remove the given object from the selection
   */
  public void deSelect(int index)
  {
    removeSelectionInterval(index, index);
  }


  /** remove the given enumeration of objects from the selection
   * When possible, use this method instead of
   * deselecting single objects.    */
  public void deSelect(int index1, int index2)
  {
    removeSelectionInterval(index1, index2);
  }


  /** returns true if the object is currently selected
   */
  public boolean isInSelection(int index)
  {
    return isSelectedIndex(index);
  }


  /** 
   * Returns the first selected index.
   */
  public int getFirstSelected()
  {

    return getMinSelectionIndex();
  }

  /** 
   * Returns the first selected index.
   */
  public int getLastSelected()
  {

    return getMaxSelectionIndex();
  }
  

  /** returns the number of objects in the selection
   */
  public  int size()
  {
    if (isSelectionEmpty()) return 0;
    else return getMaxSelectionIndex() - getMinSelectionIndex() + 1;
  }


  /** selects all the objects. 
   */
  public  void selectAll()
  {
    addSelectionInterval(0, model.getSize()-1);
  }

  /** deselects all the objects currently selected
   */
  public  void deselectAll()
  {
    clearSelection();
    setCaretPosition(NO_CARET);
  }


  /**
   * returns the (unique) selection
   */
  public static TableSelection getCurrent()
  {
    return current;
  }  


  /**
   * Function used by clipboard operations.
   * This method stores the values currently selected in a local copy.
   */ 
  void prepareACopy()
  {
    if (isSelectionEmpty()) 
      {
	base = EMPTY_CLIP;
	return;
      }

    base = getFirstSelected();
    lenght = getLastSelected()-base+1;

    int i;
    for (i = base; i<=getLastSelected(); i++)
      {
	itsCopy[i-base] =model.getValue(i); 
      }

    // pad the remaining with zeros
    int modelSize = model.getSize();
    for (int j = i+1; j < modelSize; j++)
      {
	itsCopy[j-base] = 0;
      }
  }

  public class TableClip {
    public TableClip(int content[], int base, int lenght)
    {
      this.content = content;
      this.base = base;
      this.lenght = lenght;
    }
    //---
    int content[];
    int base;
    int lenght;
  } 

  public void setCaretPosition(int index)
  {
    if (caretPosition != index)
      {
	caretPosition = index;
	fireValueChanged(index, index);
	// NOTE: sort of trick (well, not too dirty by the way...)
      }
  }  

  public int getCaretPosition()
  {
    return caretPosition;
  }

  /** Transferable interface */
  public Object getTransferData(DataFlavor flavor) 
  {
    if (base != EMPTY_CLIP)
      return new TableClip(itsCopy, base, lenght); 
    else return null;
  }

  /**
   * Transferable interface */
  public DataFlavor[]  getTransferDataFlavors() 
  {
    if (base == EMPTY_CLIP) return null;
    else return flavors;
  }

  /**
   * Transferable interface */
  public boolean isDataFlavorSupported(DataFlavor flavor) 
  {
    return (base != EMPTY_CLIP && flavor.equals(tableSelection));
  } 

  //--- Fields
  TableDataModel model;
  private static TableSelection current;
  private static int itsCopy[];
  private static int EMPTY_CLIP = -1;
  private static int base = EMPTY_CLIP;
  private static int lenght = 0;

  static int NO_CARET = -1;
  int caretPosition = NO_CARET;

  protected MaxVector dataFlavors;
  private MaxVector temp = new MaxVector();

  public static DataFlavor tableSelection = new DataFlavor(ircam.jmax.editors.table.TableSelection.class, "TableSelection");
  public static DataFlavor flavors[];

  private SelectionOwner itsOwner;
}


