package ircam.jmax.fts.tcl;

import tcl.lang.*;

/**
 * Tcl FTS extension.
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

public class TclFtsPackage
{
  /**
   * Add all the commands to the given intepreter.
   *
   * @param interp the TCL interpreter instance
   */

  static public void installPackage(Interp interp)
  {
    // File Format

    interp.createCommand("patcher",   new FtsPatcherCmd());
    interp.createCommand("object",    new FtsObjectCmd());
    interp.createCommand("declare",   new FtsDeclareCmd());
    interp.createCommand("comment",   new FtsCommentCmd());
    interp.createCommand("inlet",     new FtsInletCmd());
    interp.createCommand("outlet",    new FtsOutletCmd());
    interp.createCommand("connection", new FtsConnectionCmd());


    // Abstraction and templates declaration, help and blah blah

    interp.createCommand("abstraction",  new FtsAbstractionCmd());

    interp.createCommand("template",  new FtsTemplateCmd());
    interp.createCommand("helpPatch",  new FtsHelpPatchCmd());
    interp.createCommand("referenceURL",  new FtsReferenceURLCmd());

    // Messages

    interp.createCommand("mess", new FtsMessCmd());
    interp.createCommand("ucs", new FtsUcsCmd());

    // Access to object characteristic

    interp.createCommand("className", new FtsClassNameCmd());

    // Access to object FTS properties

    interp.createCommand("setProperty", new FtsSetPropertyCmd());
    interp.createCommand("addListener", new FtsAddListenerCmd());

    // version control ?

    interp.createCommand("version", new FtsVersionCmd());

    // Sync

    interp.createCommand("sync", new FtsSyncCmd());
  }
}


