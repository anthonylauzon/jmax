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

package ircam.jmax.editors.sequence.track;

import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.menus.*;
/**
 * */

public class ChangeTrackNameDialog extends JDialog {

    JTextField nameField;
    Track track;
    
    public ChangeTrackNameDialog(Track theTrack, Frame frame)
    {
	super(frame, "Change Track Name Dialog", true);
	track = theTrack;

	//JPanel panel = new JPanel();
	//panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
	
	JPanel namePanel = new JPanel();
	namePanel.setBorder(BorderFactory.createEtchedBorder());
	namePanel.setLayout(new BoxLayout(namePanel, BoxLayout.X_AXIS));
	namePanel.add(new JLabel("track name", JLabel.LEFT));
	
	nameField = new JTextField();
	nameField.setBorder(BorderFactory.createEtchedBorder());
	nameField.setEditable(true);     
	nameField.setPreferredSize(new Dimension(200, 28));  
	nameField.setMaximumSize(new Dimension(200, 28));
	nameField.addKeyListener(new KeyListener(){
	  public void keyPressed(KeyEvent e){
	      int value;
	      if(e.getKeyCode()==KeyEvent.VK_ENTER)
		  {
		      track.getFtsTrack().requestSetName(nameField.getText());
		      setVisible(false);
		  }
	  }
	  public void keyReleased(KeyEvent e){}
	  public void keyTyped(KeyEvent e){}
	});
	namePanel.add(nameField);

	//panel.add(namePanel);

	//getContentPane().add(panel);
	getContentPane().add(namePanel);
	
	getContentPane().validate();
	
	validate();
	pack();

	//setLocation(200, 200);
	Dimension dim = namePanel.getSize();
	if(dim.height+30>700) dim.height = 700;
	else dim.height += 30;
	setSize(dim);

	nameField.setText(track.getName());
    }

    public static void changeName(Track track, Frame frame, Point position)
    {
	ChangeTrackNameDialog dialog = new ChangeTrackNameDialog(track, frame);
	dialog.setLocation(frame.getLocation().x + position.x, frame.getLocation().y + position.y);
	dialog.requestFocus();
	dialog.setVisible(true);
    }
}





