/*
 * FTS client library
 * Copyright (C) 2001 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 * 
 */

/**
 * A FtsObject is the client representation of an object residing in
 * the server.
 * It allows creation and message sending.
 * A client can only send messages to the objects that it has created.
 */

class FTSCLIENT_API FtsCallback {
public:
  virtual void invoke( FtsArgs* args) = 0;
  virtual void invoke( const char *selector, FtsArgs* args) = 0;
};

class FTSCLIENT_API FtsObject {
  friend class FtsServer;
public:
  FtsObject( FtsServer *server, FtsObject *parent, const char *ftsClassName) throw( FtsClientException);
  FtsObject( FtsServer *server, FtsObject *parent, const char *ftsClassName, FtsArgs &args) throw( FtsClientException);
  ~FtsObject() throw( FtsClientException);

  void destroy() throw( FtsClientException);

  /**
   * returns object's id
   */
  int getID() const { return _id; }

  /**
   * connect to another object
   */
  void connectTo( int outlet, FtsObject *dst, int dstInlet);

  /**
   * connect from another object
   */
  void connectFrom( int inlet, FtsObject *src, int srcOutlet);

  /**
   * send a message<BR>
   * Most general forms of message sending.<BR>
   * Atomic forms are preferred.
   */
  void send( const char *selector, FtsArgs &args) throw( FtsClientException);
  void send( const char *selector) throw( FtsClientException);

  /**
   * send a "list" message
   */
  void send( FtsArgs &args) throw( FtsClientException);

  /**
   * send atomic messages
   */
  void send( int n) throw( FtsClientException);
  void send( float f) throw( FtsClientException);

  /**
   * installs a callback on this object
   */
  void install( const char *s, FtsCallback *callback);
  void install( FtsCallback *callback);

private:
  // This constructor is used to create the pre-defined objects
  FtsObject( FtsServer *server, int id);

  static int _newObjectId;

  FtsServer *_server;
  int _id;
  FtsHashTable< const char *, FtsCallback *> *_callbacks;
};
