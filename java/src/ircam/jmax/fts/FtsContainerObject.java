package ircam.jmax.fts;

import java.io.*;
import java.util.*;

/** 
 *  This is the super class of all
 * objects that include other objects, like
 * patchers, template and abstractions.
 */

abstract public class FtsContainerObject extends FtsObject
{
  /** Just for the building of the root object */

  FtsContainerObject()
  {
  }

  protected  FtsContainerObject(FtsContainerObject parent, String className, Vector args)
  {
    super(parent, className, args);
  }

  /** The object subpatch. Null if the object is not a container. */

  protected FtsPatcher subPatcher = null;	

  /** Get the subPatcher, an implementational object represeting a patcher content. */

  public FtsPatcher getSubPatcher()
  {
    return subPatcher;
  }

  /** Set the subPatcher of an object,only for .pat support. */

  public void setSubPatcher(FtsPatcher subPatcher)
  {
    this.subPatcher = subPatcher;

    subPatcher.setObject(this);
  }


  /** 
   * Add an object to this container
   */

  public void addObject(FtsObject obj)
  {
    subPatcher.addObject(obj);
  }

  /**
   * Tell Fts the patcher is Open.
   * An open patcher is a patcher for which we want
   * continuous updates.
   */

  public void open()
  {
    subPatcher.open();
  }

  /**
   * Tell Fts the patcher is Closed.
   * An open patcher is a patcher for which we want
   * continuous updates.
   */

  public void close()
  {
    subPatcher.close();
  }

  /** Check if the object is an open patcher. */

  public boolean isOpen()
  {
    return subPatcher.isOpen();
  }


  /**
   * Declare the patcher loaded.
   * This fire the after load initializations
   * in FTS.
   */

  public void loaded()
  {
    subPatcher.loaded();
  }

  /**
   * Return the contained objects.
   * For an object representing a patcher, return
   * a Vector of all the FtsObject in the patcher.
   */

  public Vector getContainedObjects()
  {
    return subPatcher.getObjects();
  }

  /**
   * Return the contained connections.
   * For an object representing a patcher, return
   * a Vector of all the FtsConnection in the patcher.
   */

  public Vector getContainedConnections()
  {
    return subPatcher.getConnections();
  }

  /** Get the Window description of a patcher. */

  public FtsWindowDescription getWindowDescription()
  {
    return subPatcher.getWindowDescription();
  }

  /** Set the Window description of a patcher. */

  public void setWindowDescription(FtsWindowDescription wd)
  {
    subPatcher.setWindowDescription(wd);
  }


  /**
   * Save the object to an output stream.
   * <i>Bug: it actually work only if the object is a patcher.</i>
   */

  public void saveTo(OutputStream stream)
  {
    subPatcher.saveTo(stream);
  }


  /**
   * Find all the objects that have a given pattern 
   * in its descriptions.
   */

  public Vector find(String pattern)
  {
    Vector v = new Vector();

    subPatcher.find(pattern, v);

    return v;
  }
}
