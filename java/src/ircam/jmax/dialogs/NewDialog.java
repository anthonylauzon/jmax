package ircam.jmax.dialogs;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.utils.*;


/**
 * The dialog used during a menu-new operation.
 */
public class NewDialog extends Dialog implements ItemListener, ActionListener, KeyListener{
  Frame itsParent;
  Button okButton;
  Button cancelButton;
  List itsList;
  
  String itsCurrentItem = "";
  
  
  public NewDialog(Frame theFrame) {
    super(theFrame, "New File", true);
    
    itsParent = theFrame;
    
    setLayout(new BorderLayout());
		
    //Create north section.
    Panel p1 = new Panel();
    p1.setLayout(new BorderLayout());
    
    p1.add("North", new Label("New File Type"));
    itsList = new List(5, false);
    MaxDataType aDataType;
    String aString;
    for(Enumeration e = MaxDataType.getTypes().elements(); e.hasMoreElements();) {
      aDataType = (MaxDataType) e.nextElement();
      aString = aDataType.getName();
      itsList.add(aString);
    }
    itsList.addItemListener(this);
    itsList.addActionListener(this);
    
    p1.add("South", itsList);
    
    add("North",p1);
    
    //Create south section.
    Panel p2 = new Panel();
    p2.setLayout(new BorderLayout());
    
    okButton = new Button("OK");
    okButton.setBackground(Color.white);
    okButton.addActionListener(this);
    p2.add("East", okButton);
    cancelButton = new Button("Cancel");
    cancelButton.setBackground(Color.white);
    cancelButton.addActionListener(this);
    p2.add("West", cancelButton);
    
    add("South", p2);
    //Initialize this dialog to its preferred size.
    pack();

    addKeyListener(this);
  }
  
  public void itemStateChanged(ItemEvent e){
    if(e.getStateChange() == ItemEvent.SELECTED) itsCurrentItem = (itsList.getSelectedItems())[0];
  }
  //////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////keyListener--inizio
  public void actionPerformed(ActionEvent e){    
    if (e.getSource() == okButton) setVisible(false);
    else if (e.getSource() == cancelButton) {
      itsCurrentItem = "";
      setVisible(false);
    }
    else if(e.getSource() == itsList) setVisible(false);
  }
  ///////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////keyListener--fine

  public String GetNewFileType(){
    return itsCurrentItem;
  }

  //////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////keyListener--inizio
  public void keyTyped(KeyEvent e){}
  public void keyReleased(KeyEvent e){}

  public void keyPressed(KeyEvent e){
    Float aFloat = null;
    if (e.getKeyCode() == ircam.jmax.utils.Platform.RETURN_KEY)
      setVisible(false);
  }
  /////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////keyListener--fine

}




