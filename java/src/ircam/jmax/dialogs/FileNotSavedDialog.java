package ircam.jmax.dialogs;

import java.awt.*;
import java.awt.event.*;
import ircam.jmax.*;
import ircam.jmax.utils.*;

/**
 * A generic "file not saved" dialog
 */
public class FileNotSavedDialog extends Dialog {
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
   //p2.add("East", itsSaveButton);
   p2.add(itsSaveButton);
   itsNoButton = new Button("Don't Save");
   itsNoButton.setBackground(Color.white);
   //p2.add("Center", itsNoButton);
   p2.add(itsNoButton);
   itsCancelButton = new Button("Cancel");
   itsCancelButton.setBackground(Color.white);
   //p2.add("West", itsCancelButton);
   p2.add(itsCancelButton);
   
   add("South", p2);
   
   pack();
 }

  public boolean action(Event event, Object arg) {
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
  }

  public boolean GetToSaveFlag(){
    return itsToSave;
  }

  public boolean GetNothingToDoFlag(){
    return itsNothingToDo;
  }
  public boolean keyDown(Event evt,int key) {
    if (key == ircam.jmax.utils.Platform.RETURN_KEY){
      itsToSave = true;
      hide();
      return true;
    }
    return false;
  }
  
}



