 //
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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







