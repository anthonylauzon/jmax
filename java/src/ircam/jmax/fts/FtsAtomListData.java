package ircam.jmax.fts; 

import java.io.*;

import tcl.lang.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;

/**
 * This class implement the MaxData for an integer vector;
 * it can be loaded/saved from FTS, from a .tpa file (embedded in the
 * patch), or in a standalone tcl file.
 */

public class FtsIntegerVectorData extends MaxData implements MaxTclData, MaxFtsData
{
  FtsIntegerVector vector = new FtsIntegerVector();

  public FtsIntegerVectorData()
  {
    super(MaxDataType.getTypeByName("integerVector"));
  }

  /** Get the content (a patcher) as TCL code */

  public Object getContent()
  {
    return vector;
  }
  
  /** One shot save function */

  public void saveToFtsObject() throws MaxDataException
  {
    if (source instanceof MaxFtsDataSource)
      {
	FtsObject object = ((MaxFtsDataSource) source).getFtsLocation().getObject();

	if (object instanceof FtsIntegerVectorObject)
	  {
	    FtsIntegerVectorObject v = (FtsIntegerVectorObject) object;

	    v.saveVectorToFts();	

	    return;
	  }
      }

    throw new MaxDataException("Cannot save vector to " + source);
  }

  /** One shot load function */

  public void loadFromFtsObject() throws MaxDataException
  {
    if (source instanceof MaxFtsDataSource)
      {
	FtsObject object = ((MaxFtsDataSource) source).getFtsLocation().getObject();

	if (object instanceof FtsIntegerVectorObject)
	  {
	    FtsIntegerVectorObject v = (FtsIntegerVectorObject) object;
	
	    v.loadVectorFromFts();

	    return;
	  }
      }

    throw new MaxDataException("Cannot save vector to " + source);
  } 

  /** We overwrite the setDataSource method locally to handle the
   * binding to the FTS Object
   */

  public void setDataSource(MaxDataSource newSource)
  {
    FtsIntegerVectorObject oldObject = null;
    FtsIntegerVectorObject newObject = null;

    if (source instanceof MaxFtsDataSource)
      {
	FtsObject object = ((MaxFtsDataSource) source).getFtsLocation().getObject();

	if (object instanceof FtsIntegerVectorObject)
	  oldObject = (FtsIntegerVectorObject) object;
      }

    super.setDataSource(newSource);

    if (newSource instanceof MaxFtsDataSource)
      {
	FtsObject object = ((MaxFtsDataSource) newSource).getFtsLocation().getObject();

	if (object instanceof FtsIntegerVectorObject)
	  oldObject = (FtsIntegerVectorObject) object;
      }

    /** If the new Object is the same good old one, nothing to do */

    if (newObject == oldObject)
      return;

    /** Otherwise, if we have an old object, unbind it */
    
    if (oldObject != null)
      {
	oldObject.unbindVector(vector);
	((FtsDataObject) oldObject).setData(null);
      }

    /** And, if we have a new one, bind it to the vector */

    newObject.bindVector(vector);
    ((FtsDataObject) newObject).setData(this);
  }

  // WARNING: TCL FORMAT TO BE DEFINED, ALSO EMBEDDED TCL FORMAT

  /** Save the content (a patcher) as TCL code */

  public void saveContentAsTcl(PrintWriter pw)
  {
    // Sync to fts to be sure we have all the info.
    // (Probabily redundant).

    FtsServer.getServer().syncToFts();

    vector.saveAsTcl(pw);
  }

  /**
   * Eval function, to built the vector Data from a Tcl file.
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
    TclObject list = TclList.newInstance();

    TclList.append(interp, list, TclString.newInstance("_BasicThisWrapper"));
    TclList.append(interp, list, ReflectObject.newInstance(interp, this));
    TclList.append(interp, list, script);

    interp.eval(list, 0);

    object = ReflectObject.get(interp, interp.getResult());

    if (object instanceof FtsIntegerVector)
      vector = (FtsIntegerVector) ReflectObject.get(interp, interp.getResult());
    else
      throw new TclException(interp, "Syntax error in integerVector data");
  }
}





