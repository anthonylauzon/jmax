package ircam.jmax.mda;


/** This interface represent the minimum requirement on 
 *  an "Max Editing Unit", any Java object, part of a document,
 *  that we want to "edit" with a single user visible entity
 * called editor.
 * 
 * For the moment, quite minimal.
 */

public interface MaxData
{
  /** Get the document this data belong to */

  abstract public MaxDocument getDocument();
}


