
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

	setLocation(200, 200);
	Dimension dim = panel.getSize();
	if(dim.height+30>700) dim.height = 700;
	else dim.height += 30;
	setSize(dim);

	maximumField.setText(""+((Integer)track.getProperty("maximumValue")).intValue());
	minimumField.setText(""+((Integer)track.getProperty("minimumValue")).intValue());
    }

    public static void changeRange(Track track, Frame frame)
    {
	ChangeRangeDialog dialog = new ChangeRangeDialog(track, frame);
	dialog.requestFocus();//????
	dialog.setVisible(true);
    }
}





