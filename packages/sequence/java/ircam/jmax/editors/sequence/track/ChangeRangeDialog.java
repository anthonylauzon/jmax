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

public class ChangeRangeDialog extends JDialog {


    JTextField maximumField, minimumField;
    Track track;
    
    public ChangeRangeDialog(Track theTrack, Frame frame)
    {
	super(frame, "Change Range Dialog", true);
	track = theTrack;

	JPanel panel = new JPanel();
	panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));
	
	JPanel maximumPanel = new JPanel();
	maximumPanel.setBorder(BorderFactory.createEtchedBorder());
	maximumPanel.setLayout(new BoxLayout(maximumPanel, BoxLayout.X_AXIS));
	maximumPanel.add(new JLabel("maximum value", JLabel.LEFT));
	
	maximumField = new JTextField();
	maximumField.setBorder(BorderFactory.createEtchedBorder());
	maximumField.setEditable(true);     
	maximumField.setPreferredSize(new Dimension(120, 28));  
	maximumField.setMaximumSize(new Dimension(120, 28));
	maximumField.addKeyListener(new KeyListener(){
	  public void keyPressed(KeyEvent e){
	      int value;
	      if(e.getKeyCode()==KeyEvent.VK_ENTER)
		  {
		      try { 
			  value = Integer.valueOf(maximumField.getText()).intValue(); // parse int
		      } catch (NumberFormatException exc) {
			  System.err.println("Error:  invalid number format!");
			  return;
		      }
		      track.setProperty("maximumValue", new Integer(value));
		  }  
	  }
	  public void keyReleased(KeyEvent e){}
	  public void keyTyped(KeyEvent e){}
	});
	maximumPanel.add(maximumField);

	JPanel minimumPanel = new JPanel();
	minimumPanel.setBorder(BorderFactory.createEtchedBorder());
	minimumPanel.setLayout(new BoxLayout(minimumPanel, BoxLayout.X_AXIS));
	minimumPanel.add(new JLabel("minimum value", JLabel.LEFT));
	
	minimumField = new JTextField();
	minimumField.setBorder(BorderFactory.createEtchedBorder());
	minimumField.setEditable(true);
	minimumField.setPreferredSize(new Dimension(120, 28));  
	minimumField.setMaximumSize(new Dimension(120, 28));
	minimumField.addKeyListener(new KeyListener(){
	  public void keyPressed(KeyEvent e){
	      int value;
	      if(e.getKeyCode()==KeyEvent.VK_ENTER)
		  {
		      try { 
			  value = Integer.valueOf(minimumField.getText()).intValue(); // parse int
		      } catch (NumberFormatException exc) {
			  System.err.println("Error:  invalid number format!");
			  return;
		      }
		      track.setProperty("minimumValue", new Integer(value));
		  }  
	  }
	  public void keyReleased(KeyEvent e){}
	  public void keyTyped(KeyEvent e){}
	});

	minimumPanel.add(minimumField);
	
	panel.add(maximumPanel);
	panel.add(minimumPanel);

	getContentPane().add(panel);
	
	getContentPane().validate();
	
	validate();
	pack();

	//setLocation(200, 200);
	Dimension dim = panel.getSize();
	if(dim.height+30>700) dim.height = 700;
	else dim.height += 30;
	setSize(dim);

	maximumField.setText(""+((Integer)track.getProperty("maximumValue")).intValue());
	minimumField.setText(""+((Integer)track.getProperty("minimumValue")).intValue());
    }

    public static void changeRange(Track track, Frame frame, Point position)
    {
	ChangeRangeDialog dialog = new ChangeRangeDialog(track, frame);
	dialog.setLocation(frame.getLocation().x + position.x, frame.getLocation().y + position.y);
	dialog.requestFocus();
	dialog.setVisible(true);
    }
}





