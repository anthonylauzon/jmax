package ircam.jmax.editors.ermes;
import java.awt.*;
import java.awt.event.*;

/**
 * The graphic pop-up menu used to change the number of 
 * an inlet or an outlet in a subpatcher.
 */

public class ErmesObjInOutPop extends PopupMenu {
  public ErmesObject itsOwner;
  
  void SetNewOwner(ErmesObject theObject) {
    itsOwner = theObject;
    //setSize(itsOwner.currentRect.width, itsOwner.currentRect.height * 2);
  }
		
  public static void CommunicateChoice(ErmesObject target, String choice) {
    int numberChoosen;

    try {
      numberChoosen = Integer.parseInt(choice);
    }
    catch(Exception ex) {
      numberChoosen = -1;
      //impossible (?!)
    }
    if (target instanceof ircam.jmax.editors.ermes.ErmesObjIn) {
      ((ErmesObjIn) target).ChangeInletNo(numberChoosen-1);
    }
    else if (target instanceof ircam.jmax.editors.ermes.ErmesObjOut) {
      ((ErmesObjOut) target).ChangeOutletNo(numberChoosen-1);
    }  
    //else = what???!
  }
  
  public void Redefine(int numbers){
    MenuItem aMenuItem;
    if(numbers==getItemCount()) return;
    if(numbers>getItemCount()){
      for (int i=getItemCount(); i<numbers; i++){
	aMenuItem = new MenuItem(Integer.toString(i+1));
	add(aMenuItem);
	aMenuItem.addActionListener(new ActionListener(){
	  public  void actionPerformed(ActionEvent e)
	    { 
	      MenuItem aMenuItem2 = (MenuItem) e.getSource();//always 'this'? :-)
	      CommunicateChoice(itsOwner, aMenuItem2.getLabel()); 
	    }
	});
      }
    }
    else {
      int temp = getItemCount();
      while(numbers<temp){
	remove(temp-1);
	temp--;
      }
    }
  }


  /**
   * Constructor accepting the number of in/out to show in the popup
   */
  public ErmesObjInOutPop(int numbers) {
    super("choice:");
    itsOwner = null;
    MenuItem aMenuItem;

    for (int i=0; i<numbers; i++) {
      aMenuItem = new MenuItem(Integer.toString(i+1));
      add(aMenuItem);
      
      aMenuItem.addActionListener(new ActionListener() {
	public  void actionPerformed(ActionEvent e)
	  { 
	    MenuItem aMenuItem2 = (MenuItem) e.getSource();//always 'this'? :-)
	    CommunicateChoice(itsOwner, aMenuItem2.getLabel());
	    
	  }
      });
    }
  }
  
}



