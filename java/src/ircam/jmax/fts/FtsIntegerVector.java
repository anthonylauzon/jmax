package ircam.jmax.fts;

import java.io.*;
import java.util.*;

import ircam.jmax.mda.*;
import ircam.jmax.utils.*;

/** This class represent an Integer vector in 
 *  FTS
 */

public class FtsIntegerVector extends FtsRemoteData
{
  /** Key for remote calls */

  static final int REMOTE_SET     = 1;
  static final int REMOTE_UPDATE  = 2;

  int[] values = null;

  public FtsIntegerVector()
  {
    super();
  }

  /** SHould go away !!! */

  public String getName()
  {
    return "table";
  }

  /** Get the vector size */

  public int getSize()
  {
    if (values != null)
      return values.length;
    else
      return 0;
  }


  public void setSize(int size)
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


  /** Get the values  */

  public int[] getValues()
  {
    return values;
  }

  /** Get the whole content  of the vector from FST */

  public void forceUpdate()
  {
    remoteCall(REMOTE_UPDATE, null);
    Fts.sync();
  }

  /** Declare that a range in the vector has been changed
   * and this range need to be sent to FTS
   */

  public void changed(int from, int to)
  {
    remoteCall(REMOTE_SET, from, to - from + 1, values);
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

  /* a method inherited from FtsRemoteData */

  public final void call( int key, FtsMessage msg)
  {
    switch( key)
      {
      case REMOTE_SET:
	int size;
	
	size = ((Integer) msg.getArgument(2)).intValue();

	if ((values == null) || size != values.length)
	  values = new int[size];
	
	for (int i = 3 ; i < size + 2; i++)
	  values[i - 3] = ((Integer) msg.getArgument(i)).intValue();
	break;
      default:
	break;
      }
  }
}


