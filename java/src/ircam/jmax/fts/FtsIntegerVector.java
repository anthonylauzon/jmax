package ircam.jmax.fts;

import java.io.*;


/** This class represent an Integer vector in 
 *  FTS
 * @@@ The FTS connection is missing
 * @@@ the size should be handled somewhere; there should 
 * be a dinam
 */

class FtsIntegerVector
{
  FtsObject object; 
  int[] values = null;

  FtsIntegerVector()
  {
  }

  /** Get the vector size */

  public int size()
  {
    if (values != null)
      return values.length;
    else
      return 0;
  }

  /** set the vector size 
   */

  void setSize(int size)
  {
    if (values == null)
      values = new int[size];
    else if (size > values.length)
      {
	int newValues[];

	newValues = new int[size];

	System.arraycopy(values, 0, newValues, 0, values.length);

	values = newValues;
      }
    else if (size < values.length)
      {
	int newValues[];

	newValues = new int[size];

	System.arraycopy(values, 0, newValues, 0, newValues.length);

	values = newValues;
      }
    else if (size == values.length)
      {
	// Nothing to do
      }
  }


  /** Set the corresponding FTS objetc */

  void setObject(FtsObject object)
  {
    this.object = object;
  }

  /** Get the values  */

  public int[] values()
  {
    return values;
  }

  /** Get the whole content  of the vector from FST */

  public void forceUpdate()
  {
    FtsServer.getServer().sendObjectMessage(object, -1, "update", null);
  }

  /** Declare that a range in the vector has been changed
   * and this range need to be sent to FTS
   */

  public void changed(int from, int to)
  {
    // Mandare un messaggio _set all'oggetto
    // primo argomento l'offset, e poi i valori.

    FtsServer.getServer().sendSetMessage(object, from, values, from, to);
  }

  /** Declare that a value in the vector has been changed
      and this value need to be sent to FTS
      */

  public void changed(int idx)
  {
    changed(idx, idx);
  }

  /** Declare that all the  values in the vector has been changed
      and should be sent to FTS
      */

  public void changed()
  {
    changed(0, values.length - 1);
  }

  /** Saving as tcl, both embedded in a .tpa or in a table file*/

  public void saveAsTcl(PrintWriter pw)
  {
    // To be defined
  }
}
