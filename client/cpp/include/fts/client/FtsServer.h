//
// FTS client library
// Copyright (C) 2001 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
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
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

namespace ircam {
namespace fts {
namespace client {

  template <class KeyT, class ValT> class FTSCLIENT_API Hashtable;
  class BinaryProtocolEncoder;

  class FTSCLIENT_API FtsServer {
  public:
    static const int CLIENT_OBJECT_ID = 0;

    FtsServer( FtsServerConnection *connection);

    BinaryProtocolEncoder *getEncoder() { return _encoder; }

    int getNewObjectID();

    FtsObject *getObject( int id);
    void putObject( int id, FtsObject *obj);

  private:
    int _newObjectID;
    BinaryProtocolEncoder *_encoder;
    Hashtable< int, FtsObject *> *_objectTable;

    FtsServerConnection *_connection;
  };

};
};
};
