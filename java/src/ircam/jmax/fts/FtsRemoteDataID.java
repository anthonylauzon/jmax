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

import java.lang.*;
import java.util.*;

/** Remote class id table */

class FtsRemoteDataID
{
  private static final int DEFAULT_LENGTH = 128;

  class FtsSimpleTable
  {
    public FtsSimpleTable()
    {
      this( DEFAULT_LENGTH);
    }

    public FtsSimpleTable( int length)
    {
      table = new Object[length];
    }

    public Object get( int id) throws IndexOutOfBoundsException
    {
      if ( id < 0 && id >= table.length)
	throw new IndexOutOfBoundsException();
 
      return table[id];
    }

    public void put( int id, Object obj) throws IndexOutOfBoundsException
    {
      if ( id < 0)
	throw new IndexOutOfBoundsException();

      if ( id >= table.length)
	growTable( id);

      table[id] = obj;
    }

    protected void growTable( int id)
    {
      Object newTable[];

      int newLength = table.length;

      while ( newLength <= id)
	newLength *= 2;

      newTable = new Object[newLength];

      System.arraycopy( table, 0, newTable, 0, table.length);

      table = newTable;
    }

    protected Object table[];
  }


  static int ftsDataIDCounter = 3;	// Skip zero and one, use odd numbers

  public int getNewDataID()
  {
    int newid;

    newid = ftsDataIDCounter;
    ftsDataIDCounter += 2;

    return newid;
  }

  public FtsRemoteData get( int id)
  {
    FtsRemoteData data = null;

    try
      {
	data = (FtsRemoteData)dataTable.get( id);
      }
    catch ( IndexOutOfBoundsException e)
      {
      }

    return data;
  }

  public void put( int id, FtsRemoteData data)
  {
    try
      {
	dataTable.put( id, data);
      }
    catch ( IndexOutOfBoundsException e)
      {
	System.err.println( "FtsRemoteDataID::put: invalid id " + id);
      }
  }

  public void release( int id)
  {
    try
      {
	dataTable.put( id, null);
	  
	if (id == ftsDataIDCounter)
	  while ((ftsDataIDCounter >= 3) && (dataTable.get(ftsDataIDCounter) != null))
	    ftsDataIDCounter -= 2;
      }
    catch ( IndexOutOfBoundsException e)
      {
	System.err.println( "FtsRemoteDataID::put: invalid id " + id);
      }
  }

  protected FtsSimpleTable dataTable = new FtsSimpleTable();
}







