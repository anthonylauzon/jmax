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
// Authors: Francois Dechelle, Norbert Schnell, Riccardo Borghesi.
// 

package ircam.jmax.guiobj;

import java.util.*;
import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;

import ircam.jmax.editors.patcher.objects.*;

public class FloatBoxControlPanel extends JPanel implements ActionListener, ObjectControlPanel
{
  GraphicObject target = null;
  JTextField incrField;
  double incr;

  public FloatBoxControlPanel()
  {
    super();
    setLayout(new BoxLayout(this, BoxLayout.Y_AXIS));

    add( new JSeparator());

    JLabel incrLabel = new JLabel("Increment", JLabel.CENTER);
    incrLabel.setForeground(Color.black);

    Box labelBox = new Box(BoxLayout.X_AXIS);
    labelBox.add(Box.createRigidArea(new Dimension(20, 0)));    
    labelBox.add(incrLabel);    
    labelBox.add(Box.createHorizontalGlue());    

    add(labelBox);    

    incrField = new JTextField();
    incrField.setPreferredSize(new Dimension(120, 20));
    incrField.setMaximumSize(new Dimension(120, 20));
    incrField.addActionListener(this);
    incrField.requestDefaultFocus();

    Box fieldBox = new Box( BoxLayout.X_AXIS);
    fieldBox.add( Box.createRigidArea(new Dimension(20, 0)));    
    fieldBox.add( incrField);    
    fieldBox.add( Box.createHorizontalGlue());    

    add( fieldBox);

    validate();
  }

  public void update(GraphicObject obj)
  {
    target = obj;
    incr = ( (FloatBox)obj).getDecimalIncrement();
    incrField.setText( ((FloatBox)obj).getDecimalIncrementAsText());    
  }

  public void done()
  {
    setDecimalIncrement();
  }

  public void setDecimalIncrement()
  {
    try
      {
	incr = Double.parseDouble( incrField.getText());
      }
    catch (NumberFormatException e1)
      {
	return;
      }
    ((FloatBox)target).setDecimalIncrement( incr);
  }

  public void actionPerformed( ActionEvent e)
  {
    if( e.getSource() == incrField)
      setDecimalIncrement();
  }
}











