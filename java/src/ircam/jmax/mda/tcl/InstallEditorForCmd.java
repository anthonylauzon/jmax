package ircam.jmax.mda.tcl;

import tcl.lang.*;

import ircam.jmax.mda.*;

/**
 * This command install a named tcl function as "editor factory"
 * for a named Max Data type; this function should create an editor
 * for it.
 *
 * The Command Syntax is : <p>
 *
 * <code>
 *     installEditorFor  <i>typeName function </i>
 * </code>
 */

class InstallEditorForCmd implements Command
{
  /** Method implementing the TCL command */

  public void cmdProc(Interp interp, TclObject argv[]) throws TclException
  {
    if (argv.length == 3)
      {
	String type;
	String command;

	// Retrieve the arguments

	type  = argv[1].toString();
	command = argv[2].toString();

	MaxDataType.getTypeByName(type).setDefaultEditorFactory(new MaxTclEditorFactory(command));
      }
    else
      {
	throw new TclNumArgsException(interp, 1, argv, "typeName function");
      }
  }
}










