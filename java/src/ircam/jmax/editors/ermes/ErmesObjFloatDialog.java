package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import ircam.jmax.utils.*;

//
// A dialog used to edit the value inside a "float box".
//
class ErmesObjFloatDialog extends Dialog implements ActionListener {
  Frame itsParent;
  Button okButton;
  Button cancelButton;
  TextField value;
  ErmesObjFloat itsFloatObject = null;
  String itsValue = "";
  
  public ErmesObjFloatDialog(Frame theFrame) 
  {
    super(theFrame, "Float setting", false);
    
    itsParent = theFrame;
    setLayout(new BorderLayout());
    
    //Create north section.
    Panel p1 = new Panel();
    p1.setLayout(new FlowLayout(FlowLayout.LEFT));
    
    p1.add(new Label("Float Value"));
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
  }

  public void actionPerformed(ActionEvent e)
  {
    float aFloat = 0;

    if (e.getSource() == cancelButton) 
      {
	setVisible(false);
      }
    else  
      {
	itsValue = value.getText();
	try
	  {
	    aFloat = (Float.valueOf(itsValue)).floatValue();
	  }
	catch (NumberFormatException e1)
	  {
	    setVisible(false);
	    return;
	  }

	itsFloatObject.FromDialogValueChanged(aFloat);
	setVisible(false);
      }
  }
    
  public void ReInit( String theValue, ErmesObjFloat theFloat, Frame theParent)
  {
    itsValue = theValue;
    value.setText( theValue);
    itsFloatObject = theFloat;
    itsParent = theParent;
    setVisible( true);
    value.requestFocus();
  }
}
