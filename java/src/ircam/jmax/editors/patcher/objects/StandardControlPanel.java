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

package ircam.jmax.editors.patcher.objects;

import java.util.*;
import java.awt.*;
import java.awt.event.*;

//import javax.swing.*;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JCheckBox;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;
import javax.swing.JSeparator;

import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;
import ircam.jmax.widgets.*;

public class StandardControlPanel extends JPanel implements ActionListener, ObjectControlPanel
{
  GraphicObject target = null;
  JTextField nameField = null;
  String name = null;
  JCheckBox persistentCB = null;
  private boolean persistenceAdded = false;

  public StandardControlPanel( GraphicObject obj)
  {
    super();
    setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));

    target = obj;
    name = target.getFtsObject().getVariableName();

    /* persistence handling */
    if( target.getFtsObject().isPersistent() != -1)
      {
	persistentCB = new JCheckBox("Persistence");
	persistentCB.addActionListener( new ActionListener(){
	    public void actionPerformed(ActionEvent e)
	    {
	      int persist = ((JCheckBox)e.getSource()).isSelected() ? 1 : 0;
	      target.getFtsObject().requestSetPersistent( persist);
	    }
	  });
	persistentCB.setSelected( target.getFtsObject().isPersistent() == 1);

	add( persistentCB);
	
	persistenceAdded = true;
      }
    
    /* name handling */
    if( obj.getName() != null)
      {
	if( persistenceAdded)
	  add( new JSeparator());

	JLabel titleLabel = new JLabel("Name", JLabel.RIGHT);
	titleLabel.setForeground(Color.black);

	nameField = new JTextField();
	nameField.setPreferredSize(new Dimension(180, 20));
	nameField.addActionListener(this);
	nameField.setText( name);    

	JPanel namePanel = new JPanel();
	namePanel.setLayout( new BoxLayout( namePanel, BoxLayout.X_AXIS));    
	namePanel.add(Box.createRigidArea(new Dimension(5, 0)));  
	namePanel.add(titleLabel);    
	namePanel.add(Box.createRigidArea(new Dimension(5, 0)));  
	namePanel.add( nameField);

	add(namePanel);
      }

    validate();
  }

  public void update( GraphicObject obj)
  {
    target = obj;
    name = target.getFtsObject().getVariableName();
    nameField.setText( name);    
    persistentCB.setSelected( target.getFtsObject().isPersistent() == 1);
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











