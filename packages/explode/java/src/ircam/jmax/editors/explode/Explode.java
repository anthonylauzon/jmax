package ircam.jmax.editors.explode;

import java.io.*;
import java.lang.*;
import java.awt.event.*;
import java.awt.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.toolkit.*;

import com.sun.java.swing.*;
import com.sun.java.swing.table.*;


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

    setTitle("Explode");

    // get the data
    ExplodeRemoteData explodeData = (ExplodeRemoteData) maxData;
    
    itsPanel = new ScrPanel(explodeData);
    getContentPane().add(itsPanel);
    
    if (toolbar == null)
      toolbar = itsPanel.prepareToolbar();
    else itsPanel.linkToToolbar();
    
    //----

    validate();
    pack();
    setVisible(true);

  }


  /**
   * personalize the menubar (adds the settings menu)
   */

  public void SetupMenu()
  {
    Menu settingsMenu = new Menu("Settings");
    
    MenuItem settings = new MenuItem("Settings...");
    settings.addActionListener(new ActionListener() 
			     {
			       public void actionPerformed(ActionEvent e) 
				 {
				   itsPanel.settings();
				 }
			     }
			     );
    
    settingsMenu.add(settings);
    getMenuBar().add(settingsMenu);
  
  }

  protected void Undo()
  {
    itsPanel.undo();
  }

  protected void Redo()
  {
    itsPanel.redo();
  }


  //------------------- fields

  ScrPanel itsPanel;
  static EditorToolbar toolbar;
}

