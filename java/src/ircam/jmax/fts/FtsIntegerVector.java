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
package ircam.jmax.fts;

import java.io.*;
import java.util.*;

import ircam.jmax.mda.*;
import ircam.jmax.utils.*;

/** This class represent an Integer vector in 
 *  FTS
 */

public class FtsIntegerVector extends FtsRemoteUndoableData
{
  /** Key for remote calls */

  static final int REMOTE_SET     = 1;
  static final int REMOTE_UPDATE  = 2;
  static final int REMOTE_NAME    = 3;

  // The name of this data; actually, this should be defined in a subclass,
  // not all the integer vectors are named

  String name;
  protected int[] values = null;

  public FtsIntegerVector()
  {
    super();
  }
  /** Get the vector name */

  public String getName()
  {
    return name;
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
    remoteCall(REMOTE_UPDATE);
    fts.sync();
  }

  /** Declare that a range in the vector has been changed
   * and this range need to be sent to FTS
   */

  public void changed(int from, int howMany)
  {
    remoteCall(REMOTE_SET, from, howMany, values);
  }

  /** Declare that a value in the vector has been changed
      and this value need to be sent to FTS
      */

  public void changed(int idx)
  {
    changed(idx, 1);
  }

  /** Declare that all the  values in the vector has been changed
      and should be sent to FTS
      */


  public void changed()
  {
    changed(0, values.length);
  }

  /* a method inherited from FtsRemoteData */

  public final void call( int key, FtsStream stream)
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    switch( key)
      {      
      case REMOTE_NAME:
	name = stream.getNextStringArgument();
	break;
      case REMOTE_SET:
	int size;
	
	size = stream.getNextIntArgument();

	if ((values == null) || size != values.length)
	  values = new int[size];
	
	for (int i = 0 ; i < size; i++)
	  values[i] = stream.getNextIntArgument();
	break;
      default:
	break;
      }
  }
}


