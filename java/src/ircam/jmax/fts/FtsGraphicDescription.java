package ircam.jmax.fts;
import java.util.*;
import java.io.*;

/**
 * Class keeping the graphic information for an object 
 * when saving/getting it from a file, or when the object
 * is not being edited.
 * Graphic description will be soon splitted in multiple 
 * object properties
 * CHANGING: Drop name and ermesInfo.
 * both will be implemented to optional property of the FtsObject,
 * in the new file format.
 */


public class FtsGraphicDescription
{
  /** X coordinate of the object. */

  public int x = 0;

  /** Y coordinate of the object. */

  public int y = 0;

  /** Vertical size of the object. */
  
  public int height = 0;

  /** Horizontal size of the object. */

  public int width = 0;

  /** Additional information on the object. */

  public String ermesInfo = null;

  public FtsGraphicDescription(int x, int y, int width, int height)
  {
    this.x = x;
    this.y = y;
    this.height = height;
    this.width = width;
  }

  public FtsGraphicDescription()
  {
  }

  /**
   * Parse a Graphic Description.
   * Parse a graphic description
   * from a String; used to parse the tpa files, or to build the
   * description from the editor.
   */

  public FtsGraphicDescription(String descr)
  {
    // Temporarly we parse the string immediately.
    // it should really parse by need when the value
    // is accessed; also, the parser can be smarter

    //The format *must* be " <x> <y> <width> <height> [<ermesInfo>]"

    //the ermesInfo string has the format \"whatever\""
    //whatever, for now (8 dec. 1997), is a string composed of arguments,
    //each of them has the form (type: value)
    //ex: timer 100 100 100 20 "(font: helvetica)(fontsize:14)"
    //this way, the argument number is optional, we can add future
    //argument types, we can skip some argument if the 
    //system doesn't handle it.  No argument is mandatory.
    //For now if a loader (es. ermes) doesn't use the argument, it should anyway
    //keep it while saving, in order not to loose informations.


    StringTokenizer st = new StringTokenizer(descr);

    x  = Integer.parseInt(st.nextToken());
    y  = Integer.parseInt(st.nextToken());
    width  = Integer.parseInt(st.nextToken());
    height = Integer.parseInt(st.nextToken());

    ermesInfo = null;//obsolete, moving away

  }

  /** Save the graphic description to a TCL file. */

  void saveAsTcl(FtsSaveStream stream)
  {
    stream.print("{ " + x + " " + y + " " + width + " " + height+"}");
  }

  public String toString()
  {
    return "FtsGraphicDescription{  " + x + " " + y + " " + width + " " + height + "}";
  }
}








