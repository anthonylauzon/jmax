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
    StringTokenizer st = new StringTokenizer(descr);

    x  = Integer.parseInt(st.nextToken());
    y  = Integer.parseInt(st.nextToken());
    width  = Integer.parseInt(st.nextToken());
    height = Integer.parseInt(st.nextToken());
  }

  /** Save the description to a stream */

  void saveAsTcl(FtsSaveStream stream)
  {
      stream.print("{" + x + " " + y + " " + width + " " + height + "}");
  }
}


