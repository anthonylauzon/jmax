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
import ircam.jmax.guiobj.*;
import ircam.fts.client.*;

// debug
import java.awt.*;
import java.io.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.*;

public class Guiobj implements JMaxPackage {

  public void load()
  {    
    JMaxObjectCreator standardCreator = new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
	{
	  return new Standard( new FtsGraphicObject( server, parent, objId, className, args, offset, length));
	}
      }; 
    JMaxObjectCreator patcherCreator = new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
	{
	  return new Patcher( new FtsPatcherObject( server, parent, objId,  className, args, offset, length));
	}
      }; 
    JMaxObjectCreator inletCreator = new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
	{
	  return new Inlet( new FtsInletObject( server, parent, objId,  className, args, offset, length));
	}
      }; 
    JMaxObjectCreator outletCreator = new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
	{
	  return new Outlet( new FtsOutletObject( server, parent, objId,  className, args, offset, length));
	}
      }; 
    JMaxObjectCreator forkCreator = new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
	{
	  return new Fork( new FtsForkObject( server, parent, objId,  className, args, offset, length));
	}
      };
    JMaxObjectCreator jcommentCreator = new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
	{
	  return new Comment( new FtsCommentObject( server, parent, objId,  className, args, offset, length));
	}
      };
    JMaxObjectCreator messconstCreator = new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
	{
	  return new MessConst( new FtsMessConstObject( server, parent, objId,  className, args, offset, length));
	}
      };
    JMaxObjectCreator buttonCreator = new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
	{
	  return new Bang( new FtsBangObject( server, parent, objId,  className, args, offset, length));
	}
      };
    JMaxObjectCreator toggleCreator = new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
	{
	  return new Toggle( new FtsToggleObject( server, parent, objId,  className, args, offset, length));
	}
      };
    JMaxObjectCreator sliderCreator = new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
	{
	  return new Slider( new FtsSliderObject( server, parent, objId,  className, args, offset, length));
	}
      };
    JMaxObjectCreator intboxCreator = new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
	{
	  return new IntBox( new FtsIntValueObject( server, parent, objId,  className, args, offset, length));
	}
      };
    JMaxObjectCreator floatboxCreator = new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
	{
	  return new FloatBox( new FtsFloatValueObject( server, parent, objId,  className, args, offset, length));
	}
      };
    JMaxObjectCreator displayCreator = new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
	{
	  return new Display( new FtsDisplayObject( server, parent, objId,  className, args, offset, length));
	}
      };
    JMaxObjectCreator vecdisplayCreator = new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
	{
	  return new VectorDisplay( new FtsVectorDisplayObject( server, parent, objId,  className, args, offset, length));
	}
      };
    JMaxObjectCreator matdisplayCreator = new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
	{
	  return new MatDisplay( new FtsMatDisplayObject( server, parent, objId,  className, args, offset, length));
	}
      };
    JMaxObjectCreator scopeCreator = new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
	{
	  return new Scope( new FtsScopeObject( server, parent, objId, className, args, offset, length));
	}
      };
    JMaxObjectCreator monitorCreator = new JMaxObjectCreator() {
	public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
	{
	  return new Monitor( new FtsMonitorObject( server, parent, objId, className, args, offset, length));
	}
      };

    JMaxClassMap.put( "standard", standardCreator, "/icons/standard.gif", "/icons/standard_cursor.gif", this);
    JMaxClassMap.put( "jpatcher", patcherCreator, "/icons/jpatcher.gif", "/icons/jpatcher_cursor.gif", this);
    JMaxClassMap.put( "inlet", inletCreator, "/icons/inlet.gif", "/icons/inlet_cursor.gif", this);
    JMaxClassMap.put( "outlet", outletCreator, "/icons/outlet.gif", "/icons/outlet_cursor.gif", this);
    JMaxClassMap.put( "fork", forkCreator, "/icons/fork.gif", "/icons/fork_cursor.gif", this);
    JMaxClassMap.put( "jcomment", jcommentCreator, "/icons/jcomment.gif", "/icons/jcomment_cursor.gif", this);
    JMaxClassMap.put( "messconst", messconstCreator, "/icons/messconst.gif", "/icons/messconst_cursor.gif", this);
    JMaxClassMap.put( "button", buttonCreator, "/icons/button.gif", "/icons/button_cursor.gif", this);
    JMaxClassMap.put( "toggle", toggleCreator, "/icons/toggle.gif", "/icons/toggle_cursor.gif", this);
    JMaxClassMap.put( "slider", sliderCreator, "/icons/slider.gif", "/icons/slider_cursor.gif", this);
    JMaxClassMap.put( "intbox", intboxCreator, "/icons/intbox.gif", "/icons/intbox_cursor.gif", this);
    JMaxClassMap.put( "floatbox", floatboxCreator, "/icons/floatbox.gif", "/icons/floatbox_cursor.gif", this);
    JMaxClassMap.put( "display", displayCreator, "/icons/display.gif", "/icons/display_cursor.gif", this);
    JMaxClassMap.put( "vecdisplay", vecdisplayCreator, "/icons/vecdisplay.gif", "/icons/vecdisplay_cursor.gif", this);
    JMaxClassMap.put( "matdisplay", matdisplayCreator, "/icons/matdisplay.gif", "/icons/matdisplay_cursor.gif", this);
    JMaxClassMap.put( "scope~", scopeCreator, "/icons/scope.gif", "/icons/scope_cursor.gif", this);
    JMaxClassMap.put( "monitor~", monitorCreator, "/icons/monitor.gif", "/icons/monitor_cursor.gif", this);
  }
}
