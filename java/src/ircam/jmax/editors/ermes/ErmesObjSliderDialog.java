package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import ircam.jmax.utils.*;


class ErmesObjSliderDialog extends Dialog implements KeyListener, ActionListener{
  Frame itsParent;
  Button okButton;
  Button cancelButton;
  TextField itsMaxValueField, itsMinValueField, itsCurrentValueField;
  ErmesObjSlider itsSliderObject;
  String itsMaxValue = "";
  String itsMinValue = "";
  String itsCurrentValue = "";
  
  public ErmesObjSliderDialog(Frame theFrame, ErmesObjSlider theSliderObject) {
    super(theFrame, "Slider setting", true);
    
    itsSliderObject = theSliderObject;
    itsParent = theFrame;
    setLayout(new BorderLayout());
    
    //Create north section.
    Panel p1 = new Panel();
    p1.setLayout(new GridLayout(3,1));

    Panel p11 = new Panel();
    p11.setLayout(new FlowLayout(FlowLayout.LEFT));
    p11.add(new Label("Slider Current Value    "));
    itsCurrentValueField = new TextField("", 20);
    itsCurrentValueField.addActionListener(this);
    p11.add(itsCurrentValueField);
    p1.add(p11);


    Panel p12 = new Panel();
    p12.setLayout(new FlowLayout(FlowLayout.LEFT));
    p12.add(new Label("Slider Maximum Value"));
    itsMaxValueField = new TextField("", 20);
    itsMaxValueField.addActionListener(this);
    p12.add(itsMaxValueField);
    p1.add(p12);
    
    Panel p13 = new Panel();
    p13.setLayout(new FlowLayout(FlowLayout.LEFT));
    p13.add(new Label("Slider Minimum Value"));
    itsMinValueField = new TextField("", 20);
    itsMinValueField.addActionListener(this);
    p13.add(itsMinValueField);    
    p1.add(p13);

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
    Integer aInt = null;
    if (e.getSource() == okButton) {
      itsMaxValue = itsMaxValueField.getText();
      try{
	aInt = new Integer(itsMaxValue);
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
    else if (e.getSource()==itsMaxValueField) {
      itsMaxValue = itsMaxValueField.getText();
    }
  }
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// actionListener --fine
    
  public void ReInit(String theMaxValue, String theMinValue, String theCurrentValue,ErmesObjSlider theSlider, Frame theFrame){
    itsMaxValue = theMaxValue;
    itsMinValue = theMinValue;
    itsCurrentValue = theCurrentValue;
    itsMaxValueField.setText(theMaxValue);
    itsMinValueField.setText(theMinValue);
    itsCurrentValueField.setText(theCurrentValue);
    itsSliderObject = theSlider;
    itsParent = theFrame;
  }
    

 /////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////keyListener --inizio
  
  public void keyTyped(KeyEvent e){}
  public void keyReleased(KeyEvent e){}

  public void keyPressed(KeyEvent e){
    Integer aInteger = null;
    if (e.getKeyCode() == ircam.jmax.utils.Platform.RETURN_KEY){	
      itsMaxValue = itsMaxValueField.getText();
      try{
	aInteger = new Integer(itsMaxValue);
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







