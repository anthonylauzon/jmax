package ircam.jmax.mda;

import tcl.lang.*;
import ircam.jmax.*;

/** an editor factory for editors that we can run from tcl (with a tcl command
 * that take the argument.
 * The corresponding tcl command in mda.tcl "installEditorFor"
 * create one of this and install it in a named type.
 */


public class MaxTclEditorFactory implements MaxDataEditorFactory
{
  String command;

  public MaxTclEditorFactory(String command)
  {
    this.command = command;
  }

  public MaxDataEditor newEditor(MaxData data)
  {
    try
      {
	Interp interp  = MaxApplication.getTclInterp();

	// Call the tcl function, with the data as
	// first argument

	TclObject list = TclList.newInstance();

	TclList.append(interp, list, TclString.newInstance(command));
	TclList.append(interp, list, ReflectObject.newInstance(interp, this));

	interp.eval(list, 0);

	return (MaxDataEditor) ReflectObject.get(interp, interp.getResult());
      }
    catch (tcl.lang.TclException e)
      {
	// Should do something better ???
	return null;
      }
  }
}


