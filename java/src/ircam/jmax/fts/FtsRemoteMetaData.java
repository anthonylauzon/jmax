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

  private void newFtsRemoteData( FtsMessage msg)
  {
    int newId = ((Integer)msg.getArgument(2)).intValue();
    String className = (String)msg.getArgument(3);
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

  public final void call( int key, FtsMessage msg)
  {
    switch( key) {
    case 1:
      newFtsRemoteData(msg);
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





