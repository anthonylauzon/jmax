
package ircam.jmax.editors.table;

import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
/**
 * */

public class ChangeRangeDialog extends JDialog {

    JTextField maximumField;
    TableGraphicContext tgc;
    TablePanel target;
    
    public ChangeRangeDialog(Frame frame, TableGraphicContext gc, TablePanel tabPanel)
    {
	super(frame, "Change Range Dialog", true);
	tgc = gc; 
	target = tabPanel;

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
		      target.setMaximumValue(value);
		  }  
	  }
	  public void keyReleased(KeyEvent e){}
	  public void keyTyped(KeyEvent e){}
	});
	maximumPanel.add(maximumField);

	panel.add(maximumPanel);

	getContentPane().add(panel);
	
	getContentPane().validate();
	
	validate();
	pack();

	//setLocation(200, 200);
	Dimension dim = panel.getSize();
	if(dim.height+30>700) dim.height = 700;
	else dim.height += 30;
	setSize(dim);

	maximumField.setText(""+gc.getVerticalMaximum());
    }

    public static void changeRange(Frame frame, TableGraphicContext gc, Point position, TablePanel panel)
    {
	ChangeRangeDialog dialog = new ChangeRangeDialog(frame, gc, panel);
	dialog.setLocation(frame.getLocation().x + position.x, frame.getLocation().y + position.y);
	dialog.requestFocus();
	dialog.setVisible(true);
    }
}





