package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import ircam.jmax.utils.*;


class ErmesObjSliderDialog extends Dialog implements KeyListener, ActionListener{
  Frame itsParent;
  Button okButton;
  Button cancelButton;
  TextField value;
  ErmesObjSlider itsSliderObject;
  String itsValue = "";
  
  
  public ErmesObjSliderDialog(Frame theFrame, ErmesObjSlider theSliderObject) {
    super(theFrame, "Slider range setting", true);
    
    itsSliderObject = theSliderObject;
    itsParent = theFrame;
    setLayout(new BorderLayout());
    
    //Create north section.
    Panel p1 = new Panel();
    p1.setLayout(new FlowLayout(FlowLayout.LEFT));
    
    p1.add(new Label("Slider Maximum Value"));
    value = new TextField("", 20);
    value.addActionListener(this);
    p1.add(value);
    
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

   ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// actionListener --inizio

  public void actionPerformed(ActionEvent e){        
    Integer aInt = null;
    if (e.getSource() == okButton) {
      itsValue = value.getText();
      try{
	aInt = new Integer(itsValue);
      }
      catch (NumberFormatException e1){
	setVisible(false);
	return;
      }
      itsSliderObject.FromDialogValueChanged(aInt);
      setVisible(false);
    }
    else if (e.getSource()==cancelButton) {
      setVisible(false);
    }
    else if (e.getSource()==value) {
      itsValue = value.getText();
    }
  }
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// actionListener --fine

  /*public boolean action(Event event, Object arg) {
    
    Integer aInt = null;
    if ( event.target == okButton) {
    //	Ok action
    itsValue = value.getText();
    try{
    aInt = new Integer(itsValue);
    }
    catch (NumberFormatException e){
    hide();
    return false;
    }
    itsSliderObject.FromDialogValueChanged(aInt);
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
    }*/
    
  public void ReInit(String theValue, ErmesObjSlider theSlider, Frame theFrame){
    itsValue = theValue;
    value.setText(theValue);
    itsSliderObject = theSlider;
    itsParent = theFrame;
  }
    
  /*public boolean keyDown(Event evt,int key) {
    Integer aInteger = null;
    if (key == ircam.jmax.utils.Platform.RETURN_KEY){	
    itsValue = value.getText();
    try{
    aInteger = new Integer(itsValue);
    }
    catch (NumberFormatException e){
    hide();
    return false;
    }
    itsSliderObject.FromDialogValueChanged(aInteger);
    hide();
    return true;
    }
    return false;
    }*/

 /////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////keyListener --inizio
  
  public void keyTyped(KeyEvent e){}
  public void keyReleased(KeyEvent e){}

  public void keyPressed(KeyEvent e){
    Integer aInteger = null;
    if (e.getKeyCode() == ircam.jmax.utils.Platform.RETURN_KEY){	
      itsValue = value.getText();
      try{
	aInteger = new Integer(itsValue);
      }
      catch (NumberFormatException e1){
	setVisible(false);
	return;
      }
      itsSliderObject.FromDialogValueChanged(aInteger);
      setVisible(false);
    }
  }
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// keyListener --fine
}







