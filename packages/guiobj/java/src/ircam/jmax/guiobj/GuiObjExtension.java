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

import ircam.jmax.script.pkg.*;
import ircam.jmax.script.*;
import ircam.jmax.editors.patcher.objects.*;
import ircam.jmax.*;
import ircam.jmax.mda.*;

import java.io.*;

/**
 * The table extension; install the table data type
 * and the table file data handler
 */
public class GuiObjExtension extends tcl.lang.Extension implements JavaExtension
{
  public void init(Interpreter interp)
  {
      ObjectCreatorManager.registerFtsClass("fork", ircam.jmax.guiobj.FtsForkObject.class);
      ObjectCreatorManager.registerGraphicClass("fork", ircam.jmax.guiobj.Fork.class, "guiobj");

      ObjectCreatorManager.registerFtsClass("jcomment", ircam.jmax.guiobj.FtsCommentObject.class);
      ObjectCreatorManager.registerGraphicClass("jcomment", ircam.jmax.guiobj.Comment.class, "guiobj");

      ObjectCreatorManager.registerFtsClass("messconst", ircam.jmax.guiobj.FtsMessConstObject.class);
      ObjectCreatorManager.registerGraphicClass("messconst", ircam.jmax.guiobj.MessConst.class, "guiobj");

      ObjectCreatorManager.registerFtsClass("button", ircam.jmax.guiobj.FtsBangObject.class);
      ObjectCreatorManager.registerGraphicClass("button", ircam.jmax.guiobj.Bang.class, "guiobj");

      ObjectCreatorManager.registerFtsClass("toggle", ircam.jmax.fts.FtsIntValueObject.class);
      ObjectCreatorManager.registerGraphicClass("toggle", ircam.jmax.guiobj.Toggle.class, "guiobj");

      ObjectCreatorManager.registerFtsClass("slider", ircam.jmax.guiobj.FtsSliderObject.class);
      ObjectCreatorManager.registerGraphicClass("slider", ircam.jmax.guiobj.Slider.class, "guiobj");

      ObjectCreatorManager.registerFtsClass("intbox", ircam.jmax.fts.FtsIntValueObject.class);
      ObjectCreatorManager.registerGraphicClass("intbox", ircam.jmax.guiobj.IntBox.class, "guiobj");

      ObjectCreatorManager.registerFtsClass("floatbox", ircam.jmax.guiobj.FtsFloatValueObject.class);
      ObjectCreatorManager.registerGraphicClass("floatbox", ircam.jmax.guiobj.FloatBox.class, "guiobj");

      ObjectCreatorManager.registerFtsClass("display", ircam.jmax.guiobj.FtsDisplayObject.class);
      ObjectCreatorManager.registerGraphicClass("display", ircam.jmax.guiobj.Display.class, "guiobj");
      
      ObjectCreatorManager.registerFtsClass("vecdisplay", ircam.jmax.guiobj.FtsVectorDisplayObject.class);
      ObjectCreatorManager.registerGraphicClass("vecdisplay", ircam.jmax.guiobj.VectorDisplay.class, "guiobj");

      ObjectCreatorManager.registerFtsClass("scope", ircam.jmax.guiobj.FtsScopeObject.class);
      ObjectCreatorManager.registerGraphicClass("scope", ircam.jmax.guiobj.Scope.class, "guiobj");
  }

    /* this method should be removed as soon as jacl is completely forgotten about */
  public void init(tcl.lang.Interp interp)
  {
      ObjectCreatorManager.registerFtsClass("fork", ircam.jmax.guiobj.FtsForkObject.class);
      ObjectCreatorManager.registerGraphicClass("fork", ircam.jmax.guiobj.Fork.class, "guiobj");
	
      ObjectCreatorManager.registerFtsClass("jcomment", ircam.jmax.guiobj.FtsCommentObject.class);
      ObjectCreatorManager.registerGraphicClass("jcomment", ircam.jmax.guiobj.Comment.class, "guiobj");

      ObjectCreatorManager.registerFtsClass("messconst", ircam.jmax.guiobj.FtsMessConstObject.class);
      ObjectCreatorManager.registerGraphicClass("messconst", ircam.jmax.guiobj.MessConst.class, "guiobj");

      ObjectCreatorManager.registerFtsClass("button", ircam.jmax.guiobj.FtsBangObject.class);
      ObjectCreatorManager.registerGraphicClass("button", ircam.jmax.guiobj.Bang.class, "guiobj");

      ObjectCreatorManager.registerFtsClass("toggle", ircam.jmax.fts.FtsIntValueObject.class);
      ObjectCreatorManager.registerGraphicClass("toggle", ircam.jmax.guiobj.Toggle.class, "guiobj");

      ObjectCreatorManager.registerFtsClass("slider", ircam.jmax.guiobj.FtsSliderObject.class);
      ObjectCreatorManager.registerGraphicClass("slider", ircam.jmax.guiobj.Slider.class, "guiobj");

      ObjectCreatorManager.registerFtsClass("intbox", ircam.jmax.fts.FtsIntValueObject.class);
      ObjectCreatorManager.registerGraphicClass("intbox", ircam.jmax.guiobj.IntBox.class, "guiobj");

      ObjectCreatorManager.registerFtsClass("floatbox", ircam.jmax.guiobj.FtsFloatValueObject.class);
      ObjectCreatorManager.registerGraphicClass("floatbox", ircam.jmax.guiobj.FloatBox.class, "guiobj");

      ObjectCreatorManager.registerFtsClass("display", ircam.jmax.guiobj.FtsDisplayObject.class);
      ObjectCreatorManager.registerGraphicClass("display", ircam.jmax.guiobj.Display.class, "guiobj");
      
      ObjectCreatorManager.registerFtsClass("vecdisplay", ircam.jmax.guiobj.FtsVectorDisplayObject.class);
      ObjectCreatorManager.registerGraphicClass("vecdisplay", ircam.jmax.guiobj.VectorDisplay.class, "guiobj");

      ObjectCreatorManager.registerFtsClass("scope", ircam.jmax.guiobj.FtsScopeObject.class);
      ObjectCreatorManager.registerGraphicClass("scope", ircam.jmax.guiobj.Scope.class, "guiobj");
  }
}





