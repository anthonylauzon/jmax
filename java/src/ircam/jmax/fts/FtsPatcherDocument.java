package ircam.jmax.fts; 

import java.io.*;

import tcl.lang.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/** A FtsPatcherDocument is the Max Document containing an FTS Patch.
 * It implement the MaxTclDocument interfaces, i.e. it can be loaded from/stored to TCL files.
 */

public class FtsPatcherDocument extends MaxDocument implements MaxTclDocument
{
  public FtsPatcherDocument()
  {
    super(Mda.getDocumentTypeByName("patcher"));
  }

  public void setRootData(MaxData patcher)
  {
    super.setRootData(patcher);
    ((FtsContainerObject) patcher).setDocument(this);
  }

  /** Save the content (a patcher) as TCL code */

  public void saveContentAsTcl(PrintWriter pw)
  {
    // Sync to fts to be sure we have all the info.

    Fts.getServer().syncToFts();

    ((FtsContainerObject) rootData).saveAsTcl(pw);
  }

  /** Eval function, to built the Patch Data from a Tcl file.
    Eval a given script inside this documeynt */

  public void eval(Interp interp, String  script) throws tcl.lang.TclException
  {
    eval (interp, TclString.newInstance(script));
  }

  /** Eval function, to built the Patch Data from a Tcl file.
    Eval a given script inside this document */

  public void eval(Interp interp, TclObject script) throws tcl.lang.TclException
  {
    MaxData patcher;

    // Call the tcl function maxTclDataEval, with this as first argument,
    // and the script as second

    TclObject list = TclList.newInstance();

    TclList.append(interp, list, TclString.newInstance("_BasicThisWrapper"));
    TclList.append(interp, list, ReflectObject.newInstance(interp, this));
    TclList.append(interp, list, script);

    interp.eval(list, 0);

    patcher = (MaxData) ReflectObject.get(interp, interp.getResult());
    setRootData(patcher);
  }


  public void dispose()
  {
    super.dispose();

    FtsContainerObject patcher;

    patcher = (FtsContainerObject) getRootData();
    patcher.close();
    patcher.delete();
  }

  /**
   * Highly experimental method for generating a binary file
   */

//   public void saveBmax(String fileName)
//   {
//     Fts.getServer().savePatcherBmax(patcher, fileName);
//   }


}






