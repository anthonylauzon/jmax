package ircam.jmax.fts;

import java.lang.*;
import java.util.*;

import ircam.jmax.mda.*;
import ircam.jmax.utils.*;

public abstract class FtsRemoteData implements MaxData {
  
  void setMaster()
  {
    master = true;
  }

  int getId()
  {
    return id;
  }

  void setId( int id)
    {
      this.id = id;
    }

  public String toString()
    {
      return this.getClass().getName() + "." + id;
    }

  abstract public void call( int key, FtsMessage msg);

  /* Subclasses should implement the release method (calling super !!) */

  public void delete()
  {
    if (master)
      {
	Object args[] = new Object[1];

	args[0] = this;
	FtsRemoteMetaData.getRemoteMetaData().remoteCall(FtsRemoteMetaData.REMOTE_DELETE, args);
      }

    release();
  }

  protected void release()
  {
    Mda.dispose(this);
    FtsRemoteDataID.release(id);
    
  }

  /* We implement a family of remoteCall methods.
     The first one accept the argument as an object array;
     others accept various form of arguments, to avoid
     allocating new objects when sending the message;
     some of the special version are quite general,
     and some are quite specific, of course, but the problem
     cannot be solved in general; we support first messages
     that can have vector related arguments.
     */
     
  public void remoteCall( int key, Object args[])
  {
    Fts.getServer().remoteCall( this, key, args);
  }

  public void remoteCall( int key, int offset, int size, int values[])
  {
    Fts.getServer().remoteCall( this, key, offset, size, values);
  }

  public void remoteCall( int  key, int id, String name, Object args[])
  {
    Fts.getServer().remoteCall(this, key, id, name, args);
  }

  public void remoteCall( int  key, FtsObject obj, Object args[])
  {
    Fts.getServer().remoteCall(this, key, obj, args);
  }

  public void remoteCall( int  key, FtsObject obj, MaxVector args)
  {
    Fts.getServer().remoteCall(this, key, obj, args);
  }

  protected int id;
  private boolean master = false;	// true if the object has been created by the client
  
  //  The MaxData interface

  /* Temporary handle the MaxDocument; a MaxData ust be declared
     part of a document, so that the MaxData editor can be closed
     when the document is closed; example, you want the explode editor
     to be closed if the patch is closed and discarded.
     The problem is that MaxDocument are not yet known on the FTS 
     side, so the FtsRemoteData cannot really now the document by itself.

     For the moment, is handled with an hack, it will be really solved
     with the generalization of the use of FtsRemoteData in FTS.

     The getName method is left to the subclasses.
     */


  private MaxDocument document;

  public MaxDocument getDocument()
  {
    return this.document;
  }

  /** Hack method */

  public void setDocument(MaxDocument document)
  {
    this.document = document;
  }
};

