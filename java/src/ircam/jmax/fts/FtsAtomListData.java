package ircam.jmax.fts; 

import java.io.*;

import tcl.lang.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/**
 * This class implement the MaxData for an integer vector;
 * NOTE: since most of the code here is in common with IntegerVector,
 * Why we don't do a common API and enough superclasses ?
 * it can be loaded/saved from FTS, from a .tpa file (embedded in the
 * patch), or in a standalone tcl file.
 */

public class FtsAtomListData extends MaxData implements MaxTclData, MaxFtsData
{
  FtsAtomList list = new FtsAtomList();

  public FtsAtomListData()
  {
    super(MaxDataType.getTypeByName("atomList"));
  }

  /** Get the content (a patcher) as TCL code */

  public Object getContent()
  {
    return list;
  }
  
  /** One shot save function */

  public void saveToFtsObject() throws MaxDataException
  {
    if (source instanceof MaxFtsDataSource)
      {
	FtsObject object = ((MaxFtsDataSource) source).getFtsLocation().getObject();

	if (object instanceof FtsAtomListObject)
	  {
	    FtsAtomListObject v = (FtsAtomListObject) object;

	    v.saveAtomListToFts();	

	    return;
	  }
      }

    throw new MaxDataException("Cannot save list to " + source);
  }

  /** One shot load function */

  public void loadFromFtsObject() throws MaxDataException
  {
    if (source instanceof MaxFtsDataSource)
      {
	FtsObject object = ((MaxFtsDataSource) source).getFtsLocation().getObject();

	if (object instanceof FtsAtomListObject)
	  {
	    FtsAtomListObject v = (FtsAtomListObject) object;
	
	    v.loadAtomListFromFts();

	    return;
	  }
      }

    throw new MaxDataException("Cannot save list to " + source);
  } 

  /** We overwrite the setDataSource method locally to handle the
   * binding to the FTS Object
   */

  public void setDataSource(MaxDataSource newSource)
  {
    FtsAtomListObject oldObject = null;
    FtsAtomListObject newObject = null;

    if (source instanceof MaxFtsDataSource)
      {
	FtsObject object = ((MaxFtsDataSource) source).getFtsLocation().getObject();

	if (object instanceof FtsAtomListObject)
	  oldObject = (FtsAtomListObject) object;
      }

    super.setDataSource(newSource);

    if (newSource instanceof MaxFtsDataSource)
      {
	FtsObject object = ((MaxFtsDataSource) newSource).getFtsLocation().getObject();

	if (object instanceof FtsAtomListObject)
	  oldObject = (FtsAtomListObject) object;
      }

    /** If the new Object is the same good old one, nothing to do */

    if (newObject == oldObject)
      return;

    /** Otherwise, if we have an old object, unbind it */
    
    if (oldObject != null)
      {
	oldObject.unbindList(list);
	((FtsDataObject) oldObject).setData(null);
      }

    /** And, if we have a new one, bind it to the list */

    newObject.bindList(list);
    ((FtsDataObject) newObject).setData(this);
  }

  // WARNING: TCL FORMAT TO BE DEFINED, ALSO EMBEDDED TCL FORMAT

  /** Save the content (a patcher) as TCL code */

  public void saveContentAsTcl(PrintWriter pw)
  {
    // Sync to fts to be sure we have all the info.
    // (Probabily redundant).

    FtsServer.getServer().syncToFts();

    list.saveAsTcl(pw);
  }

  /**
   * Eval function, to built the list Data from a Tcl file.
   * Eval a given script inside this documeynt
   */

  public void eval(Interp interp, String  script) throws tcl.lang.TclException
  {
    eval (interp, TclString.newInstance(script));
  }

  /**
   * Eval function, to built the Patch Data from a Tcl file.
   * Eval a given script inside this document
   */

  public void eval(Interp interp, TclObject script) throws tcl.lang.TclException
  {
    Object object;
    TclObject tclList = TclList.newInstance();

    TclList.append(interp, tclList, TclString.newInstance("_BasicThisWrapper"));
    TclList.append(interp, tclList, ReflectObject.newInstance(interp, this));
    TclList.append(interp, tclList, script);

    interp.eval(tclList, 0);

    object = ReflectObject.get(interp, interp.getResult());

    if (object instanceof FtsAtomList)
      list = (FtsAtomList) ReflectObject.get(interp, interp.getResult());
    else
      throw new TclException(interp, "Syntax error in AtomList data");
  }
}





