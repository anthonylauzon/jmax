package ircam.jmax.editors.explode;

import java.io.*;
import java.lang.*;
import java.awt.event.*;
import java.awt.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import com.sun.java.swing.*;
import com.sun.java.swing.table.*;
import com.sun.java.swing.event.*;


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
    super( Mda.getDocumentTypeByName( "explodeRemoteData"));

    Init();

    setTitle(MaxWindowManager.getWindowManager().makeUniqueWindowTitle("Explode"));

    ExplodeRemoteData explodeRemoteData = (ExplodeRemoteData) maxData;

    itsPanel = new ScrPanel(explodeRemoteData);
    getContentPane().add(itsPanel);
    
    itsPanel.prepareToolbar();
    

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

  //------------------- fields

  ScrPanel itsPanel;
}  

