package ircam.jmax.editors.ermes;

import java.awt.*;
import ircam.jmax.utils.*;

/**
 * A dialog used to edit the value inside a "float box".
 */
class ErmesObjFloatDialog extends Dialog {
  Frame itsParent;
  Button okButton;
  Button cancelButton;
  TextField value;
  ErmesObjFloat itsFloatObject;
  String itsValue = "";
  
  
  public ErmesObjFloatDialog(Frame theFrame, ErmesObjFloat theFloatObject) {
    super(theFrame, "Float setting", true);
    
    itsFloatObject = theFloatObject;
    itsParent = theFrame;
    setLayout(new BorderLayout());
    
    //Create north section.
    Panel p1 = new Panel();
    p1.setLayout(new FlowLayout(FlowLayout.LEFT));
    
    p1.add(new Label("Float Value"));
    value = new TextField(/*itsFloatObject.GetFloatString()*/"", 20);
    p1.add(value);
    
    add("North",p1);
    
    //Create south section.
    Panel p2 = new Panel();
    p2.setLayout(new BorderLayout());
    
    okButton = new Button("OK");
    okButton.setBackground(Color.white);
    p2.add("East", okButton);
    cancelButton = new Button("Cancel");
    cancelButton.setBackground(Color.white);
    p2.add("West", cancelButton);
    
    add("South", p2);
    //Initialize this dialog to its preferred size.
    pack();
  }

  public boolean action(Event event, Object arg) {
    
    Float aFloat = null;
    if ( event.target == okButton) {
      //	Ok action
      itsValue = value.getText();
      try{
	aFloat = new Float(itsValue);
      }
      catch (NumberFormatException e){
	hide();
	return false;
      }
      itsFloatObject.FromDialogValueChanged(aFloat);
      hide();
    }
    else if ( event.target == cancelButton) {
      //	Cancel action
      hide();
    }
    else if ( event.target == value) {
      itsValue = value.getText();
      //	Connection choose action
    }
    return true;
  }
    
  public void ReInit(String theValue, ErmesObjFloat theFloat, Frame theFrame){
    itsValue = theValue;
    value.setText(theValue);
    itsFloatObject = theFloat;
    itsParent = theFrame;
  }
    
  public boolean keyDown(Event evt,int key) {
    Float aFloat = null;
    if (key == ircam.jmax.utils.Platform.RETURN_KEY){	
      itsValue = value.getText();
      try{
	aFloat = new Float(itsValue);
      }
      catch (NumberFormatException e){
	hide();
	return false;
      }
      itsFloatObject.FromDialogValueChanged(aFloat);
      hide();
      return true;
    }
    return false;
  }
}
