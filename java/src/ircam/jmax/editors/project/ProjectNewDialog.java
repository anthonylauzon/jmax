package ircam.jmax.editors.project;

import java.awt.*;
import java.awt.event.*;//???????
import ircam.jmax.*;
import ircam.jmax.utils.*;

/**
 * The dialog used during a menu-new operation.
 */
class ProjectNewDialog extends Dialog implements ItemListener,  ActionListener{
  Frame itsParent;
  Button okButton;
  Button cancelButton;
  List itsList;
  
  String itsCurrentItem = "";
  
  
  public ProjectNewDialog(Frame theFrame) {
    super(theFrame, "New File", true);
    
    itsParent = theFrame;
    
    setLayout(new BorderLayout());
		
    //Create north section.
    Panel p1 = new Panel();
    p1.setLayout(new BorderLayout());
    
    p1.add("North", new Label("New File Type"));
    itsList = new List(5, false);
    MaxResourceId aResId;
    String aString;
    for(int i=0; i< MaxApplication.getApplication().resourceVector.size();i++){
      aResId = (MaxResourceId)MaxApplication.getApplication().resourceVector.elementAt(i);
      aString = aResId.GetName();
      itsList.add(aString);
    }
    itsList.addItemListener(this);
    itsList.addActionListener(this);
    
    p1.add("South", itsList);
    
    add("North",p1);
    
    //Create south section.
    Panel p2 = new Panel();
    p2.setLayout(new BorderLayout());
    
    okButton = new Button("OK");
    okButton.setBackground(Color.white);
    p2.add("East", okButton);
    cancelButton = new Button("Cancel");
    cancelButton.setBackground(Color.white);
    p2.add("West", cancelButton);
    
    add("South", p2);
    //Initialize this dialog to its preferred size.
    pack();
  }
  
  public void itemStateChanged(ItemEvent e){
    if(e.getStateChange() == ItemEvent.SELECTED) itsCurrentItem = (itsList.getSelectedItems())[0];
  }
  
  public void actionPerformed(ActionEvent e){
    hide();
  }

  public boolean action(Event event, Object arg) {
    if ( event.target == okButton) {
      //	Ok action
      hide();
    }
    else if ( event.target == cancelButton) {
      //	Cancel action
      itsCurrentItem = "";
      hide();
    }
    return true;
  }

  public String GetNewFileType(){
    return itsCurrentItem;
  }
    
  public boolean keyDown(Event evt,int key) {
    Float aFloat = null;
    if (key == ircam.jmax.utils.Platform.RETURN_KEY){
      hide();
      return true;
    }
    return false;
  }
}

