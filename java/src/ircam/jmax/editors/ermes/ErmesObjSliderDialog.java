package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import ircam.jmax.utils.*;


class ErmesObjSliderDialog extends Dialog implements KeyListener, ActionListener{
  Frame itsParent;
  Button okButton;
  Button cancelButton;
  TextField itsMaxValueField, itsMinValueField, itsCurrentValueField;
  ErmesObjSlider itsSliderObject = null;
  String itsMaxValue = "";
  String itsMinValue = "";
  String itsCurrentValue = "";
  
  public ErmesObjSliderDialog(Frame theFrame) {
    super(theFrame, "Slider setting", true);
    
    itsParent = theFrame;
    setLayout(new BorderLayout());
    
    //Create north section.
    Panel p1 = new Panel();
    p1.setLayout(new GridLayout(1,2));
    
    Panel p11 = new Panel();
    p11.setLayout(new GridLayout(3,1));
    p11.add(new Label("Slider Current Value"));
    p11.add(new Label("Slider Maximum Value"));
    p11.add(new Label("Slider Minimum Value"));
    p1.add(p11);
    
    Panel p12 = new Panel();
    p12.setLayout(new GridLayout(3,1));
    itsCurrentValueField = new TextField("", 20);
    itsCurrentValueField.addActionListener(this);
    p12.add(itsCurrentValueField);
    itsMaxValueField = new TextField("", 20);
    itsMaxValueField.addActionListener(this);
    p12.add(itsMaxValueField);
    itsMinValueField = new TextField("", 20);
    itsMinValueField.addActionListener(this);
    p12.add(itsMinValueField);    
    p1.add(p12);

    add("North", p1);

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
    Integer aMaxInt = null;
    Integer aCurrentInt = null;
    Integer aMinInt = null;
    if (e.getSource() == okButton) {
      itsMaxValue = itsMaxValueField.getText();
      itsCurrentValue = itsCurrentValueField.getText();
      itsMinValue = itsMinValueField.getText();
      try{
	aMaxInt = new Integer(itsMaxValue);
	aCurrentInt = new Integer(itsCurrentValue);
	aMinInt = new Integer(itsMinValue);
      }
      catch (NumberFormatException e1){
	setVisible(false);
	return;
      }
      itsSliderObject.FromDialogValueChanged(aCurrentInt, aMaxInt, aMinInt);
      setVisible(false);
    }
    else if (e.getSource()==cancelButton) {
      setVisible(false);
    }
    else if (e.getSource()==itsMaxValueField) {
      itsMaxValue = itsMaxValueField.getText();
    }
    else if (e.getSource()==itsCurrentValueField) {
      itsCurrentValue = itsCurrentValueField.getText();
    }
    else if (e.getSource()==itsMinValueField) {
      itsMinValue = itsMinValueField.getText();
    }
  }
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// actionListener --fine
    
  public void ReInit(String theMaxValue, String theMinValue, String theCurrentValue,ErmesObjSlider theSlider){
    itsMaxValue = theMaxValue;
    itsMinValue = theMinValue;
    itsCurrentValue = theCurrentValue;
    itsMaxValueField.setText(theMaxValue);
    itsMinValueField.setText(theMinValue);
    itsCurrentValueField.setText(theCurrentValue);
    itsSliderObject = theSlider;
  }
    

 /////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////keyListener --inizio
  
  public void keyTyped(KeyEvent e){}
  public void keyReleased(KeyEvent e){}

  public void keyPressed(KeyEvent e){
    Integer aMaxInt = null;
    Integer aCurrentInt = null;
    Integer aMinInt = null;
    if (e.getKeyCode() == ircam.jmax.utils.Platform.RETURN_KEY){	
      itsMaxValue = itsMaxValueField.getText();
      itsCurrentValue = itsCurrentValueField.getText();
      itsMinValue = itsMinValueField.getText();
      try{
	aMaxInt = new Integer(itsMaxValue);
	aCurrentInt = new Integer(itsCurrentValue);
	aMinInt = new Integer(itsMinValue);
      }
      catch (NumberFormatException e1){
	setVisible(false);
	return;
      }
      itsSliderObject.FromDialogValueChanged(aCurrentInt, aMaxInt, aMinInt);
      setVisible(false);
    }
  }
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// keyListener --fine
}










