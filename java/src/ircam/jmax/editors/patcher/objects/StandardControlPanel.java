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

import java.util.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;
import ircam.jmax.widgets.*;

public class StandardControlPanel extends JPanel implements ActionListener, ObjectControlPanel
{
  GraphicObject target = null;
  JTextField nameField;
  String name;

  public StandardControlPanel()
  {
    super();
    setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));

    JLabel titleLabel = new JLabel("Name", JLabel.RIGHT);
    titleLabel.setForeground(Color.black);

    nameField = new JTextField();
    nameField.setPreferredSize(new Dimension(100, 20));
    nameField.setMaximumSize(new Dimension(100, 20));
    nameField.addActionListener(this);

    JPanel namePanel = new JPanel();
    namePanel.setPreferredSize(new Dimension(150, 20));
    namePanel.setLayout( new BoxLayout( namePanel, BoxLayout.X_AXIS));    
    namePanel.add(Box.createRigidArea(new Dimension(5, 0)));  
    namePanel.add(titleLabel);    
    namePanel.add(Box.createRigidArea(new Dimension(5, 0)));    
    namePanel.add( nameField);
    namePanel.add(Box.createHorizontalGlue());    

    add(namePanel);

    add( new JSeparator());

    validate();
  }

  public void update( GraphicObject obj)
  {
    target = obj;
    name = obj.getFtsObject().getVariableName();
    nameField.setText( name);    
  }

  public void done()
  {
    setName();
  }

  public void actionPerformed( ActionEvent e)
  {
    setName();
  }

  public void setName()
  {
    String text = nameField.getText().trim();

    if( text != null)
      if( ((this.name == null) && !text.equals("")) || ((this.name != null) && !text.equals( this.name)))
	{
	  target.getFtsObject().requestSetName( text);
	  name = text;
	}
  }
}











