package ircam.jmax.editors.ermes;
import java.awt.*;
import java.awt.event.*;

/**
 * The graphic pop-up menu used to change the number of 
 * an inlet or an outlet in a subpatcher.
 */

public class ErmesObjInOutChoice extends Choice implements ItemListener {
  public ErmesObject itsOwner;
  
  void SetNewOwner(ErmesObject theObject) {
    itsOwner = theObject;
    setSize(itsOwner.currentRect.width, itsOwner.currentRect.height * 2);
  }
		
  public ErmesObjInOutChoice(ErmesObject theOwner) {
    super();
    itsOwner = theOwner;
    addItemListener(this);
  }

  public ErmesObjInOutChoice() {
    super();
    itsOwner = null;
  }

	
  /*public boolean mouseUp(Event evt, int x, int y) {
    super.mouseUp(evt, x, y);
    return true;
    }
    public boolean mouseDown(Event evt, int x, int y) {
    super.mouseDown(evt, x, y);
    return false;
    }
    public boolean mouseDrag(Event evt, int x, int y) {
    super.mouseDrag( evt, x, y);
    return false;
    }*/
	
  //this method is called (I hope) when the user choosed a number
  
  public boolean action(Event event, Object arg) {
    System.err.println("booh!");
    int numberChoosen = getSelectedIndex();
    if (itsOwner instanceof ircam.jmax.editors.ermes.ErmesObjIn) {
    ((ErmesObjIn) itsOwner).ChangeInletNo(numberChoosen);
    }
    else if (itsOwner instanceof ircam.jmax.editors.ermes.ErmesObjOut) {
    ((ErmesObjOut) itsOwner).ChangeOutletNo(numberChoosen);
    }
    move(-200, -200);
    hide();
    return true;
    }

   ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// itemListener --inizio
  public void itemStateChanged(ItemEvent e){
    int numberChoosen = getSelectedIndex();
    
    System.err.println("yhoo-hoo! Era un "+itsOwner.getClass().getName() );
    if(itsOwner instanceof ircam.jmax.editors.ermes.ErmesObjIn) {
      ((ErmesObjIn) itsOwner).ChangeInletNo(numberChoosen);
    }
    else if (itsOwner instanceof ircam.jmax.editors.ermes.ErmesObjOut) {
      ((ErmesObjOut) itsOwner).ChangeOutletNo(numberChoosen);
    }
    setLocation(-200, -200);
    setVisible(false);
  }
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// itemListener --fine
}




