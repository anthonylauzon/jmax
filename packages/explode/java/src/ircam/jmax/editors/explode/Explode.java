package ircam.jmax.editors.explode;

import java.io.*;
import java.lang.*;
import java.awt.event.*;
import java.awt.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.toolkit.*;

import javax.swing.*;
import javax.swing.table.*;
import javax.swing.undo.*;


/**
 * the main class of the explode package.
 * It inherits from MaxEditor, and implements the MaxDataEditor Interface.
 * This implementation builds a ScrPanel to represent the score.
 */
public class Explode extends MaxEditor implements AAAReadme {

  /**
   * constructor.
   * It creates a panel that will display the datas in maxData
   */
  public Explode(ExplodeRemoteData maxData)
  {
    super();

    Init();

    setTitle("Explode " + maxData.getName());

    // get the data
    explodeData = (ExplodeRemoteData) maxData;
    
    itsPanel = new ScrPanel(explodeData);
    getContentPane().add(itsPanel);
    
    toolbar = itsPanel.prepareToolbar();
    
    
    //----

    validate();
    pack();
    setVisible(true);


  }


  /**
   * personalize the menubar (adds the Options menu)
   */

  public void SetupMenu()
  {
    Menu editMenu = getEditMenu();

    editMenu.remove(getCopyMenu());
    editMenu.remove(getPasteMenu());
    editMenu.remove(getCutMenu());
    editMenu.remove(getDuplicateMenu());

    Menu optionsMenu = new Menu("Options");    

    MenuItem settings = new MenuItem("Settings...");
    settings.addActionListener(new ActionListener() 
			     {
			       public void actionPerformed(ActionEvent e) 
				 {
				   itsPanel.settings();
				 }
			     }
			     );
    
    MenuItem pianoRollView = new MenuItem("Piano roll view");
    settings.addActionListener(new ActionListener() 
			     {
			       public void actionPerformed(ActionEvent e) 
				 {
				 }
			     }
			     );

    MenuItem tableView = new MenuItem("Table view");
    settings.addActionListener(new ActionListener() 
			     {
			       public void actionPerformed(ActionEvent e) 
				 {
				 }
			     }
			     );

    optionsMenu.add(settings);
    optionsMenu.add(pianoRollView);
    optionsMenu.add(tableView);
    getMenuBar().add(optionsMenu);

  }

  protected void Undo()
  {
    try 
      {
	explodeData.undo();
      } catch (CannotUndoException e1) {
	System.out.println("can't undo");
      }
  }

  protected void Redo()
  {
    try 
      {
	explodeData.redo();
      } catch (CannotRedoException e1) {
	System.out.println("can't redo");
      }
  }


  //------------------- fields

  ScrPanel itsPanel;
  ExplodeDataModel explodeData;
  static EditorToolbar toolbar;
}

