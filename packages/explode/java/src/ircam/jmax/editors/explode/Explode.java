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

public class Explode extends MaxEditor implements MaxDataEditor, AAAReadme {

  public Explode( MaxData maxData)
  {
    super( Mda.getDocumentTypeByName( "explodeRemoteData"));

    this.maxData = maxData;

    Init();

    setTitle( "Explode");

    ExplodeRemoteData explodeRemoteData = (ExplodeRemoteData) maxData;


    /* JDK1.1 version */
    /*JScrollPane aScrollPane = new JScrollPane();
      aScrollPane.add(new ScrPanel(explodeRemoteData));
      aScrollPane.setSize(200, 200);*/
    /* swing version (too slow!!) 
       JScrollPane aScrollPane = new JScrollPane();
       aScrollPane.getViewport().add(new ScrPanel(explodeRemoteData)); */
    
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
