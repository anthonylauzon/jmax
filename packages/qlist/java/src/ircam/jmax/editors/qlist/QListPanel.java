package ircam.jmax.editors.qlist;

import java.lang.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.AWTEvent.*;
import java.io.*;
import tcl.lang.*;
import com.sun.java.swing.*;

public class QListPanel extends JPanel implements ActionListener {
  
  QList itsQList;
  TextArea itsTextArea;
  Button itsSendButton;
  
  public QListPanel(QList theQList) { 
    super();
    itsTextArea = new TextArea(40, 40);
    itsSendButton = new Button("Send");

    setLayout(new BorderLayout());
    add("North", itsSendButton);
    add("Center", itsTextArea);
    validate();
    itsQList = theQList;
    setBackground(Color.white);
  }

  public void fillContent(FtsAtomList theContent) {
    itsTextArea.setText(theContent.getValuesAsText());
  }
 
  public void actionPerformed(ActionEvent e) {
  }

  public Dimension preferredSize() {
    Dimension d = new Dimension(512, 412);
    return d;
  }

  public Dimension minimumSize() {
    return preferredSize();
  }
}







