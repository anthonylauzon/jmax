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

package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;
import java.lang.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;

public class JMaxFontPopUpAction extends EditorAction
{
  String font;
  public  void actionPerformed(ActionEvent e)
  {     
    font = PatcherFontManager.getInstance().getFontName(((JMenuItem)e.getSource()).getText());
    super.actionPerformed(e);
  }
  
  public void doAction(EditorContainer container)
  {
    GraphicObject object = ObjectPopUp.getPopUpTarget();
    try{
      object.redraw();
      object.redrawConnections();
      object.setFontName(font);
      object.redraw();
      object.redrawConnections();
    }
    catch (Exception e){
      ErrorDialog aErr = new ErrorDialog(container.getFrame(), "This font does not exist on this platform");
      aErr.setLocation( 100, 100);
      aErr.show();  
    }
  }
}
