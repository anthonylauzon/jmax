
package ircam.jmax.editors.ermes;

import java.awt.datatransfer.*;
import java.text.*;
import java.io.*;
import ircam.jmax.editors.ermes.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.utils.*;
import java.util.*;
/**
 * The class that handles the data put in the clipboard, for patchers.
 * jMax has a systemClipboard (but user and/or applications can create local ones).
 * ClipboardProvider will Provide the asked version of the stored data. This class provides datas in three flavors: plain text, ftsGroupFlavor (vector of ftsObjects) and tclGroupFlavor (tcl scripts that create a vector of NEW FtsObject).
 * For the tclGroupFlavour, the new data structure is used to save the objects under the form of a jmax group tcl command. 
 * 
 */


public class ErmesClipboardProvider implements Transferable, ClipboardOwner {

  DataFlavor flavorList[] = new DataFlavor[3];
  public static DataFlavor tclGroupFlavor = new DataFlavor("text/plain; charset=unicode", "tclGroupFlavor");
  public static DataFlavor ftsGroupFlavor = new DataFlavor("unknown", "ftsGroupFlavor");
  StringWriter itsStringWriter = new StringWriter();
  Vector itsFtsGroup = new Vector();
  static String itsStoredText;

  /**
   * Simple constructor.
   * Provides tcl script format, text format, ftsObject vector format.
   */
  public ErmesClipboardProvider() {
    super();
    flavorList[0] = tclGroupFlavor;
    flavorList[1] = ftsGroupFlavor;
    flavorList[2] = DataFlavor.plainTextFlavor;
  }

  /**
   * Fill the clipboard starting from an FtsSelection
   */
  /*  void addSelection(FtsSelection theSelection) {
      if (theSelection.getObjects().size() == 0)
      return;
      
      flushContent();
      
      try 
      {
      //PrintWriter aPrintWriter = new PrintWriter(itsStringWriter);
      IndentedPrintWriter pw = new IndentedPrintWriter(itsStringWriter); 
      theSelection.saveAsTcl(pw);
      }
      catch (Exception e)
      {
      System.out.println("ERROR " + e + " while copying ");
      e.printStackTrace(); // temporary
      }   
      }*/
  
  /**
   * Fill the clipboard starting from a list of fts objects (ErmesObjects)
   */
  /*public void addFtsObjects(Vector theSelectedList) {
    // actually call, for now,  the objects' saveAsTcl method.
    flushContent();
    
    PrintWriter aPrintWriter = new PrintWriter(itsStringWriter);
    FtsObject aFtsObject;
    for(Enumeration e=theSelectedList.elements(); e.hasMoreElements();) {
      aFtsObject = (FtsObject) e.nextElement();
      aFtsObject.saveAsTcl(aPrintWriter);
      aPrintWriter.println("");
      itsFtsGroup.addElement(aFtsObject);
    }
  }*/

  /**
   * returns the list of flavors it supports
   */
  public DataFlavor[] getTransferDataFlavors() {
    return flavorList;
  }
  
  /**
   * Only tclgroupflavors (and text) supported for now
   */
  public boolean isDataFlavorSupported(DataFlavor flavor) {
    return (flavor.equals(tclGroupFlavor) ||
	    flavor.equals(DataFlavor.plainTextFlavor) ||
	    flavor.equals(ftsGroupFlavor));
  }
  
  public Object getTransferData(DataFlavor flavor) throws UnsupportedFlavorException, IOException 
  {
    return itsFtsGroup;
  }


  /**
   * From ClipboardOwner interface.
   */
  public void lostOwnership(Clipboard clipboard, Transferable contents) {
    flushContent();
  }

  /**
   * Empties the buffers.
   * It's called when the content is no more needed (someone is putting other
   * stuff, and/or we loose clipboard ownership)
   */ 
  private void flushContent() {
    itsStringWriter.close();
    itsStringWriter = new StringWriter();//we had to do so. flush seems not to work properly on StringWriters(printing again after a flush just appends)
    itsFtsGroup.removeAllElements();
  }
}
