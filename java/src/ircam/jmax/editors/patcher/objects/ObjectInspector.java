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

package ircam.jmax.editors.patcher.objects;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.io.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.*;
import ircam.jmax.toolkit.*;

/**
 * The "system statistics" dialog.
 */

public class ObjectInspector extends JDialog 
{
  Frame parent;
  private GraphicObject obj;
  private ObjectControlPanel controlPanel;

  public ObjectInspector( GraphicObject obj) 
  {
    super( obj.getSketchPad().getEditorContainer().getFrame(), "Inspector", false);

    String title = obj.getName();
    if( title == null) title = obj.getFtsObject().getDescription();
    setTitle( "Inspector : "+title);

    parent = obj.getSketchPad().getEditorContainer().getFrame();
    this.obj = obj;
    this.controlPanel = obj.getControlPanel();

    ((JPanel)controlPanel).setBorder( BorderFactory.createCompoundBorder(  BorderFactory.createEmptyBorder( 5, 5, 5, 5), BorderFactory.createEtchedBorder()));

    getContentPane().add( (JPanel) controlPanel);

    getContentPane().validate();
    pack();  

    setResizable(false);
    setLocation( parent.getX() + obj.getX() + 10, parent.getY() + obj.getY() + 10 + 40);
    setVisible(true);

    addWindowListener( new WindowAdapter(){
	public void windowClosing(WindowEvent e)
	{
	  controlPanel.done();
	}
      });
  }
}










