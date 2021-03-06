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
     public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsGraphicObject( server, parent, objId, className, args, offset, length);
    }  
    }; 
    JMaxObjectCreator patcherCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new Patcher( new FtsPatcherObject( server, parent, objId,  className, args, offset, length));
    }
     public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsPatcherObject( server, parent, objId, className, args, offset, length);
    } 
    }; 
    JMaxObjectCreator inletCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new Inlet( new FtsInletObject( server, parent, objId,  className, args, offset, length));
    }
      public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsInletObject( server, parent, objId, className, args, offset, length);
    } 
    }; 
    JMaxObjectCreator outletCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new Outlet( new FtsOutletObject( server, parent, objId,  className, args, offset, length));
    }
      public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsOutletObject( server, parent, objId, className, args, offset, length);
    } 
    }; 
    JMaxObjectCreator forkCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new Fork( new FtsForkObject( server, parent, objId,  className, args, offset, length));
    }
     public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsForkObject( server, parent, objId, className, args, offset, length);
    } 
    };
    JMaxObjectCreator jcommentCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new Comment( new FtsCommentObject( server, parent, objId,  className, args, offset, length));
    }
     public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsCommentObject( server, parent, objId, className, args, offset, length);
    } 
    };
    JMaxObjectCreator messconstCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new MessConst( new FtsMessConstObject( server, parent, objId,  className, args, offset, length));
    }
      public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsMessConstObject( server, parent, objId, className, args, offset, length);
    } 
    };
    JMaxObjectCreator buttonCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new Bang( new FtsBangObject( server, parent, objId,  className, args, offset, length));
    }
      public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length)  
    {
        return new FtsBangObject( server, parent, objId, className, args, offset, length);
    } 
    };
    JMaxObjectCreator toggleCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new Toggle( new FtsToggleObject( server, parent, objId,  className, args, offset, length));
    }
      public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsToggleObject( server, parent, objId, className, args, offset, length);
    }       
    };
    JMaxObjectCreator sliderCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new Slider( new FtsSliderObject( server, parent, objId,  className, args, offset, length));
    }
      public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsSliderObject( server, parent, objId, className, args, offset, length);
    }    
    };
    JMaxObjectCreator intboxCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new IntBox( new FtsIntValueObject( server, parent, objId,  className, args, offset, length));
    }
      public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsIntValueObject( server, parent, objId, className, args, offset, length);
    }    
    };
    JMaxObjectCreator floatboxCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FloatBox( new FtsFloatValueObject( server, parent, objId,  className, args, offset, length));
    }
      public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsFloatValueObject( server, parent, objId, className, args, offset, length);
    }    
    };
    JMaxObjectCreator displayCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new Display( new FtsDisplayObject( server, parent, objId,  className, args, offset, length));
    }
     public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsDisplayObject( server, parent, objId, className, args, offset, length);
    }    
    };
    JMaxObjectCreator vecdisplayCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new VectorDisplay( new FtsVectorDisplayObject( server, parent, objId,  className, args, offset, length));
    }
      public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsVectorDisplayObject( server, parent, objId, className, args, offset, length);
    }    
    };
    JMaxObjectCreator matdisplayCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new MatDisplay( new FtsMatDisplayObject( server, parent, objId,  className, args, offset, length));
    }
      public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsMatDisplayObject( server, parent, objId, className, args, offset, length);
    }    
    };
    JMaxObjectCreator scopeCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new Scope( new FtsScopeObject( server, parent, objId, className, args, offset, length));
    }
      public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsScopeObject( server, parent, objId, className, args, offset, length);
    }    
    };
    JMaxObjectCreator monitorCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new Monitor( new FtsMonitorObject( server, parent, objId, className, args, offset, length));
    }
      public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsMonitorObject( server, parent, objId, className, args, offset, length);
    }    
    };
    JMaxObjectCreator xyPadCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new XYPad( new FtsXYPadObject( server, parent, objId, className, args, offset, length));
    }
      public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsXYPadObject( server, parent, objId, className, args, offset, length);
    }    
    };
    JMaxObjectCreator defineCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new Define( new FtsDefineObject( server, parent, objId, className, args, offset, length));
    }
      public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsDefineObject( server, parent, objId, className, args, offset, length);
    }    
    };
    JMaxObjectCreator inputCreator = new JMaxObjectCreator() {
      public GraphicObject create( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new Input( new FtsInputObject( server, parent, objId, className, args, offset, length));
    }
      public FtsGraphicObject createFtsObject( FtsServer server, FtsObject parent, int objId, String className, FtsAtom[] args, int offset, int length) 
    {
        return new FtsInputObject( server, parent, objId, className, args, offset, length);
    }    
    };
    
    JMaxClassMap.put( "standard", standardCreator, "/icons/standard.gif", "/icons/standard_selected.gif", "/icons/standard_cursor.gif", "standard", this);
    JMaxClassMap.put( "jpatcher", patcherCreator, "/icons/jpatcher.gif", "/icons/jpatcher_selected.gif", "/icons/jpatcher_cursor.gif", "patcher", this);
    JMaxClassMap.put( "receive", inletCreator, "/icons/inlet.gif",  "/icons/inlet_selected.gif", "/icons/inlet_cursor.gif", "inlet label", this);
    JMaxClassMap.put( "send", outletCreator, "/icons/outlet.gif", "/icons/outlet_selected.gif","/icons/outlet_cursor.gif", "outlet label", this);
    JMaxClassMap.put( "define", defineCreator, "/icons/define.gif", "/icons/define_selected.gif", "/icons/define_cursor.gif", "define", this);
    JMaxClassMap.put( "fork", forkCreator, "/icons/fork.gif", "/icons/fork_selected.gif", "/icons/fork_cursor.gif", "fork", this);
    JMaxClassMap.put( "jcomment", jcommentCreator, "/icons/jcomment.gif", "/icons/jcomment_selected.gif", "/icons/jcomment_cursor.gif", "comment", this);
    JMaxClassMap.put( "messconst", messconstCreator, "/icons/messconst.gif", "/icons/messconst_selected.gif", "/icons/messconst_cursor.gif", "message",this);
    JMaxClassMap.put( "button", buttonCreator, "/icons/button.gif", "/icons/button_selected.gif", "/icons/button_cursor.gif", "bang", this);
    JMaxClassMap.put( "toggle", toggleCreator, "/icons/toggle.gif", "/icons/toggle_selected.gif", "/icons/toggle_cursor.gif", "toggle", this);
    JMaxClassMap.put( "slider", sliderCreator, "/icons/slider.gif", "/icons/slider_selected.gif", "/icons/slider_cursor.gif", "slider", this);
    JMaxClassMap.put( "xypad", xyPadCreator, "/icons/xypad.gif", "/icons/xypad_selected.gif", "/icons/xypad_cursor.gif", "2D controller", this);
    JMaxClassMap.put( "intbox", intboxCreator, "/icons/intbox.gif", "/icons/intbox_selected.gif", "/icons/intbox_cursor.gif", "integer", this);
    JMaxClassMap.put( "floatbox", floatboxCreator, "/icons/floatbox.gif", "/icons/floatbox_selected.gif", "/icons/floatbox_cursor.gif", "float", this);
    JMaxClassMap.put( "display", displayCreator, "/icons/display.gif",  "/icons/display_selected.gif", "/icons/display_cursor.gif", "generic display", this);
    JMaxClassMap.put( "vecdisplay", vecdisplayCreator, "/icons/vecdisplay.gif", "/icons/vecdisplay_selected.gif", "/icons/vecdisplay_cursor.gif", "vector display", this);
    JMaxClassMap.put( "matdisplay", matdisplayCreator, "/icons/matdisplay.gif", "/icons/matdisplay_selected.gif","/icons/matdisplay_cursor.gif", "matrix display", this);
    JMaxClassMap.put( "scope~", scopeCreator, "/icons/scope.gif", "/icons/scope_selected.gif", "/icons/scope_cursor.gif", "oscilloscope", this);
    JMaxClassMap.put( "monitor~", monitorCreator, "/icons/monitor.gif", "/icons/monitor_selected.gif", "/icons/monitor_cursor.gif", "monitor", this);
    JMaxClassMap.put( "input~", inputCreator, "/icons/input.gif", "/icons/input_selected.gif", "/icons/input_cursor.gif", "input", this);
}
}



