//
// jMax
// Copyright (C) 1999 by IRCAM
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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
