//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.ispw.*;
import ircam.jmax.editors.qlist.*;
import ircam.jmax.editors.explode.*;
import ircam.fts.client.*;

public class Ispw implements JMaxPackage {
  
  public void load()
{    
    JMaxObjectCreator messageCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new Message( new FtsMessageObject( server, parent, objId, className, args, offset, length));
    }
      public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsMessageObject( server, parent, objId, className, args, offset, length);
    }      
    }; 
    
    JMaxObjectCreator qlistCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new Standard( new FtsQListObject( server, parent, objId, className, args, offset, length));
    }
      public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsQListObject( server, parent, objId, className, args, offset, length);
    }      
    }; 
    
    JMaxObjectCreator explodeCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new Standard( new FtsExplodeObject( server, parent, objId, className, args, offset, length));
    }
      public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsExplodeObject( server, parent, objId, className, args, offset, length);
    }      
    }; 
    
    JMaxClassMap.put( "messbox", messageCreator, "/icons/messbox.gif", "/icons/messbox_selected.gif", "/icons/messbox_cursor.gif", "old message box", this);
    JMaxClassMap.put( "qlist", qlistCreator, null, null, "qlist", this);
    JMaxClassMap.put( "explode", explodeCreator, null, null, "explode", this);    
}
}
