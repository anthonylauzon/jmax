//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.utils;

import java.io.*;

/** A specialization of the print writer that support
 *   Indentation; indentation is printed on the output
 *   each time a println is done; it add a couple of functions more.
 *
 *  Warning: the indentation feature will work only if just
 *  the print and println methods on strings will be called,
 *  so it is probabily of limited use.
 */

public class IndentedPrintWriter extends PrintWriter
{
  boolean lineDone = false;
  int indentation = 0;

  public IndentedPrintWriter(Writer stream) {
    super(stream); //EM
  }

  public IndentedPrintWriter(OutputStream stream)
  {
    super(stream);
  }

  public void println()
  {
    super.println();

    lineDone = false;
  }

  public void println(String s)
  {
    super.println(s);

    lineDone = false;
  }


  public void print(String s)
  {
    if (! lineDone)
      {
	lineDone = true;

	for (int i = 0; i < indentation; i++)
	  print(" ");
      }

    super.print(s);
  }

  public void indentMore()
  {
    indentation += 2;
  }

  public void indentLess()
  {
    indentation -= 2;
  }
}
