package ircam.jmax.editors.patcher.objects;

import java.awt.*;
import java.awt.event.*;

import ircam.jmax.utils.*;
import ircam.jmax.editors.patcher.*;

/* Not static any ore */

class ErmesObjSliderDialog extends Frame implements ActionListener {

  Button okButton;
  Button cancelButton;
  TextField itsMaxValueField, itsMinValueField, itsCurrentValueField;
  ErmesObjSlider itsSliderObject = null;
  String itsMaxValue = "";
  String itsMinValue = "";
  String itsCurrentValue = "";
  
  public ErmesObjSliderDialog() 
  {
    super( "Slider setting");
    
    setLayout( new BorderLayout());
    
    //Create north section.
    Panel p1 = new Panel();
    p1.setLayout(new GridLayout(1,2));
    
    Panel p11 = new Panel();
    p11.setLayout(new GridLayout(3,1));
    p11.add( new Label("Slider Maximum Value"));
    p11.add( new Label("Slider Minimum Value"));
    p11.add( new Label("Slider Current Value"));
    p1.add( p11);
    
    Panel p12 = new Panel();
    p12.setLayout(new GridLayout(3,1));

    itsMaxValueField = new TextField("", 20);
    itsMaxValueField.addActionListener( this);
    p12.add( itsMaxValueField);

    itsMinValueField = new TextField("", 20);
    itsMinValueField.addActionListener( this);
    p12.add( itsMinValueField);   

    itsCurrentValueField = new TextField("", 20);
    itsCurrentValueField.addActionListener( this);
    p12.add( itsCurrentValueField);

    p1.add( p12);

    add("North", p1);

    //Create south section.
    Panel p2 = new Panel();
    p2.setLayout( new BorderLayout());
    
    okButton = new Button("OK");
    okButton.setBackground( Color.white);
    okButton.addActionListener( this);
    p2.add( "East", okButton);

    cancelButton = new Button( "Cancel");
    cancelButton.setBackground( Color.white);
    cancelButton.addActionListener( this);
    p2.add( "West", cancelButton);
    
    add( "South", p2);

    pack();
  }

  public void actionPerformed( ActionEvent e)
  {
    int aMaxInt = 0;
    int aCurrentInt = 0;
    int aMinInt = 0;

    if ( e.getSource() == cancelButton) 
      {
	setVisible( false);
      }
    else  
      { 
	itsMaxValue = itsMaxValueField.getText();
	itsCurrentValue = itsCurrentValueField.getText();
	itsMinValue = itsMinValueField.getText();
	try
	  {
	    aMaxInt = Integer.parseInt( itsMaxValue);
	    aCurrentInt = Integer.parseInt( itsCurrentValue);
	    aMinInt = Integer.parseInt( itsMinValue);
	  }
	catch (NumberFormatException e1)
	  {
	    setVisible(false);
	    return;
	  }
	itsSliderObject.fromDialogValueChanged( aCurrentInt, aMaxInt, aMinInt);
	itsSliderObject = null; // make the gc happy.
	setVisible( false);
      }
  }
    
  public void ReInit( String theMaxValue, String theMinValue, String theCurrentValue,ErmesObjSlider theSlider, Frame theParent)
  {
    itsMaxValue = theMaxValue;
    itsMinValue = theMinValue;
    itsCurrentValue = theCurrentValue;
    itsMaxValueField.setText( theMaxValue);
    itsMinValueField.setText( theMinValue);
    itsCurrentValueField.setText( theCurrentValue);
    itsSliderObject = theSlider;

    setVisible( true);

    itsMaxValueField.requestFocus();
  }
}
