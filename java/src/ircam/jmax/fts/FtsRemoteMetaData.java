package ircam.jmax.fts;

import java.lang.*;
import java.io.*;
import ircam.jmax.mda.*;

class FtsRemoteMetaData extends FtsRemoteData
{
  public static void install()
  {
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

    dataJavaClass = Fts.getRemoteDataClass(className);

    if (dataJavaClass == null)
      {
	System.err.println( "Don't find Java class for " + className);
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

  /* MaxData interface */

  /** Get the document this data belong to.
    Doesn't mind for the meta data, because it will never 
    be edited.
    */

  public MaxDocument getDocument()
  {
    return null;
  }

  /** Get the a name for this data, for UI purposes only */

  public String getName()
  {
    return "metaData";
  }
}





