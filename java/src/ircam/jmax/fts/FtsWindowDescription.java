package ircam.jmax.fts;
import java.io.*;
import java.util.*;

/**
 * Class keeping the graphic information for an object corresponding
 *  to a Window when saving/getting it from a file, or when the object
 *  is not being edited.  Window description will be soon splitted
 *  in multiple object properties
 */


public class FtsWindowDescription
{
  /** X coordinate of the window. */

  public int x;

  /** Y coordinate of the window. */

  public int y;


  /** Vertical size of the window. */

  public int height;

  /** Horizontal size of the window. */

  public int width;

  /** Additional information on the object. */

  public String ermesInfo = null;

  /** Flag that register if a Window is open or not */

  public boolean open = false;


  /** Build a Window Description from discrete argument. */

  public FtsWindowDescription(int x, int y, int width, int height)
  {
    super();

    this.x = x;
    this.y = y;
    this.width = width;
    this.height = height;
  }


  /** Build a Window Description by parsing a string.*/

  public FtsWindowDescription(String descr)
  {
    // Temporarly we parse the string immediately.
    // it should really parse by need when the value
    // is accessed; also, the parser can be smarter

    // The format *must* be <x> <y> <width> <height> <open> ["ermes info"].
    //for a description of the ermesInfo, see the FtsGraphicDescription.java file
    // the ermesInfo, for now, contains AT LEAST the attribute (name: theName)
    // where open can be 1 or 0

    StringTokenizer st = new StringTokenizer(descr);
    int toOpen;

    x  = Integer.parseInt(st.nextToken());
    y  = Integer.parseInt(st.nextToken());
    width  = Integer.parseInt(st.nextToken());
    height = Integer.parseInt(st.nextToken());
    toOpen  = Integer.parseInt(st.nextToken());

    if (descr.indexOf('"') != -1)
      ermesInfo = descr.substring(descr.indexOf('"')+1, descr.lastIndexOf('"'));
    else
      ermesInfo = null;//old format..

    if (toOpen == 1)
      open = true;
  }

  /** Save the description to a stream */

  void saveAsTcl(FtsSaveStream stream)
  {
    if (ermesInfo == null)
      stream.print("{" + x + " " + y + " " + width + " " + height + " " + (open ? 1 : 0) + "}");
    else
      stream.print("{" + x + " " + y + " " + width + " " + height + " " + (open ? 1 : 0) + " "+"\""+ermesInfo+"\""+"}");
  }
}


