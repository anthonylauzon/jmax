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

package ircam.jmax.guiobj;

import java.util.*;
import java.awt.*;
import java.awt.event.*;

//import javax.swing.*;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JComboBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JSeparator;
import javax.swing.JTextField;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;
import ircam.jmax.widgets.*;

import ircam.jmax.editors.patcher.objects.*;

//
// The graphic pop-up menu used to change the number of an inlet or an outlet in a subpatcher.
//

public class DefineControlPanel extends JPanel implements ActionListener, ObjectControlPanel
{
  GraphicObject target = null;
  JTextField nameField;
  JComboBox typeCombo = null;
  String name = null;
  String type = null;

  public DefineControlPanel( GraphicObject obj)
  {
    super();
    setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));
    
    target = obj;

    JLabel typeLabel = new JLabel("Type", JLabel.CENTER);
    typeLabel.setForeground(Color.black);

    String[] types = { "const", "arg", "args"};
    typeCombo = new JComboBox( types);
    typeCombo.setBackground( Color.white);
    typeCombo.addActionListener(new ActionListener() {
	public void actionPerformed(ActionEvent e) {
            setType();
        }
      });
    //typeCombo.setPreferredSize( new Dimension( 160, 20));

    Box typeBox = new Box(BoxLayout.X_AXIS);
    typeBox.add( Box.createRigidArea(new Dimension(5, 0)));  
    typeBox.add( typeLabel);    
    typeBox.add( Box.createRigidArea( new Dimension( 10, 0)));    
    typeBox.add( typeCombo);    

    add(typeBox);    

    add(new JSeparator());

    /* name handling */
    JLabel nameLabel = new JLabel("Name", JLabel.RIGHT);
    nameLabel.setForeground(Color.black);

    nameField = new JTextField();
    nameField.setPreferredSize(new Dimension(150, 20));
    nameField.addActionListener(this);

    JPanel namePanel = new JPanel();
    namePanel.setLayout( new BoxLayout( namePanel, BoxLayout.X_AXIS));    
    namePanel.add(Box.createRigidArea(new Dimension(5, 0)));  
    namePanel.add(nameLabel);    
    namePanel.add(Box.createRigidArea(new Dimension(5, 0)));  
    namePanel.add( nameField);

    add(namePanel);

    update( obj);
    validate();
  }

  public void update(GraphicObject obj)
  {
    target = obj;
    type = ((Define)obj).getType();
    
    name = target.getFtsObject().getVariableName();
    nameField.setText( name);    
  }

  public void done()
  {
    setType();
    setName();
  }
  
  public void actionPerformed( ActionEvent e)
  {
     setName();
  }

  public void setType()
  {  
    String type = (String) typeCombo.getSelectedItem();	  
    ((FtsDefineObject)((Define)target).getFtsObject()).requestSetType( type);
  }

  public void setName()
  {
    if( nameField != null)
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
}











