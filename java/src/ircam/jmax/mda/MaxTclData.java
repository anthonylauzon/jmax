package ircam.jmax.mda;

import tcl.lang.*;
import java.io.*;

/** This class is a specialization of MaxData
 *  That define the infrastructure for supporting 
 *  the jmax tcl document format, in reading and writing
 *  but, it is an abstract class, no real implementation.
 */


public abstract class  MaxTclData extends MaxData implements MaxTclInterpreter
{
  /** A constructor that get only the type */

  public MaxTclData(MaxDataType type)
  {
    super(type);
  }

  /** A constructor that get the type and the instance name */

  public MaxTclData(MaxDataType type, String name)
  {
    super(type, name);
  }

  /** Ask the content to save itself as tcl code to
   * the given printwriter
   */

  abstract public void saveContentAsTcl(PrintWriter pw);

  /** Eval a given script inside this documeynt */

  public void eval(Interp interp, String  script) throws tcl.lang.TclException
  {
    eval (interp, TclString.newInstance(script));
  }

  /** Eval a given script inside this document */

  public void eval(Interp interp, TclObject script) throws tcl.lang.TclException
  {
    // Call the tcl function maxTclDataEval, with this as first argument,
    // and the script as second

    TclObject list = TclList.newInstance();

    TclList.append(interp, list, TclString.newInstance("_BasicThisWrapper"));
    TclList.append(interp, list, ReflectObject.newInstance(interp, this.tclTarget()));
    TclList.append(interp, list, script);

    interp.eval(list, 0);

    setContent(ReflectObject.get(interp, interp.getResult()));
  }

  /** This method provide the subclasses of MaxTclData a way to specify
      what to bind to the this Tcl variable
      */

  protected Object tclTarget()
  {
    return this;
  }
}

