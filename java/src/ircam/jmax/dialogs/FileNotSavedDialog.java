package ircam.jmax.dialogs;

import java.awt.*;
import java.awt.event.*;
import ircam.jmax.*;
import ircam.jmax.utils.*;

/**
 * A generic "file not saved" dialog
 */
public class FileNotSavedDialog extends Dialog implements ActionListener, KeyListener{
  MaxWindow itsParent;
  String itsMessage;
  Button itsSaveButton, itsCancelButton, itsNoButton;
  boolean itsToSave = true;
  boolean itsNothingToDo;

  public FileNotSavedDialog(Frame theFrame) {
    super(theFrame, "File Not Saved Message", true);
    itsParent = (MaxWindow)theFrame;
    itsMessage = "File "+itsParent.GetDocument().GetName()+" is not saved."+'\n'+" Do you want to save it now?";
    
    setLayout(new BorderLayout());
    
    //Create middle section.
    Panel p1 = new Panel();
    Label label = new Label(itsMessage);
    p1.add(label);
    add("Center", p1);
    
    //Create bottom row.
    Panel p2 = new Panel();
    // p2.setLayout(new BorderLayout());
    p2.setLayout(new GridLayout(1,3));
    itsSaveButton = new Button("Save");
    itsSaveButton.setBackground(Color.white);
    itsSaveButton.addActionListener(this);
    //p2.add("East", itsSaveButton);
    p2.add(itsSaveButton);
    itsNoButton = new Button("Don't Save");
    itsNoButton.setBackground(Color.white);
    itsNoButton.addActionListener(this);
    
    //p2.add("Center", itsNoButton);
    p2.add(itsNoButton);
    itsCancelButton = new Button("Cancel");
    itsCancelButton.setBackground(Color.white);
    itsCancelButton.addActionListener(this);
    //p2.add("West", itsCancelButton);
    p2.add(itsCancelButton);
    
    add("South", p2);
    
    addKeyListener(this);

    pack();
  }

  public void actionPerformed(ActionEvent e){    
    if(e.getSource() == itsSaveButton){
      itsToSave = true;
      itsNothingToDo = false;
      setVisible(false);
    }
    if(e.getSource() == itsCancelButton){
      itsToSave = false;
      itsNothingToDo = true;
      setVisible(false);
    }
    if(e.getSource() == itsNoButton){
      itsToSave = false;
      itsNothingToDo = false;
      setVisible(false);
    }
  }

  /*public boolean action(Event event, Object arg) {
    if (event.target == itsSaveButton){
    itsToSave = true;
    itsNothingToDo = false;
    hide();
    }
    else if(event.target == itsCancelButton){
    itsToSave = false;
    itsNothingToDo = true;
    hide();
    }
    else if(event.target == itsNoButton){
    itsToSave = false;
    itsNothingToDo = false;
    hide();
    }
    return true;
    }*/

  public boolean GetToSaveFlag(){
    return itsToSave;
  }

  public boolean GetNothingToDoFlag(){
    return itsNothingToDo;
  }
  
  /*public boolean keyDown(Event evt,int key) {
    if (key == ircam.jmax.utils.Platform.RETURN_KEY){
    itsToSave = true;
    hide();
    return true;
    }
    return false;
    }*/

  //////////////////////////////////////////////////////////////////////
  //////////////////////////////////////////////////keyListener--inizio
  public void keyTyped(KeyEvent e){}
  public void keyReleased(KeyEvent e){}
  
  public void keyPressed(KeyEvent e){
    if (e.getKeyCode() == ircam.jmax.utils.Platform.RETURN_KEY){
      itsToSave = true;
      setVisible(false);
    }
  }
  /////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////keyListener--fine
}








