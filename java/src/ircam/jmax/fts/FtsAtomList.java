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

/** This class represent an Integer list in 
 *  FTS
 */

public class FtsAtomList extends FtsRemoteData
{
  /** Key for remote calls */

  static final int REMOTE_UPDATE = 1;
  static final int REMOTE_SET    = 2;

  MaxVector values = new MaxVector();

  public FtsAtomList()
  {
    super();
  }

  public int getSize()
  {
    return values.size();
  }

  public MaxVector getValues()
  {
    return values;
  }

  /** Get the list in text form; it should use the FtsParse unparser !!*/

  public String getValuesAsText()
  {
    return FtsParse.unparseDescription(values);
  }

  /** Set the list as text */
  
  public void setValuesAsText(String value)
  {
    values.removeAllElements();
    FtsParse.parseAtoms(value, values);
    changed();
  }

  /** Get the whole content  of the list from FTS */

  public void forceUpdate()
  {
    remoteCall(REMOTE_UPDATE);
    Fts.sync();
  }

  /** Declare that a range in the list has been changed
   * and this range need to be sent to FTS
   */

  public void changed()
  {
    remoteCall(REMOTE_SET, values);
  }


  /* a method inherited from FtsRemoteData */

  public final void call( int key, FtsStream stream)
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    switch( key)
      {
      case REMOTE_SET:

	values.removeAllElements();

	while (! stream.endOfArguments())
	  values.addElement(stream.getNextArgument());

	break;
      default:
	break;
      }
  }
}


