
package ircam.jmax.editors.bpf;

import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
/**
 * */

public class ChangeRangeDialog extends JDialog {

    JTextField maximumField, minimumField;
    BpfGraphicContext bgc;
    
    public ChangeRangeDialog(Frame frame, BpfGraphicContext gc)
    {
	super(frame, "Change Range Dialog", true);
	bgc = gc;

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
	      float value;
	      if(e.getKeyCode()==KeyEvent.VK_ENTER)
		  {
		      try { 
			  value = Float.valueOf(maximumField.getText()).floatValue(); // parse int
		      } catch (NumberFormatException exc) {
			  System.err.println("Error:  invalid number format!");
			  return;
		      }
		      bgc.getAdapter().setMaximumValue(value);
		      bgc.getGraphicDestination().repaint();
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
	      float value;
	      if(e.getKeyCode()==KeyEvent.VK_ENTER)
		  {
		      try { 
			  value = Float.valueOf(minimumField.getText()).floatValue();
		      } catch (NumberFormatException exc) {
			  System.err.println("Error:  invalid number format!");
			  return;
		      }
		      bgc.getAdapter().setMinimumValue(value);
		      bgc.getGraphicDestination().repaint();
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

	maximumField.setText(""+gc.getAdapter().getMaximumValue());
	minimumField.setText(""+gc.getAdapter().getMinimumValue());
    }

    public static void changeRange(Frame frame, BpfGraphicContext gc, Point position)
    {
	ChangeRangeDialog dialog = new ChangeRangeDialog(frame, gc);
	dialog.setLocation(frame.getLocation().x + position.x, frame.getLocation().y + position.y);
	dialog.requestFocus();
	dialog.setVisible(true);
    }
}





