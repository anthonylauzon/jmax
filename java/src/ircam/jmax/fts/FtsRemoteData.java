 //
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
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
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.fts;

import java.lang.*;
import java.util.*;

import ircam.jmax.mda.*;
import ircam.jmax.utils.*;

/**
 * Remote data generic class.
 */

public abstract class FtsRemoteData implements MaxData
{
  Fts fts;

  void setFts(Fts fts)
  {
    this.fts = fts;
  }

  public Fts getFts()
  {
    return fts;
  }

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

  abstract protected void call( int key, FtsStream stream)
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException;

  /* Subclasses should implement the release method (calling super !!) */

  public void delete()
  {
    if (master)
      {
	FtsRemoteMetaData.getRemoteMetaData().remoteCallStart(FtsRemoteMetaData.REMOTE_DELETE);
	FtsRemoteMetaData.getRemoteMetaData().remoteCallAddArg(this);
	FtsRemoteMetaData.getRemoteMetaData().remoteCallEnd();
      }

    release();
  }

  protected void release()
  {
    Mda.dispose(this);
    fts.getServer().getRemoteTable().release(id);
  }


  /* NEW: family of var args remote calls;
     unconsistent use of this functions will broke everything;
     on the other side, these functions allow you to send
     messages without allocating new objects.
   */

  public void remoteCallStart( int key)
  {
    fts.getServer().remoteCallStart( this, key);
  }

  public void remoteCallAddArg( int arg)
  {
    fts.getServer().remoteCallAddArg( arg);
  }

  public void remoteCallAddArg( Integer arg)
  {
    fts.getServer().remoteCallAddArg( arg);
  }

  public void remoteCallAddArg( float arg)
  {
    fts.getServer().remoteCallAddArg( arg);
  }

  public void remoteCallAddArg( Float arg)
  {
    fts.getServer().remoteCallAddArg( arg);
  }

  public void remoteCallAddArg( FtsObject arg)
  {
    fts.getServer().remoteCallAddArg( arg);
  }

  public void remoteCallAddArg( String arg)
  {
    fts.getServer().remoteCallAddArg( arg);
  }

  public void remoteCallAddArg( FtsRemoteData arg)
  {
    fts.getServer().remoteCallAddArg( arg);
  }

  public void remoteCallEnd()
  {
    fts.getServer().remoteCallEnd();
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
   
  // No argument remoteCall

  public void remoteCall( int key)
  {
    fts.getServer().remoteCall( this, key, (MaxVector) null);
  }
  
  // Single argument remote call

  public void remoteCall( int key, int arg)
  {
    fts.getServer().remoteCall( this, key, arg);
  }

  public void remoteCall( int key, float arg)
  {
    fts.getServer().remoteCall( this, key, arg);
  }

  public void remoteCall( int key, Object arg)
  {
    fts.getServer().remoteCall( this, key, arg);
  }


  // Multiple arguments remote call

  public void remoteCall( int key, Object args[])
  {
    fts.getServer().remoteCall( this, key, args);
  }

  public void remoteCall( int key, MaxVector args)
  {
    fts.getServer().remoteCall( this, key, args);
  }

  // Special  remote calls

  public void remoteCall( int key, int offset, int size, int values[])
  {
    fts.getServer().remoteCall( this, key, offset, size, values);
  }

  public void remoteCall( int  key, int id, String name, Object args[])
  {
    fts.getServer().remoteCall(this, key, id, name, args);
  }

  public void remoteCall( int  key, FtsObject obj, Object args[])
  {
    fts.getServer().remoteCall(this, key, obj, args);
  }

  public void remoteCall( int  key, FtsObject obj, MaxVector args)
  {
    fts.getServer().remoteCall(this, key, obj, args);
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


