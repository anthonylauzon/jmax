
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

	setLocation(200, 200);
	Dimension dim = namePanel.getSize();
	if(dim.height+30>700) dim.height = 700;
	else dim.height += 30;
	setSize(dim);

	nameField.setText(track.getName());
    }

    public static void changeName(Track track, Frame frame)
    {
	ChangeTrackNameDialog dialog = new ChangeTrackNameDialog(track, frame);
	dialog.requestFocus();//????
	dialog.setVisible(true);
    }
}





