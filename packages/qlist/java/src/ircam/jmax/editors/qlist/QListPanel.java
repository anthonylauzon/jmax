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
  
  //QList itsQList;
  FtsAtomList itsAtomList;
  TextArea itsTextArea;
  Button itsSendButton;
  
  public QListPanel(FtsAtomList theAtomList) { 
    super();
    itsTextArea = new TextArea(40, 40);
    itsSendButton = new Button("Send");

    setLayout(new BorderLayout());
    add("North", itsSendButton);
    add("Center", itsTextArea);
    validate();
    itsAtomList = theAtomList;
    setBackground(Color.white);
  }

  public void fillContent(FtsAtomList theContent) {
    itsTextArea.setText(theContent.getValuesAsText());
    itsAtomList = theContent;
  }
 
  public void actionPerformed(ActionEvent e) {
    if (e.getSource() == itsSendButton)
      itsAtomList.setValuesAsText(itsTextArea.getText());
  }

  public Dimension preferredSize() {
    Dimension d = new Dimension(512, 412);
    return d;
  }

  public Dimension minimumSize() {
    return preferredSize();
  }
}







