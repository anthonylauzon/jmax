
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

  private TableSelection(TableDataModel model) 
  {
    this.model = model;
    setSelectionMode(ListSelectionModel.SINGLE_INTERVAL_SELECTION) ;

    itsCopy = new int[model.getSize()];
    if (flavors == null)
      flavors = new DataFlavor[1];
    flavors[0] = tableSelection;
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
  }

  /**
   * static constructor
   */
  static public TableSelection createSelection(TableDataModel ep) 
  {
    if (itsSelection == null) 
      {
	itsSelection = new TableSelection(ep);
      }

    return itsSelection;
  }

  /**
   * returns the (unique) selection
   */
  public static TableSelection getSelection()
  {
    return itsSelection;
  }  


  /** Transferable interface */
  public Object getTransferData(DataFlavor flavor) 
  {
    return itsCopy; 
  }


  /**
   * Function used by clipboard operations.
   * This method stores the values currently selected in a local copy.
   */ 
  void prepareACopy()
  {
    if (isSelectionEmpty()) return;

    int base = getFirstSelected();

    for (int i = base; i<=getLastSelected(); i++)
      {
	itsCopy[i-base] =model.getValue(i); 
      }
  }


  /**
   * Transferable interface */
  public DataFlavor[]  getTransferDataFlavors() 
  {
    return flavors;
  }

  /**
   * Transferable interface */
  public boolean isDataFlavorSupported(DataFlavor flavor) 
  {
    return flavor.equals(tableSelection);
  } 

  //--- Fields
  TableDataModel model;
  private static TableSelection itsSelection;
  private static int itsCopy[];
  protected MaxVector dataFlavors;
  private MaxVector temp = new MaxVector();

  public static DataFlavor tableSelection = new DataFlavor(ircam.jmax.editors.table.TableSelection.class, "TableSelection");
  public static DataFlavor flavors[];
}


