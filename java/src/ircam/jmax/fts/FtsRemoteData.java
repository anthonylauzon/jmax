package ircam.jmax.fts;

import java.lang.*;
import ircam.jmax.mda.*;

public abstract class FtsRemoteData implements MaxData {

  public int getId()
    {
      return id;
    }

  public void setId( int id)
    {
      this.id = id;
    }

  public String toString()
    {
      return this.getClass().getName() + "." + id;
    }

  abstract public void call( int key, FtsMessage msg);


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

  protected int id;

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

