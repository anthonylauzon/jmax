package ircam.jmax.editors.console;

import ircam.jmax.*;
import ircam.jmax.dialogs.*;
import java.io.*;
import java.util.*;
/**
  Window containing a tcl console
  */

import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;
import tcl.lang.*;
 
public class ConsoleWindow extends MaxEditor implements ClipboardOwner, Transferable{
  StringBuffer itsSbuf = new StringBuffer();
  Console itsConsole;
  ConsoleDocument itsConsoleDocument = new ConsoleDocument(this);
  String itsCopiedText;

  public ConsoleWindow(Console theConsole, String theTitle) {
    super(theTitle);
    itsConsole = theConsole;
    GridBagLayout gridbag = new GridBagLayout();
    GridBagConstraints c = new GridBagConstraints();
    getContentPane().setLayout(gridbag);
    
    c.weightx = 1.0;
    c.weighty = 1.0;
    c.gridwidth = 1;
    c.gridheight = 1;
    c.fill = GridBagConstraints.BOTH;
    c.anchor = GridBagConstraints.NORTHWEST;
    gridbag.setConstraints(theConsole, c);
    getContentPane().add(itsConsole);
    itsConsole.SetContainer(this);
  }
  

  public MaxDocument GetDocument(){
    return itsConsoleDocument;
  }
  
  public void SetupMenu(){
    GetCloseMenu().setEnabled(false);
    GetSaveMenu().setEnabled(false);
    GetSaveAsMenu().setEnabled(false);
    GetPrintMenu().setEnabled(true);
    GetCutMenu().setEnabled(false);
    GetCopyMenu().setEnabled(true);//clipboard test
    GetPasteMenu().setEnabled(true);
    GetClearMenu().setEnabled(false);
  }

  // start of ClipboardOwner, Tranferable interface methods
  public void lostOwnership(Clipboard clipboard, Transferable contents) {
    //nun me ne po' frega' de meno
  }
  //--
  public Object getTransferData(DataFlavor flavor) throws UnsupportedFlavorException, IOException {
    return itsCopiedText;
  }

  public DataFlavor[] getTransferDataFlavors() {
    DataFlavor flavorList[] = new DataFlavor[1];
    flavorList[0] = DataFlavor.plainTextFlavor;
    //    System.err.println("quali flavor supporto? io dico plainText "+DataFlavor.plainTextFlavor.toString());
    return (flavorList);
  }

  public boolean isDataFlavorSupported(DataFlavor flavor) {
    //System.err.println("is supported "+flavor.toString()+"? io dico si");
    return true;
  }
  //end

  protected boolean Copy() {
    MaxApplication.systemClipboard.setContents(this, this);
    itsCopiedText = itsConsole.itsTextArea.getSelectedText();//tout simplement
    return true;
  }

  protected boolean Paste(){
    String aPastingString = new String();
    Transferable aTransferable = MaxApplication.systemClipboard.getContents(this);

    try {
      aPastingString = (String) aTransferable.getTransferData(DataFlavor.plainTextFlavor);
    } catch (Exception e) {
    }

    itsConsole.PutInKeyboardBuffer(aPastingString);

    return true;
  }

  //redefini parce-que la console ne doit pas se fermer sans quitter
  public boolean Close(){
    return false;
  }

  public void Print(){
    PrintJob aPrintjob = getToolkit().getPrintJob(this, "Printing Console", MaxApplication.jmaxProperties);
    if(aPrintjob != null){
      Graphics aPrintGraphics = aPrintjob.getGraphics();
      if(aPrintGraphics != null){
	//aPrintGraphics.setClip(0, 0, 400, 400);
	printAll(aPrintGraphics);
	aPrintGraphics.dispose();
      }
      aPrintjob.end();
    }
  }

  public boolean CustomMenuActionPerformed(MenuItem theMenuItem, String theString){
    return true;
  }

  public boolean CustomMenuItemStateChanged(CheckboxMenuItem theMenuItem, String theString){
    return true;
  }
}






