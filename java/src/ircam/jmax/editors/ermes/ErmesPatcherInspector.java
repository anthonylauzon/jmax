package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import ircam.jmax.*;
import ircam.jmax.utils.*;
import ircam.jmax.fts.*;

class ErmesPatcherInspector extends Frame implements  ActionListener{

  Frame itsParent;
  Button okButton;
  Button cancelButton;
  TextField itsInsField, itsOutsField;
  FtsContainerObject itsPatcherObject = null;
  String itsInsNum = "";
  String itsOutsNum = "";
  
  
  public ErmesPatcherInspector(FtsContainerObject thePatcher) {
    super("inspector for: "+thePatcher.getName());
    
    /*itsPatcherObject = thePatcher;

    itsParent = MaxWindowManager.getTopFrame();*/
    setLayout(new BorderLayout());
    
    //Create north section.
    Panel p1 = new Panel();
    p1.setLayout(new GridLayout(1,2));
    
    Panel p11 = new Panel();
    p11.setLayout(new GridLayout(2,1));
    p11.add(new Label("Number of inlets"));
    p11.add(new Label("Number of outlets"));

    p1.add(p11);
    
    Panel p12 = new Panel();
    p12.setLayout(new GridLayout(2,1));
    itsInsField = new TextField("", 20);
    itsInsField.addActionListener(this);
    p12.add(itsInsField);
    itsOutsField = new TextField("", 20);
    itsOutsField.addActionListener(this);
    p12.add(itsOutsField);   
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
    reInit(thePatcher);
  }

   ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// actionListener --inizio

  public void actionPerformed(ActionEvent e){        
    int aInsNum = 0;
    int aOutsNum = 0;
    
    if (e.getSource()==cancelButton) {
      setVisible(false);
    }
    else  { //OK button and the editable fields are handled the same
      itsInsNum = itsInsField.getText();
      itsOutsNum = itsOutsField.getText();
      try{
	aInsNum = Integer.parseInt(itsInsNum);
	aOutsNum = Integer.parseInt(itsOutsNum);
      }
      catch (NumberFormatException e1){
	setVisible(false);
	return;
      }
      itsPatcherObject.setNumberOfInlets(aInsNum);
      itsPatcherObject.setNumberOfOutlets(aOutsNum);

      setVisible(false);
    }
  }
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// actionListener --fine
    
  public void reInit(FtsContainerObject thePatcher){

    itsInsNum = String.valueOf(thePatcher.getNumberOfInlets());
    itsOutsNum = String.valueOf(thePatcher.getNumberOfOutlets());

    itsInsField.setText(itsInsNum);
    itsOutsField.setText(itsOutsNum);

    itsPatcherObject = thePatcher;
    itsParent = MaxWindowManager.getTopFrame();
    setTitle("inspector for: "+thePatcher.getName());
    setVisible(true);

    itsInsField.requestFocus();
  }
}










