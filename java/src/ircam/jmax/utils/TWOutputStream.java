//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
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
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.utils;
import java.awt.*;
import java.io.*;

/** 
 *A simple OutputStream associated with the TextWindow 
*/

public class TWOutputStream extends OutputStream
{
  TextWindow itsTextWindow;
  StringBuffer buffer = new StringBuffer(256);
  int count = 0;
	
  public TWOutputStream(TextWindow theTextWindow)
  {
    super();
    itsTextWindow = theTextWindow;
  }
	
  public void write(int b) /*throws IOException*/
  {
    buffer.append((char)b);
    count++;

    if ((b == '\n') || (count > 1024)){
      itsTextWindow.itsTextArea.append(buffer.toString());
      buffer.setLength(0);
      count = 0;
    }
  }

  public void write(String s)
  {
    itsTextWindow.itsTextArea.append(s);
  }
}
