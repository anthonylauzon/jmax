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
import ircam.jmax.editors.bpf.*;
import ircam.jmax.editors.table.*;
import ircam.jmax.editors.mat.*;
import ircam.fts.client.*;

// debug
import java.awt.*;
import java.io.*;
import java.util.*;
import javax.swing.*;

public class Data implements JMaxPackage 
{
  
  public void load()
{    
    JMaxObjectCreator bpfCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
      {
        return new Standard( new FtsBpfObject( server, parent, objId, className, args, offset, length));
      }
      public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
      {
        return new FtsBpfObject( server, parent, objId, className, args, offset, length);
      }
    }; 
    JMaxObjectCreator ivecCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new Standard( new FtsIvecObject( server, parent, objId, className, args, offset, length));
    }
     public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsIvecObject( server, parent, objId, className, args, offset, length);
    }      
    }; 
    JMaxObjectCreator fvecCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new Standard( new FtsFvecObject( server, parent, objId, className, args, offset, length));
    }
      public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length)  
    {
        return new FtsFvecObject( server, parent, objId, className, args, offset, length);
    }  
    }; 
    JMaxObjectCreator matCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new Standard( new FtsMatObject( server, parent, objId, className, args, offset, length));
    }
      public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsMatObject( server, parent, objId, className, args, offset, length);
    }  
    };  
    JMaxObjectCreator dictCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new Standard( new FtsDictObject( server, parent, objId, className, args, offset, length));
    }
      public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsDictObject( server, parent, objId, className, args, offset, length);
    }  
    }; 
    JMaxObjectCreator fmatCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new Standard( new FtsFmatObject( server, parent, objId, className, args, offset, length));
    }
      public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsFmatObject( server, parent, objId, className, args, offset, length);
    }        
    };  
    
    JMaxClassMap.put( "bpf", bpfCreator, null, null, "bpf", this);
    JMaxClassMap.put( "ivec", ivecCreator, null, null, "ivec", this);
    JMaxClassMap.put( "fvec", fvecCreator, null, null, "fvec", this);
    JMaxClassMap.put( "mat", matCreator, null, null, "mat", this);
    JMaxClassMap.put( "dict", dictCreator, null, null, "dict", this);
    JMaxClassMap.put( "fmat", fmatCreator, null, null, "fmat", this);
}
}
