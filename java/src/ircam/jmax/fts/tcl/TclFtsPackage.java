package ircam.jmax.fts.tcl;

import cornell.Jacl.*;

/**
 * Tcl FAL package.
 * Add all the application layer TCL commands to the tcl interpreter.
 * 
 * @see FtsPatcherCmd
 * @see FtsObjectCmd
 * @see FtsDeclareCmd
 * @see FtsConnectionCmd
 * @see FtsInitCmd
 * @see FtsSyncCmd
 * @see FtsAbstrationCmd
 * @see FtsTemplateCmd
 * @see FtsHelpPatchCmd
 * @see FtsReferenceURLCmd
 * @see FtsMessCmd
 * @see FtsUcsCmd
 * @see StatsCmd
 * @see FtsClassNameCmd
 * @see FtsSetPropertyCmd
 */

public class TclFtsPackage extends cornell.Jacl.Package
{
  /**
   * Create the package.
   * Add all the commands to the given intepreter.
   *
   * @param interp the TCL interpreter instance
   */

  public TclFtsPackage(Interp interp)
  {
    super(interp);

    // Patcher creation

    interp.CreateCommand("patcher",  new FtsPatcherCmd());
    interp.CreateCommand("object",   new FtsObjectCmd());
    interp.CreateCommand("declare",  new FtsDeclareCmd());
    interp.CreateCommand("connection",  new FtsConnectionCmd());

    interp.CreateCommand("init",  new FtsInitCmd());
    interp.CreateCommand("sync", new FtsSyncCmd());

    // Abstraction and templates declaration, help and blah blah

    interp.CreateCommand("abstraction",  new FtsAbstractionCmd());

    interp.CreateCommand("template",  new FtsTemplateCmd());
    interp.CreateCommand("helpPatch",  new FtsHelpPatchCmd());
    interp.CreateCommand("referenceURL",  new FtsReferenceURLCmd());

    // Messages

    interp.CreateCommand("mess", new FtsMessCmd());
    interp.CreateCommand("ucs", new FtsUcsCmd());

    // Access to object characteristic

    interp.CreateCommand("className", new FtsClassNameCmd());

    // Access to object FTS properties

    interp.CreateCommand("setProperty", new FtsSetPropertyCmd());
    interp.CreateCommand("addListener", new FtsAddListenerCmd());

    // version control ?
    interp.CreateCommand("version", new FtsVersionCmd());
  }
}
