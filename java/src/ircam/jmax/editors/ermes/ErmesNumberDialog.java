package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import ircam.jmax.utils.*;

//
// A dialog used to edit the value inside a "number box".
//
class ErmesNumberDialog extends Dialog implements ActionListener {
  private Button okButton;
  private Button cancelButton;
  private TextField valueField;

  private ErmesObjNumberBox itsObject = null;
  
  public ErmesNumberDialog( Frame theFrame, ErmesObjNumberBox theNumberBox)
  {
    super(theFrame, "Number setting", false);
    
    setLayout( new BorderLayout());
    
    //Create north section.
    Panel p1 = new Panel();
    p1.setLayout( new FlowLayout(FlowLayout.LEFT));
    p1.add( new Label( "Value"));
    valueField = new TextField( "", 20);
    valueField.addActionListener( this);
    valueField.setText( theNumberBox.getValueAsText());
    
    p1.add(valueField);
    
    add("North",p1);
    
    //Create south section.
    Panel p2 = new Panel();
    p2.setLayout(new BorderLayout());
    
    okButton = new Button("OK");
    okButton.setBackground( Color.white);
    okButton.addActionListener( this);
    p2.add("East", okButton);

    cancelButton = new Button( "Cancel");
    cancelButton.setBackground( Color.white);
    cancelButton.addActionListener( this);
    p2.add( "West", cancelButton);

    add( "South", p2);

    itsObject = theNumberBox;
    pack();

    setVisible( true);
    valueField.requestFocus();
  }

  public void actionPerformed( ActionEvent e)
  {
    if (e.getSource() == cancelButton) 
      {
	setVisible(false);
      }
    else  
      {
	itsObject.setValueAsText( valueField.getText());
	itsObject = null; // make the gc happy, later.
	setVisible(false);
      }
  }
}
