//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
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
 * This implementation builds a ScrPanel and a ExplodeTablePanel to represent the data.
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

    // create the Selection model, that will be in common between
    // the different views

    ExplodeSelection s = new ExplodeSelection(explodeData);

    // creates the Table view
    ExplodeTableModel model = new ExplodeTableModel(explodeData);
    itsTablePanel = new ExplodeTablePanel(model, s);

    // creates the Piano roll view
    itsScrPanel = new ScrPanel(explodeData, s);

    setView(defaultView);

  }


  /**
   * personalize the menubar (adds the Options menu)
   */

  public void SetupMenu()
  {
    // NOTES: the transition to JMenu and to Actions will allow the automatical
    // deselection of items (via the JCheckboxMenuItem).
    // For now, the setView method takes care of this (I agree, it's shit)

    Menu editMenu = getEditMenu();

    Menu optionsMenu = new Menu("Options");    

    settings = new MenuItem("Settings...");
    settings.addActionListener(new ActionListener() 
			     {
			       public void actionPerformed(ActionEvent e) 
				 {
				   itsScrPanel.settings();
				 }
			     }
			     );
    
    pianoRollMenuItem = new CheckboxMenuItem("Piano roll view");
    pianoRollMenuItem.addItemListener(new ItemListener() 
			     {
			       public void itemStateChanged(ItemEvent e) 
				 {
				   setView(PIANOROLL_VIEW);
				 }
			     }
			     );

    tableMenuItem = new CheckboxMenuItem("Table view");
    tableMenuItem.addItemListener(new ItemListener() 
			     {
			       public void itemStateChanged(ItemEvent e) 
				 {
				   setView(TABLE_VIEW);
				 }
			     }
			     );

    optionsMenu.add(settings);

    optionsMenu.add("-");

    optionsMenu.add(pianoRollMenuItem);
    optionsMenu.add(tableMenuItem);


    getMenuBar().add(optionsMenu);

  }


  protected void Copy()
  {
    ((ClipableData) explodeData).copy();
  }

  protected void Cut()
  {
    ((ClipableData) explodeData).cut();
  }

  protected void Paste()
  {
    ((ClipableData) explodeData).paste();
  }

  protected void Duplicate()
  {
    Copy();
    Paste();
  }

  protected void Undo()
  {
    try 
      {
	((UndoableData) explodeData).undo();
      } catch (CannotUndoException e1) {
	System.out.println("can't undo");
      }
  }

  protected void Redo()
  {
    try 
      {
	((UndoableData) explodeData).redo();
      } catch (CannotRedoException e1) {
	System.out.println("can't redo");
      }
  }

  /**
   * Sets the default representation to show the Explode content */
  public void setDefaultView(int view)
  {
    defaultView = view;
  }

  /**
   * Gets the default representation */
  public int getDefaultView()
  {
    return defaultView;
  }

  /** 
   * Set the kind of panel corresponding to the given view (PIANOROLL, TABLE).
   * Removes also the old panel from the container, if needed */
  public void setView(int view)
  {

    if (itsView != null)
      {
	setVisible(false);
	getContentPane().remove(itsView);
      }


    if (view == PIANOROLL_VIEW)
      {
	itsView = itsScrPanel;
	tableMenuItem.setState(false);
	settings.setEnabled(true);
      }
    else 
      {
	itsView = itsTablePanel;
	pianoRollMenuItem.setState(false);
	settings.setEnabled(false);
      }

    getContentPane().add(itsView);

    if (view == PIANOROLL_VIEW && itsScrPanel.tb == null)
      {
	itsScrPanel.prepareToolbar();
	// See also the documentation
	// in ScrPanel.prepareToolbar() for details.
      }
    
    validate();
    pack();

    setVisible(true);
  }

  //------------------- fields

  ExplodeTablePanel itsTablePanel;
  ScrPanel itsScrPanel;
  ExplodeDataModel explodeData;

  private JPanel itsView;
  public int defaultView = PIANOROLL_VIEW;
  public static final int PIANOROLL_VIEW = 0;
  public static final int TABLE_VIEW = 1;

  MenuItem settings;
  CheckboxMenuItem tableMenuItem;
  CheckboxMenuItem pianoRollMenuItem;
}

