package ircam.jmax.editors.explode;

import com.sun.java.swing.*;
import com.sun.java.swing.border.*;
import java.awt.*;

/**
 * the class of the "status" bar on top of the score.
 * It implements the StatusBar interface
 */
public class InfoPanel extends JPanel implements StatusBar{
  
  /** constructor */
  public InfoPanel() {

    setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

    setBackground(Color.lightGray);
    setBorder(new BevelBorder(BevelBorder.RAISED));
    
    itsHeaderFont = new Font(getFont().getName(), Font.BOLD, getFont().getSize());
    itsHeaderLabel = new JLabel("                                        ");
    itsHeaderLabel.setFont(itsHeaderFont);
    add(itsHeaderLabel);

    itsLabel = new JLabel("                                                                                                                                 ");
    itsLabel.setSize(infoSize.width, infoSize.height);
    itsLabel.setBackground(Color.white);
    add(itsLabel);

  }
  
  /** Status bar interface */
  public void post(StatusBarClient theClient, String message) 
  {
    if (theClient.getIcon() != null)    
      itsHeaderLabel.setIcon(theClient.getIcon());
    itsHeaderLabel.setText(theClient.getName()+":");
    itsLabel.setText(message);
  }

  public Dimension getPreferredSize()
  {
    return getMinimumSize();
  }

  public Dimension getMinimumSize() 
  {
    return infoSize;
  }

  //----- Fields
  public static final int INFO_WIDTH = 300;
  public static final int INFO_HEIGHT = 30;
  Dimension infoSize = new Dimension(INFO_WIDTH, INFO_HEIGHT);
  Font itsHeaderFont;
  JLabel itsLabel;

  JLabel itsHeaderLabel;
  Dimension itsHeaderLabelSize = new Dimension(250, INFO_HEIGHT);
}

