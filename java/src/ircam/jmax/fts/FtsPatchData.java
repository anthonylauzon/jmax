package ircam.jmax.fts; 

import java.io.*;

import tcl.lang.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/** A FtsPatchData is the Max Data instance containing an FTS Patch,
 * It have two possible loaders, MaxTclFileDataHandler and FtsDotPatFileDataHandler
 * It implement the MaxTclData interfaces, i.e. it can be loaded from/stored to TCL files.
 */

public class FtsPatchData extends MaxData implements MaxTclData
{
  FtsObject patcher;

  public FtsPatchData()
  {
    super(MaxDataType.getTypeByName("patcher"));
  }

  void setPatcher(FtsObject patcher)
  {
    this.patcher = patcher;
  }

  FtsObject getPatcher()
  {
    return patcher;
  }

  /** Get the content (a patcher) as TCL code */

  public Object getContent()
  {
    return patcher;
  }
  
  /** Save the content (a patcher) as TCL code */

  public void saveContentAsTcl(PrintWriter pw)
  {
    // Sync to fts to be sure we have all the info.

    FtsServer.getServer().syncToFts();

    patcher.saveAsTcl(pw);
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
    // Call the tcl function maxTclDataEval, with this as first argument,
    // and the script as second

    TclObject list = TclList.newInstance();

    TclList.append(interp, list, TclString.newInstance("_BasicThisWrapper"));
    TclList.append(interp, list, ReflectObject.newInstance(interp, this));
    TclList.append(interp, list, script);

    interp.eval(list, 0);

    patcher = (FtsObject) ReflectObject.get(interp, interp.getResult());
  }

  /**
   * Highly experimental method for generating a binary file
   */

  public void saveBmax(String fileName)
  {
    FtsServer.getServer().savePatcherBmax(patcher, fileName);
  }
}





