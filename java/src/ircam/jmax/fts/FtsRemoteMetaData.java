package ircam.jmax.fts;

import java.lang.*;
import java.io.*;

class FtsRemoteMetaData extends FtsRemoteData {

  public static void install()
    {
      System.err.println( "Ouf ! installed");

      FtsRemoteDataID.put( 1, new FtsRemoteMetaData());
    }

  protected FtsRemoteMetaData()
    {
      super();
    }

  private void newFtsRemoteData( Object args[])
  {
    int newId = ((Integer)args[0]).intValue();
    String className = (String)args[1];
    Class dataJavaClass;
    FtsRemoteData newRemoteData;

    try
      {
	dataJavaClass = Class.forName( className);
      }
    catch( java.lang.ClassNotFoundException e)
      {
	System.err.println( e + " cannot load class " + className);
	return;
      }

    try
      {
	newRemoteData = (FtsRemoteData) dataJavaClass.newInstance();
      }
    catch( java.lang.IllegalAccessException e)
      {
	System.err.println( e + " in " + dataJavaClass.getName());
	return;
      }
    catch( java.lang.InstantiationException e)
      {
	System.err.println( e + " in " + dataJavaClass.getName());
	return;
      }

    newRemoteData.setId( newId);
    FtsRemoteDataID.put( newId, newRemoteData);
  }

  public final void call( int key, Object args[])
    {
      switch( key) {
      case 1:
	newFtsRemoteData( args);
	break;
      default:
	break;
      }
    }
}

