package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import ircam.jmax.utils.*;

/**
 * A dialog used to edit the value inside a "float box".
 */
class ErmesObjIntegerDialog extends Dialog implements KeyListener, ActionListener, WindowListener{
  Frame itsParent;
  Button okButton;
  Button cancelButton;
  TextField value;
  ErmesObjInt itsIntObject = null;
  String itsValue = "";
  
  
  public ErmesObjIntegerDialog(Frame theFrame) {
    super(theFrame, "Integer setting", true);
    
    itsParent = theFrame;
    setLayout(new BorderLayout());
    
    //Create north section.
    Panel p1 = new Panel();
    p1.setLayout(new FlowLayout(FlowLayout.LEFT));
    
    p1.add(new Label("Integer Value"));
    value = new TextField("", 20);
    value.addActionListener(this);
    value.addKeyListener(this);
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
    addWindowListener(this);
  }

   ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// actionListener --inizio

  public void actionPerformed(ActionEvent e){        
    Integer  aInt = null;
    if (e.getSource() == okButton) {
      itsValue = value.getText();
      try{
	aInt = new Integer(itsValue);
      }
      catch (NumberFormatException e1){
	setVisible(false);
	return;
      }
      itsIntObject.FromDialogValueChanged(aInt);
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
    
  public void ReInit(String theValue, ErmesObjInt theInt, Frame theParent){
    itsValue = theValue;
    value.setText(theValue);
    itsIntObject = theInt;
    itsParent = theParent;
  }
 
  /////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////keyListener --inizio
  
  public void keyTyped(KeyEvent e){}
  public void keyReleased(KeyEvent e){}

  public void keyPressed(KeyEvent e){
    Integer aInt = null;
    if (e.getKeyCode() == ircam.jmax.utils.Platform.RETURN_KEY){	
      itsValue = value.getText();
      try{
	aInt = new Integer(itsValue);
      }
      catch (NumberFormatException e1){
	setVisible(false);
	return;
      }
      itsIntObject.FromDialogValueChanged(aInt);
      setVisible(false);
    }
  }
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// keyListener --fine
  public void windowClosing(WindowEvent e){}
  public void windowOpened(WindowEvent e){}
  public void windowClosed(WindowEvent e){}
  public void windowIconified(WindowEvent e){}       
  public void windowDeiconified(WindowEvent e){}
  public void windowActivated(WindowEvent e)
  {
    requestFocus();
    value.selectAll();
  }
  public void windowDeactivated(WindowEvent e){}  
}






