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

package ircam.jmax.editors.patcher;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;


/* Not static any ore */

public class ScaleDialog extends JDialog implements ActionListener {

  JButton okButton, cancelButton;
  JTextField xscaleField, yscaleField;
  ErmesSketchWindow sketchWindow;
  String xscale = "";
  String yscale = "";

  public ScaleDialog(ErmesSketchWindow frame) 
  {
    super(frame, "Rescale Window", true);
    getContentPane().setLayout(new GridLayout(3,1));

    JPanel p1 = new JPanel();
    p1.setLayout(new BoxLayout(p1, BoxLayout.X_AXIS));
    p1.setBorder(BorderFactory.createEtchedBorder());
    JLabel xLabel = new JLabel(" X scale factor: ", JLabel.CENTER);
    xLabel.setAlignmentY(Component.CENTER_ALIGNMENT);
    p1.add(xLabel);
    xscaleField = new JTextField();
    xscaleField.addActionListener( this);
    xscaleField.setAlignmentX(Component.CENTER_ALIGNMENT);
    p1.add( xscaleField);

    JPanel p2 = new JPanel();
    p2.setLayout(new BoxLayout(p2, BoxLayout.X_AXIS));
    p2.setBorder(BorderFactory.createEtchedBorder());
    JLabel yLabel = new JLabel(" Y scale factor: ", JLabel.CENTER);
    yLabel.setAlignmentY(Component.CENTER_ALIGNMENT);
    p2.add(yLabel);
    yscaleField = new JTextField();
    yscaleField.addActionListener( this);
    yscaleField.setAlignmentX(Component.CENTER_ALIGNMENT);
    p2.add( yscaleField);

    getContentPane().add(p1);
    getContentPane().add(p2);

    JPanel p4 = new JPanel();
    p4.setLayout(new BoxLayout(p4, BoxLayout.X_AXIS));
    
    okButton = new JButton("OK"); 
    okButton.setPreferredSize(new Dimension(90, 30));
    okButton.setAlignmentY(Component.CENTER_ALIGNMENT);
    okButton.addActionListener( this);

    cancelButton = new JButton("Cancel");
    cancelButton.setPreferredSize(new Dimension(90, 30));
    cancelButton.setAlignmentY(Component.CENTER_ALIGNMENT);
    cancelButton.addActionListener( this);

    p4.add(Box.createHorizontalGlue());
    p4.add(okButton);
    p4.add(cancelButton);
    p4.add(Box.createHorizontalGlue());

    getContentPane().add(p4);
    
    pack();

    setResizable(false);
    
    init(frame);
  }

  public void actionPerformed( ActionEvent e)
  {
    float fsx = (float)0.0;
    float fsy = (float)0.0;

    if ( e.getSource() == cancelButton) 
      {
	setVisible( false);
      }
    else  
      { 
	xscale = xscaleField.getText();
	yscale = yscaleField.getText();
	try
	  {
	    fsx = Float.parseFloat( xscale);
	    fsy = Float.parseFloat( yscale);
	  }
	catch (NumberFormatException e1)
	  {
	    setVisible(false);
	    return;
	  }
	sketchWindow.scale(fsx, fsy);
	sketchWindow = null; // make the gc happy.
	setVisible( false);
      }
  }
    
  private void init(ErmesSketchWindow frame)
  {
    sketchWindow = frame;
    xscale = "1.0";
    yscale = "1.0";
    xscaleField.setText( xscale);
    yscaleField.setText( yscale);

    xscaleField.requestFocus();//??
  }
}




