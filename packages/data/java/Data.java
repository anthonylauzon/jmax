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

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.editors.bpf.*;
import ircam.jmax.guiobj.*;
import ircam.jmax.editors.table.*;
import ircam.fts.client.*;

// debug
import java.awt.*;
import java.io.*;
import java.util.*;
import javax.swing.*;

public class Data implements JMaxPackage {

  public void load()
  {    
    JMaxObjectCreator bpfCreator = new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
	{
	  return new Standard( new FtsBpfObject( server, parent, objId, className, args, offset, length));
	}
      }; 
    JMaxObjectCreator tableCreator = new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
	{
	  return new Standard( new FtsTableObject( server, parent, objId, className, args, offset, length));
	}
      }; 

    JMaxClassMap.put( "bpf", bpfCreator, null, null, this);
    JMaxClassMap.put( "ivec", tableCreator, null, null, this);

    System.out.println( "package Data loaded (Java Classes)");
  }
}
