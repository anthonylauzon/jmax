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
import java.io.*;
import ircam.jmax.mda.*;

class FtsRemoteMetaData extends FtsRemoteData
{
  // Remote call codes

  static final int REMOTE_NEW     = 1;
  static final int REMOTE_DELETE  = 2;
  static final int REMOTE_RELEASE = 3;
 
  static private FtsRemoteMetaData metaData;

  public static void install(Fts fts)
  {
    metaData = new FtsRemoteMetaData();
    metaData.setFts(fts);
    fts.getServer().getRemoteTable().put( 1, metaData);
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

    id = fts.getServer().getRemoteTable().getNewDataID();

    remoteCall(REMOTE_NEW, id, name, args);
   
    fts.sync();

    data = fts.getServer().getRemoteTable().get(id);
    data.setFts(fts);
    data.setMaster();
    return data;
  }


  /* Remote Calls implementation   */

  public final void call( int key, FtsStream stream)
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    switch( key)
      {
      case REMOTE_NEW:
	newFtsRemoteData(stream);
	break;
      case REMOTE_RELEASE:
	releaseRemoteData(stream); 
      default:
	break;
      }
  }


  /*
   * Remote calls implementation 
   */

  private void newFtsRemoteData( FtsStream stream)
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    int newId        = stream.getNextIntArgument();
    String className = stream.getNextStringArgument();
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
    catch (java.lang.Exception e)
      {
	System.err.println( e + " in " + dataJavaClass.getName());
	return;
      }

    newRemoteData.setFts( fts);
    newRemoteData.setId( newId);
    fts.getServer().getRemoteTable().put( newId, newRemoteData);
  }


  private void releaseRemoteData( FtsStream stream)
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    FtsRemoteData data = (FtsRemoteData) stream.getNextDataArgument();

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
}





