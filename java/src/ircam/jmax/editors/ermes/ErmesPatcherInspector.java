package ircam.jmax.editors.ermes;

import java.awt.*;
import java.awt.event.*;
import ircam.jmax.*;
import ircam.jmax.utils.*;
import ircam.jmax.fts.*;

class ErmesPatcherInspector extends Frame {

  TextField itsInsField, itsOutsField;
  static FtsContainerObject itsPatcherObject = null;
  private static ErmesPatcherInspector itsInspector = null;
  
  /**
   * The only function actually needed: a static call to create (or re-assign) an
   * ispector, given a patcher
   */
  public static void inspect(FtsContainerObject thePatcher) {
    if (itsInspector == null) itsInspector = new ErmesPatcherInspector(thePatcher);
    else if (getInspectedObject() == thePatcher) itsInspector.setVisible(true);
    else itsInspector.reInit(thePatcher);
  }

  public static boolean isOpen() {
    return itsInspector != null && itsInspector.isVisible();
  }

  public static FtsContainerObject getInspectedObject() {
    return itsPatcherObject;
  }

  public ErmesPatcherInspector(FtsContainerObject thePatcher) {
    super("inspector for: "+thePatcher.getName());
    
    setLayout(new BorderLayout());
    
    // The action listener to be used by the Ok button and the text fields,
    // stored in a variable!
    ActionListener aActionListener = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
	
	try{
	  itsPatcherObject.put("ins", Integer.parseInt(itsInsField.getText()));
	  itsPatcherObject.put("outs", Integer.parseInt(itsOutsField.getText()));
	  itsPatcherObject.ask("ins");
	  itsPatcherObject.ask("outs");
	  Fts.sync();
	}
	catch (NumberFormatException e1){
	  //ignore and close
	}
	
	setVisible(false);
      }
    };

    {//Create north section (labels, textfields).
      Panel p1 = new Panel();
      p1.setLayout(new GridLayout(1,2));
      
      { //the labels
	Panel p11 = new Panel();
	p11.setLayout(new GridLayout(2,1));
	p11.add(new Label("Number of inlets"));
	p11.add(new Label("Number of outlets"));
	
	p1.add(p11);
      }
      
      { //the edit fields
	Panel p12 = new Panel();
	p12.setLayout(new GridLayout(2,1));
	itsInsField = new TextField("", 20);
	itsInsField.addActionListener(aActionListener);
	p12.add(itsInsField);
	itsOutsField = new TextField("", 20);
	itsOutsField.addActionListener(aActionListener);
	p12.add(itsOutsField);   
	p1.add(p12);
      }
      
      add("North", p1);
    }

    {//Create south section (Ok-Cancel buttons).
      Panel p2 = new Panel();
      p2.setLayout(new BorderLayout());
      
      Button okButton = new Button("Ok");
      
      okButton.setBackground(Color.white);
      okButton.addActionListener(aActionListener);
      
      p2.add("East", okButton);
      
      Button cancelButton = new Button("Cancel");
      cancelButton.setBackground(Color.white);
      cancelButton.addActionListener(new ActionListener() {
	public void actionPerformed(ActionEvent e) {
	  setVisible(false);
	}
      });
      
      p2.add("West", cancelButton);
      
      add("South", p2);
    }
    //Initialize this dialog to its preferred size.
    pack();
    reInit(thePatcher);
  }


  public void reInit(FtsContainerObject thePatcher){

    itsInsField.setText(String.valueOf(thePatcher.getNumberOfInlets()));
    itsOutsField.setText(String.valueOf(thePatcher.getNumberOfOutlets()));

    itsPatcherObject = thePatcher;

    setTitle("inspector for: "+thePatcher.getName());
    setVisible(true);

    itsInsField.requestFocus();
  }
}










