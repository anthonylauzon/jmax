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

public class QListPanel extends JPanel {
  
  QList itsQList;
  TextArea itsTextArea;
  
  public QListPanel(QList theQList) {
    super();
    itsTextArea = new TextArea(40, 40);
    setLayout(new BorderLayout());
    add("Center", itsTextArea);
    validate();
    itsQList = theQList;
    setBackground(Color.white);
  }

  public void fillContent(FtsAtomList theContent) {
    itsTextArea.setText(theContent.getValuesAsText());
  }
 
  public Dimension preferredSize() {
    Dimension d = new Dimension(512, 412);
    return d;
  }

  public Dimension minimumSize() {
    return preferredSize();
  }
}







