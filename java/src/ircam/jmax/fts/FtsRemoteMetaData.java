package ircam.jmax.fts;

import java.lang.*;
import java.io.*;
import ircam.jmax.mda.*;

class FtsRemoteMetaData extends FtsRemoteData
{
  // Remote call codes

  static final int REMOTE_NEW     = 1;
  static final int REMOTE_DELETE  = 2;
  static final int REMOTE_RELEASE = 3;
 
  static private FtsRemoteMetaData metaData;

  public static void install()
  {
    metaData = new FtsRemoteMetaData();
    FtsRemoteDataID.put( 1, metaData);
    metaData.setId(1);
  }

  static FtsRemoteMetaData getRemoteMetaData()
  {
    return metaData;
  }

  protected FtsRemoteMetaData()
  {
    super();
  }

  /* Called by Fts.newRemoteData */

  final FtsRemoteData newInstance(String name, Object args[])
  {
    FtsRemoteData data;
    int id;

    id = FtsRemoteDataID.getNewDataID();
    remoteCall(REMOTE_NEW, id, name, args);
   
    Fts.sync();

    data = FtsRemoteDataID.get(id);
    data.setMaster();
    return data;
  }


  /* Remote Calls implementation   */

  public final void call( int key, FtsMessage msg)
  {
    switch( key)
      {
      case REMOTE_NEW:
	newFtsRemoteData(msg);
	break;
      case REMOTE_RELEASE:
	releaseRemoteData(msg); 
      default:
	break;
      }
  }


  /*
   * Remote calls implementation 
   */

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


  private void releaseRemoteData( FtsMessage msg)
  {
    FtsRemoteData data = (FtsRemoteData) msg.getArgument(2);

    data.release();
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





