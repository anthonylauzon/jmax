package ircam.jmax.editors.explode;

import java.io.*;
import java.lang.*;
import java.awt.event.*;
import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
import com.sun.java.swing.*;
import com.sun.java.swing.table.*;
import com.sun.java.swing.event.*;

public class Explode extends MaxEditor implements MaxDataEditor, AAAReadme {

  public Explode( MaxData maxData)
  {
    super( Mda.getDocumentTypeByName( "explodeRemoteData"));

    this.maxData = maxData;

    Init();

    setTitle( "Explode");

    ExplodeRemoteData explodeRemoteData = (ExplodeRemoteData) maxData;

    
    getContentPane().add(new ScrPanel(explodeRemoteData));
    
    /**
     * just to be able to exit...*/
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


  // MaxDataEditor interface
  public MaxData getData()
  {
    return maxData;
  }

  // MaxDataEditor interface
  public void reEdit()
  {
    if (!isVisible()) 
      setVisible(true);

    toFront();
  }

  // MaxDataEditor interface
  public void quitEdit()
  {
    Close();
    dispose();
  }

  // MaxDataEditor interface
  public void syncData()
  {
  }

  // MaxEditor
  public void SetupMenu()
  {
  }

  MaxData maxData;
}  
