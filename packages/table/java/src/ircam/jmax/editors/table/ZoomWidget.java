
package ircam.jmax.editors.table;

import java.awt.*;
import java.awt.event.*;
import java.io.File;
import com.sun.java.swing.*;

import ircam.jmax.*;

/**
 * A statusbar widget containing two buttons (moreButton and lessButton)
 * represented as two vertical (or horizontal) arrows, depending on the
 * direction argument of the constructor. The initialValue is only used
 * to display the widget the first time, but must be controlled from outside */
class ZoomWidget extends Box {

  /** Constructor */
  public ZoomWidget(String name, int initialValue, int direction)
  {
    super(BoxLayout.X_AXIS);
    
    // the name
    JLabel label = new JLabel(name+": ");

    add(label);
    
    // the value as string
    itsValue = new JLabel(""+initialValue+"% ");
    
    add(itsValue);
    
    String fs = File.separator;
    String path = MaxApplication.getProperty("root")+fs+"packages/table/images"+fs;
    // the less button
    ImageIcon aImageIcon;
    if (direction == VERTICAL) aImageIcon = new ImageIcon(path+"up_arrow.gif");
    else aImageIcon = new ImageIcon(path+"left_arrow.gif");
				 
    lessButton = new JButton(aImageIcon)
      {
	    public float getAlignmentY()
	      {
		return (float) 0.5;
	      }
      };
    lessButton.setMargin(new Insets(0, 0, 0, 0));
    add(lessButton);
    
    // the 'more' button
    if (direction == VERTICAL) aImageIcon = new ImageIcon(path+"down_arrow.gif");
    else aImageIcon = new ImageIcon(path+"right_arrow.gif");

    moreButton = new JButton(aImageIcon)
      {
	public float getAlignmentY()
	  {
	    return (float) 0.5;
	  }
      };
    moreButton.setMargin(new Insets(0, 0, 0, 0));
    add(moreButton);
    
  }
  
  /**
   * Sets the given text for the widget, followed by a % sign */ 
  public void setValue(int value)
  {
    itsValue.setText(""+value+"%");
  }

  /**
   * Returns the lessButton */
  public JButton getLessButton()
  {
    return lessButton;
  }

  /**
   * Returns the moreButton */
  public JButton getMoreButton()
  {
    return moreButton;
  }
  
      //--- Fields
  JLabel itsValue;
  JButton lessButton;
  JButton moreButton;

  public static final int VERTICAL = 0;
  public static final int HORIZONTAL = 1;
}
