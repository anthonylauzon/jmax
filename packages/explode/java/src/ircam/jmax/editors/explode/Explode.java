package ircam.jmax.editors.explode;

import java.io.*;
import java.lang.*;
import java.awt.event.*;
import java.awt.*;
import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
import com.sun.java.swing.*;
import com.sun.java.swing.table.*;
import com.sun.java.swing.event.*;


/**
 * the main class of the explode package.
 * It inherits from MaxEditor, and implements the MaxDataEditor Interface
 */
public class Explode extends MaxEditor implements MaxDataEditor, AAAReadme {

  /**
   * constructor.
   * It creates a panel that will display the datas in maxData
   */
  public Explode( MaxData maxData)
  {
    super( Mda.getDocumentTypeByName( "explodeRemoteData"));

    this.maxData = maxData;

    Init();

    setTitle( "Explode");

    ExplodeRemoteData explodeRemoteData = (ExplodeRemoteData) maxData;

    getContentPane().add(new ScrPanel(explodeRemoteData));
    
    /*
     * just to be able to exit quickly...*/
    addWindowListener((new WindowAdapter() {

      public void windowClosing(WindowEvent e) {
	System.exit(0);
      }
    }));
    

    //----

    validate();
    pack();
    setVisible(true);
  }


  /** 
   * MaxDataEditor interface
   */
  public MaxData getData()
  {
    return maxData;
  }


  /** 
   * MaxDataEditor interface
   */
  public void reEdit()
  {
    if (!isVisible()) 
      setVisible(true);

    toFront();
  }


  /** 
   * MaxDataEditor interface
   */
  public void quitEdit()
  {
    Close();
    dispose();
  }


  /** 
   * MaxDataEditor interface
   */
  public void syncData()
  {
  }


  public void showObject(Object object)
  {
  }

  /**
   * personalize the menubar
   */
  public void SetupMenu()
  {
  }

  //------------------- fields

  MaxData maxData;
}  
