package ircam.jmax.fts;

import java.lang.*;
import java.util.*;

class FtsSimpleTable {

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

  protected static final int DEFAULT_LENGTH = 128;
}

class FtsRemoteDataID {

  public static FtsRemoteData get( int id)
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

  public static void put( int id, FtsRemoteData data)
    {
      try
	{
	  dataTable.put( id, (Object)data);
	}
      catch ( IndexOutOfBoundsException e)
	{
	  System.err.println( "FtsRemoteDataID::put: invalid id " + id);
	}
    }

  protected static FtsSimpleTable dataTable;

  static
    {
      dataTable = new FtsSimpleTable();
    }
}
