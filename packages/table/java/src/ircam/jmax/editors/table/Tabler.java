
package ircam.jmax.editors.table;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.toolkit.*;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.undo.*;

/** 
 * The table editor
 */
public class Tabler extends MaxEditor {
  
  /** Constructor */
  public Tabler(FtsIntegerVector theData) {
    super();

    Init();

    if (theData.getName() != null)
      setTitle("Table " + theData.getName());
    else
      setTitle("Table");

    itsData = new TableRemoteData(theData);

    itsPanel = new TablePanel(itsData);
    getContentPane().add(itsPanel);

    toolbar = itsPanel.prepareToolbar();

    //--
    validate();
    pack();
    setVisible(true);
  }

  /** 
   * Personalize the menubar */
  public void SetupMenu(){
    getEditMenu().add(new MenuItem("-"));
    itsRefreshMenuItem = new MenuItem("Refresh");
    getEditMenu().add(itsRefreshMenuItem);
    itsRefreshMenuItem.addActionListener(new ActionListener() {
      public  void actionPerformed(ActionEvent e)
	{ 
	  itsData.forceUpdate();
	  itsPanel.repaint();
	}});
    
    Menu ViewMenu = new Menu("View");
    
    MenuItem hollow = new MenuItem("Hollow");
    hollow.addActionListener(new ActionListener() 
			       {
				 public void actionPerformed(ActionEvent e) 
				   {
				     itsPanel.hollow();
				   }
			       }
			       );


    MenuItem solid = new MenuItem("Solid");
    solid.addActionListener(new ActionListener() 
			     {
			       public void actionPerformed(ActionEvent e) 
				 {
				   itsPanel.solid();
				 }
			     }
			     );
    
    ViewMenu.add(hollow);
    ViewMenu.add(solid);
    getMenuBar().add(ViewMenu);
  }

  /** NB: these functionalities can be moved in a "MaxEditor with undo"*/
  protected void Undo()
  {
    try 
      {
	itsData.undo();
	itsPanel.repaint(); //should not repaint from here
      } catch (CannotUndoException e1) {
	System.out.println("can't undo");
	
      }
  }

  /** NB: these functionalities can be moved in a "MaxEditor with undo"*/
  protected void Redo()
  {
    try 
      {
	itsData.redo();
	itsPanel.repaint(); //should not repaint from here
      } catch (CannotRedoException e1) {
	System.out.println("can't redo");
      }
  }

  //--- Fields 
  TableRemoteData itsData;
  TablePanel itsPanel;
  static EditorToolbar toolbar; 

  MenuItem itsRefreshMenuItem;//?? keep it?
}







