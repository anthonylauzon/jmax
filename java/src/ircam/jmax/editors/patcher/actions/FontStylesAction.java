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

package ircam.jmax.editors.patcher.actions;

import java.awt.*;
import java.awt.event.*;
import java.lang.*;

// import javax.swing.*;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;

import ircam.jmax.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.objects.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;

public class FontStylesAction extends EditorAction
{
  public FontStylesAction()
  {
    super("font styles");
  }

  String style;  
  boolean selected;
  public  void actionPerformed(ActionEvent e)
  {
      style = ((JMenuItem)e.getSource()).getText();
      selected = ((JMenuItem)e.getSource()).isSelected();
      super.actionPerformed(e);
  }
  
  public void doAction(EditorContainer container)
  {
    ErmesSketchPad sketch = (ErmesSketchPad) container.getEditor();

    if (ErmesSelection.patcherSelection.ownedBy(sketch)&& 
	ErmesSelection.patcherSelection.hasObjects()){
      try{

	sketch.setUndo("Font-Change", false, false);	

	ErmesSelection.patcherSelection.apply(new ObjectAction() {
	  public void processObject(GraphicObject object)
	    {
	      object.redraw();
	      object.redrawConnections();
	      object.changeFontStyle(style, selected);
	      object.redraw();
	      object.redrawConnections();
	    }});
      }
      catch (Exception e){
	  JOptionPane.showMessageDialog(container.getFrame(), "This font does not exist on this platform", 
					"Warning", JOptionPane.WARNING_MESSAGE); 
      }
    }
    else
	sketch.changeDefaultFontStyle(style, selected);
  }
}




