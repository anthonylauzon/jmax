//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
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

/** 
 * The Java Implementation for the atom list remote data class.
 */

public class FtsAtomList extends FtsRemoteData
{
  /* Keys for remote calls */

  static final int REMOTE_UPDATE = 1;
  static final int REMOTE_SET    = 2;

  MaxVector values = new MaxVector();

  public FtsAtomList()
  {
    super();
  }

  /** Return the size of the atom list */

  public int getSize()
  {
    return values.size();
  }

  /** Get the content of the atom list as a MaxVector */

  public MaxVector getValues()
  {
    return values;
  }

  /** Get the content of the atom list as text. */

  public String getValuesAsText()
  {
    return FtsParse.unparseDescription(values);
  }


  /** Set the content of the atom list as text. */
  
  public void setValuesAsText(String value)
  {
    values.removeAllElements();
    FtsParse.parseAtoms(value, values);
    changed();
  }


  /** Update the content of the atom list from the server. */

  public void forceUpdate()
  {
    remoteCall(REMOTE_UPDATE);
    fts.sync();
  }


  /** 
   * Send the content of the atom list to the server.
   */

  public void changed()
  {
    remoteCall(REMOTE_SET, values);
  }


  /** Implement the remote calls from the server */

  protected final void call( int key, FtsStream stream)
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


