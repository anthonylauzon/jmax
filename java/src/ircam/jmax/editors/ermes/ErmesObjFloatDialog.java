package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import ircam.jmax.utils.*;

//
// A dialog used to edit the value inside a "float box".
//
class ErmesObjFloatDialog extends Dialog implements ActionListener {
  Button okButton;
  Button cancelButton;
  TextField value;
  ErmesObjFloat itsObject = null;
  
  public ErmesObjFloatDialog(Frame theFrame,  String theValue, ErmesObjFloat theFloat)
  {
    super(theFrame, "Float setting", false);
    
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

    value.setText( theValue);
    itsObject = theFloat;
    pack();
    setVisible( true);
    value.requestFocus();
  }

  public void actionPerformed(ActionEvent e)
  {
    if (e.getSource() == cancelButton) 
      {
	setVisible(false);
      }
    else  
      {
	float aFloat = 0;

	try
	  {
	    aFloat = (Float.valueOf(value.getText())).floatValue();
	  }
	catch (NumberFormatException e1)
	  {
	    setVisible(false);
	    return;
	  }

	itsObject.FromDialogValueChanged(aFloat);
	itsObject = null; // make the gc happy, later.
	setVisible(false);
      }
  }
    
}
