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
// Authors: Riccardo Borghesi, Francois Dechelle, Norbert Schnell.
// 

package ircam.jmax.guiobj;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.patcher.objects.GraphicObject;
import ircam.fts.client.*;

/**
 * The table extension; install the table data type
 * and the table file data handler
 */
public class Guiobj extends JMaxPackage
{
  public Guiobj()
  {
    super("guiobj");
  }

  public void load()
  {    
    ObjectCreatorManager.register( "fork", new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, FtsAtom[] args, int offset, int length) 
	{
	  return new Fork( new FtsForkObject( server, parent, objId, args, offset, length));
	}
      });
    ObjectCreatorManager.register( "jcomment", new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, FtsAtom[] args, int offset, int length) 
	{
	  return new Comment( new FtsCommentObject( server, parent, objId, args, offset, length));
	}
      });
    ObjectCreatorManager.register( "messconst", new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, FtsAtom[] args, int offset, int length) 
	{
	  return new MessConst( new FtsMessConstObject( server, parent, objId, args, offset, length));
	}
      });
    ObjectCreatorManager.register( "button", new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, FtsAtom[] args, int offset, int length) 
	{
	  return new Bang( new FtsBangObject( server, parent, objId, args, offset, length));
	}
      });
    ObjectCreatorManager.register( "toggle", new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, FtsAtom[] args, int offset, int length) 
	{
	  return new Toggle( new FtsToggleObject( server, parent, objId, args, offset, length));
	}
      });
     ObjectCreatorManager.register( "slider", new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, FtsAtom[] args, int offset, int length) 
	{
	  return new Slider( new FtsSliderObject( server, parent, objId, args, offset, length));
	}
      });
     ObjectCreatorManager.register( "intbox", new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, FtsAtom[] args, int offset, int length) 
	{
	  return new IntBox( new FtsIntValueObject( server, parent, objId, args, offset, length));
	}
       });
     ObjectCreatorManager.register( "floatbox", new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, FtsAtom[] args, int offset, int length) 
	{
	  return new FloatBox( new FtsFloatValueObject( server, parent, objId, args, offset, length));
	}
       });
     ObjectCreatorManager.register( "display", new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, FtsAtom[] args, int offset, int length) 
	{
	  return new Display( new FtsDisplayObject( server, parent, objId, args, offset, length));
	}
       });
     ObjectCreatorManager.register( "vecdisplay", new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, FtsAtom[] args, int offset, int length) 
	{
	  return new VectorDisplay( new FtsVectorDisplayObject( server, parent, objId, args, offset, length));
	}
       });
     ObjectCreatorManager.register( "scope", new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, FtsAtom[] args, int offset, int length) 
	{
	  return new Scope( new FtsScopeObject( server, parent, objId, args, offset, length));
	}
       });
     
     ircam.jmax.editors.console.ConsoleWindow.append("package Guiobj loaded (Java Classes)");	      
  }
}





