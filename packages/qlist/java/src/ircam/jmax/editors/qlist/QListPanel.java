package ircam.jmax.editors.qlist;

import java.lang.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.AWTEvent.*;
import java.io.*;
import tcl.lang.*;
import javax.swing.*;

public class QListPanel extends JPanel implements ActionListener {
  
  //QList itsQList;
  FtsAtomList itsAtomList;
  TextArea itsTextArea;
  int caretPosition;
  Button itsSetButton;
  Button itsGetButton;

  String textToFind;
  int lastFindIndex;

  public QListPanel(FtsAtomList theAtomList) 
  { 
    super();

    itsTextArea = new TextArea(40, 40);
    caretPosition = 0;

    Panel aPanel = new Panel();
    aPanel.setLayout(new GridLayout(1, 2));
    itsSetButton = new Button("set");
    itsSetButton.addActionListener(this);
    itsGetButton = new Button("get");
    itsGetButton.addActionListener(this);
    aPanel.add(itsSetButton);
    aPanel.add(itsGetButton);
    aPanel.validate();

    setLayout(new BorderLayout());
    add("North", aPanel);

    add("Center", itsTextArea);
    validate();

    itsAtomList = theAtomList;

    textToFind = "";
    lastFindIndex = 0;

    setBackground(Color.white);
  }

  public void fillContent(FtsAtomList theContent) 
  {
    itsTextArea.setText( theContent.getValuesAsText());

    itsAtomList = theContent;
  }
 
  public void actionPerformed(ActionEvent e) 
  {
    caretPosition = itsTextArea.getCaretPosition();

    if (e.getSource() == itsSetButton) 
      {
	itsAtomList.setValuesAsText( itsTextArea.getText());
      }
    else if (e.getSource() == itsGetButton) 
      {
	itsAtomList.forceUpdate();
      }

    //in every case, update the content
    fillContent(itsAtomList);

    itsTextArea.requestFocus();
    itsTextArea.setCaretPosition( caretPosition);
  }

  public Dimension preferredSize() 
  {
    Dimension d = new Dimension(512, 412);
    return d;
  }

  public Dimension minimumSize() 
  {
    return preferredSize();
  }

  // (fd) {
  public void jumpToZero()
  {
    itsTextArea.setCaretPosition( 0);
  }

  public int getCaretPosition()
  {
    return itsTextArea.getCaretPosition();
  }

  public int find( String textToFind, int fromIndex)
  {
    int index = itsTextArea.getText().indexOf( textToFind, fromIndex);

    if ( index >= 0)
      {
	itsTextArea.select( index, index + textToFind.length());
      }

    return index;
  }
  // } (fd)
}
