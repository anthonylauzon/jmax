package ircam.jmax.editors.frobber;

import java.awt.*;
import java.awt.event.*;
import ircam.jmax.*;

//
// The graphic pop-up menu used to change the number of an inlet or an outlet in a subpatcher.
//
public class ErmesObjInOutPop extends PopupMenu
{
  public ErmesObject itsOwner;
  ErmesSketchWindow window;  

  class ErmesInOutPopActionListener implements ActionListener, MaxEditor.Disposable
  {
    MenuItem item;
    int idx;

    ErmesInOutPopActionListener(MenuItem item, int idx)
    {
      window.disposeAtDestroy(this);
      this.item = item;
      this.idx = idx;
    }

    public  void actionPerformed(ActionEvent e)
    { 
      CommunicateChoice( itsOwner, idx);
    }

    public void dispose()
    {
      item.removeActionListener(this);
    }
  };
    
  void SetNewOwner(ErmesObject theObject) 
  {
    itsOwner = theObject;
    //setSize(itsOwner.currentRect.width, itsOwner.currentRect.height * 2);
  }
		
  public static void CommunicateChoice(ErmesObject target, int numberChoosen)
  {
    if (target instanceof ircam.jmax.editors.frobber.ErmesObjIn) 
      {
	((ErmesObjIn) target).ChangeInletNo(numberChoosen);
      }
    else if (target instanceof ircam.jmax.editors.frobber.ErmesObjOut) 
      {
	((ErmesObjOut) target).ChangeOutletNo(numberChoosen);
      }  
  }
  
  public void Redefine( int numbers)
  {
    MenuItem aMenuItem;
    if ( numbers == getItemCount()) 
      return;

    if ( numbers > getItemCount())
      {
	removeAll();	
	
	for (int i = getItemCount(); i < numbers; i++)
	  {
	    aMenuItem = new MenuItem( Integer.toString( i + 1));

	    add(aMenuItem);
	    aMenuItem.addActionListener(new ErmesInOutPopActionListener(aMenuItem, i));
	  }
      }
    else 
      {
	int temp = getItemCount();
	while ( numbers < temp)
	  {
	    remove( temp-1);
	    temp--;
	  }
      }
  }


  //
  // Constructor accepting the number of in/out to show in the popup
  //
  public ErmesObjInOutPop(ErmesSketchWindow window, int numbers) 
  {
    super("choice:");

    this.window = window;

    itsOwner = null;

    for (int i=0; i<numbers; i++) 
      {
	MenuItem aMenuItem;

	aMenuItem = new MenuItem( Integer.toString( i+1 ));
	add( aMenuItem);

	aMenuItem.addActionListener(new ErmesInOutPopActionListener(aMenuItem, i));
      }
  }


  public void removeNotify()
  {
    window = null;
    itsOwner = null;
    super.removeNotify();
  }
}
