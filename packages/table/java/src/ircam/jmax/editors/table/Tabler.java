
package ircam.jmax.editors.table;
import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.AWTEvent.*;
import java.io.*;
//import tcl.lang.*;

/** 
 A simple table editor
 */

public class Tabler extends MaxEditor implements MaxDataEditor {
  
  TablePanel itsTablePanel;
  TextField itsFormula;
  Label itsCoordinates;
  public FtsIntegerVectorData itsData;
  static int untitledCounter = 1;
  
  public Tabler(MaxData theData) {
    super(MaxDataType.getTypeByName("Table"));

    if (theData.getName()==null) setTitle(GetNewUntitledName());
    else {
      setTitle(theData.getDataSource().toString()); 
    }
    itsData = (FtsIntegerVectorData) theData;
    getContentPane().setLayout(new BorderLayout());
    
    itsFormula = new TextField("", 40);
    itsFormula.resize(300, 20);
    getContentPane().add("South", itsFormula);
    
    itsTablePanel = new TablePanel(this);
    itsTablePanel.resize(itsTablePanel.getPreferredSize().width,itsTablePanel.getPreferredSize().height);
    getContentPane().add("Center", itsTablePanel);
    
    itsCoordinates = new Label();
    itsCoordinates.resize(100,20);
    itsCoordinates.setBackground(Color.lightGray);
    getContentPane().add("North", itsCoordinates);
    
    Init();

    fillTable();
    itsTablePanel.repaint();

    validate();
    itsFormula.addKeyListener(this);
    setBounds(100, 100, getPreferredSize().width,getPreferredSize().height);
    setVisible(true);
  }

 
  void fillTable() {
    //fill the internal vector with the data contained in itsData
    FtsIntegerVector aIntV = ((FtsIntegerVector)(itsData.getContent()));
    itsTablePanel.initValues(aIntV.getValues(),aIntV.getSize());
  }

  public Tabler() {
    super();
  }

   // the MaxDataEditor interface

  public void quitEdit() {}
  /** Tell the editor to syncronize, i.e. to store in the
   * data all the information possibly cached in the editor
   * and still not passed to the data instance; this usually
   * happen before saving an instance.
   */
  public void syncData(){}
  /** Tell the editor the data has changed; it pass a sigle Java
   * Object that may code what is changed and where; if the argument
   * is null, means usually that all the data is changed
   */
  public void dataChanged(Object reason){}

  //end of the MaxDataEditor interface


  public static String GetNewUntitledName() {
    return "untitled"+(untitledCounter++);
  }

  public void SetupMenu(){}

  public void setCoordinates(int x, int y){
    itsCoordinates.setText("x: "+x+"\t\t y: "+y);
  }
  
  /////////////////////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////keyListener --inizio  
  public void keyTyped(KeyEvent e){}
  public void keyReleased(KeyEvent e){}
  // Modified to use inheritance and call the MaxEditor method
  // for all the standard key bindings
  public void keyPressed(KeyEvent e){  
    int key = e.getKeyCode();
    if(key == ircam.jmax.utils.Platform.RETURN_KEY || key == ircam.jmax.utils.Platform.ENTER_KEY){
      itsTablePanel.ApplyFormula(itsFormula.getText());
    }
    else super.keyPressed(e);
  }
  ////////////////////////////////////////////////////////////////////////////
  ///////////////////////////////////////////////////////////////// keyListener --fine

  public Dimension preferredSize() {
    return new Dimension(310,412);
  }

  public Dimension minimumSize() {
    return preferredSize();
  }
	
}







