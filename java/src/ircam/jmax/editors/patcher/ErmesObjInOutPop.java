package ircam.jmax.editors.patcher;

import java.awt.*;
import java.awt.event.*;

import ircam.jmax.*;
import ircam.jmax.editors.patcher.objects.*;

//
// The graphic pop-up menu used to change the number of an inlet or an outlet in a subpatcher.
//
public class ErmesObjInOutPop extends PopupMenu
{
  ErmesObjInOut itsOwner;
  ErmesSketchWindow window;  

  class ErmesInOutPopActionListener implements ActionListener
  {
    int idx;

    ErmesInOutPopActionListener(int idx)
    {
      this.idx = idx;
    }

    public void actionPerformed( ActionEvent e)
    { 
      itsOwner.ChangeNo( idx);
    }
  };
    
  public void SetNewOwner( ErmesObjInOut object) 
  {
    itsOwner = object;
    //setSize(itsOwner.currentRect.width, itsOwner.currentRect.height * 2);
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
	    aMenuItem = new MenuItem( Integer.toString( i));

	    add(aMenuItem);
	    aMenuItem.addActionListener(new ErmesInOutPopActionListener(i));
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
    super( "Change:");

    this.window = window;

    itsOwner = null;

    for (int i = 0; i < numbers; i++) 
      {
	MenuItem aMenuItem;

	aMenuItem = new MenuItem( Integer.toString( i ));
	add( aMenuItem);

	aMenuItem.addActionListener( new ErmesInOutPopActionListener(i));
      }
  }

  public void removeNotify()
  {
    window = null;
    itsOwner = null;
    super.removeNotify();
  }
}
