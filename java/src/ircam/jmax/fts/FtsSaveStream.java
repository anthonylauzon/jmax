package ircam.jmax.fts;

import java.io.*;

/**
 * A specialization of the printstream.
 * Handle indentation
 * at the newline to improve, a little, the readability of the .tpa
 * files.
 */

class FtsSaveStream extends PrintWriter
{
  int indentation = 0;

  public FtsSaveStream(OutputStream stream)
  {
    super(stream);
  }

  public void println()
  {
    super.println();

    for (int i = 0; i < indentation; i++)
      print(" ");
  }

  void indentMore()
  {
    indentation += 1;
  }

  void indentLess()
  {
    indentation -= 1;
  }
}
