package ircam.jmax.fts;

import java.util.*;
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

public class FtsRemoteDataObject extends FtsObject implements FtsObjectWithData, MaxData {

  static
  {
    FtsRemoteMetaData.install();
  }

  protected FtsRemoteData remoteData;

  class RemoteDataMessageHandler implements FtsMessageHandler {
    public void handleMessage( FtsMessage msg)
    {
      remoteData = (FtsRemoteData) msg.getArgument( 2);
    }
  }
  
  FtsRemoteDataObject(FtsContainerObject parent, String className, String description, int objId)
  {
    super(parent, className, description, objId);

    remoteData = null;

    installMessageHandler( new RemoteDataMessageHandler());
  }

  public FtsRemoteData getRemoteData()
  {
    return remoteData;
  }

  // FtsObjectWithData implementation
  public MaxData getData()
  {
    Fts.getServer().sendObjectMessage( this, -1, "getdata", (Vector)null);
    Fts.getServer().syncToFts();

    return this;
  }

  // FtsObjectWithData implementation
  public void setData(MaxData data) throws FtsException
  {
    System.err.println( this.getClass().getName() + " !!! Ouh la la !!! setData !!! li pa implimenti !!!");
  }

  // MaxData implementation
  public MaxDocument getDocument()
  {
    return getParent().getDocument();
  }

  // MaxData implementation
  public String getName()
  {
    return getObjectName();
  }

  // Tcl...
  public void saveAsTcl( PrintWriter writer)
  {
    System.err.println( this.getClass().getName() + " !!! Ouh la la la !!! TiCiIl !! li moi pas connitre !!!");
  }
}
