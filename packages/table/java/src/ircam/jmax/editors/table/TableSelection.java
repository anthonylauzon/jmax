
package ircam.jmax.editors.table;

import java.awt.*;
import java.util.*;
import ircam.jmax.toolkit.*;

/**
 * The selection in a table.
 * In this implementation, it is represented as an array of booleans,
 * so the basic "object" to select is actually a range (represented as an awt Point). 
 * @see AbstractSelection 
 *
 * Implementation notes: The "array of booleans" model is inadequate if we need
 * an unique Table selection shared between multiple instances of Table editors.
 * This class needs to be substantially rewritten. */
public class TableSelection extends AbstractSelection {

  /**
   * Constructor */
  public TableSelection(TableDataModel t)
  {
    itsModel = t;
    field = new boolean[t.getSize()];
  }

  /**
   * Select the given range (awt.Point) */
  public void select(Object obj) 
  {
    rangeAssign(((Point)obj).x, ((Point)obj).y, true);

    notifyListeners(OBJECT_SELECTED);
  }
  
  /**
   * Utility function to assign a boolean value to a range of indexes */
  private void rangeAssign(int r1, int r2, boolean value)
  {
    if (r1 <0) r1 = 0;
    if (r2 >= itsModel.getSize()) r2 = itsModel.getSize()-1;
    for (int i = r1; i <= r2; i++)
      {
	field[i] = value;
      }
  }

  /**
   * Select the given enumeration of objects 
   */
  public void select(Enumeration e) 
  {
    while(e.hasMoreElements())
      {
	select(e.nextElement());
      }
  }

  /**
   * Selects all the table */
  public void selectAll()
  {
    rangeAssign(0, itsModel.getSize()-1, true);

    notifyListeners(OBJECT_SELECTED);
  }
  
  /**
   * remove the object from the selection
   */
  public void deSelect(Object obj) 
  {
    rangeAssign(((Point)obj).x, ((Point)obj).y, false);
    notifyListeners(OBJECT_DESELECTED);
  }


  /**
   * remove the Enumeration of object from the selection
   */
  public void deSelect(Enumeration e) 
  {
    while(e.hasMoreElements())
      {
	deSelect(e.nextElement());
      }
  }

  /**
   * Returns true if the given range is currently selected */
  public boolean isInSelection(Object obj)
  {
    int index1 = ((Point)obj).x;
    int index2 = ((Point)obj).y;
    boolean run = true;

    for (int i = index1; i <= index2 && run; i++)
      {
	run = run && field[i];
      }
    return run;
  }

  /**
   * returns an enumeration of all the selected objects.
   * This means in this case a set of selected ranges.
   */
  public Enumeration getSelected() 
  {
    return new RangeSelection();
  }

  /**
   * A class to efficiently implement the getSelected call */
  class RangeSelection implements Enumeration {
    
    public RangeSelection()
    {
      pointer = findNextTrue(0);
    }

    public boolean hasMoreElements()
    {
      return pointer < itsModel.getSize();
    }

    public Object nextElement()
    {
      int start = pointer;

      while(field[pointer] && pointer<itsModel.getSize())
	pointer++;

      tempPoint.setLocation(start, pointer-1);

      // move the pointer to the next group
      pointer = findNextTrue(pointer);

      return tempPoint;
    }

    private int findNextTrue(int idx)
    {
      while(idx < itsModel.getSize() && !field[idx])
	idx++;
      return idx;
    }
  
    //-- Fields
    Point tempPoint = new Point();
    int pointer;
  }


  /**
   * almost meaningless/ambiguous for this kind of selection
   * (how many selected points or how many selected ranges?)
   * return always 1... */
  public int size()
  {
    return 1;
  }

  /**
   * deselects all the objects
   */
  public void deselectAll() 
  {
    rangeAssign(0, itsModel.getSize()-1, false);
    notifyListeners(OBJECT_DESELECTED);
  }

  /**
   * statically creates a TableSelection. See the implementation notes*/

  static TableSelection createSelection(TableDataModel m)
  {
    instance = new TableSelection(m);
    return instance;
  }

  //--- Fields & accessors
  TableDataModel itsModel;
  boolean field[];
  static TableSelection instance;

  public static TableSelection getSelection()
  {
    return instance;
  }
}
