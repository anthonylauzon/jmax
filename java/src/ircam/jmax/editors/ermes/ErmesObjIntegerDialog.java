package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import ircam.jmax.utils.*;

//
// A dialog used to edit the value inside a "float box".
// Not static any more, to solve the gc and the 
// parent frame problem; the alternative is to store
// it in the sketchpad, and have one for patcher

class ErmesObjIntegerDialog extends Dialog implements ActionListener
{
  Button okButton;
  Button cancelButton;
  TextField value;
  ErmesObjInt itsIntObject = null;

  public ErmesObjIntegerDialog(Frame theFrame, String theValue, ErmesObjInt theInt)
  {
    super(theFrame, "Integer setting", false);
    
    setLayout(new BorderLayout()); 
    
    //Create north section.
    Panel p1 = new Panel();
    p1.setLayout(new FlowLayout(FlowLayout.LEFT));
    
    p1.add(new Label("Integer Value"));
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

    //Initialize this dialog 

    Point aPoint = theFrame.getLocation();

    setLocation(aPoint.x + itsIntObject.getItsX(),
		aPoint.y + itsIntObject.getItsY());

    value.setText( theValue);
    itsIntObject = theInt;
    pack();
    setVisible( true);
    value.requestFocus();
  }

  public void actionPerformed(ActionEvent e)
  {
    int  aInt = 0;
    
    if ( e.getSource() == cancelButton) 
      {
	setVisible(false);
      }
    else  
      {
	try
	  {
	    aInt = Integer.parseInt(value.getText());
	  }
	catch ( NumberFormatException e1)
	  {
	    setVisible(false);
	    // ??? SHOULD GIVE AN ERROR !!!
	    return;
	  }

	itsIntObject.FromDialogValueChanged( aInt);
	itsIntObject = null; // make the gc happy, later
	setVisible(false);
      }
  }
}
