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
import java.awt.*;
import java.io.*;

/** TW PrintStream: a simple PrintStream associated with TextWindows*/

/* Obsolete class: Print Stream is already a filter (by the way, deprecated
   in 1.1, we should use PrintWriter), so we don't need to specialize
   it, just to pass our OutputStream at creation time
   */

public class TWPrintStream extends PrintStream {
	TWOutputStream itsOutputStream;
	
	public TWPrintStream(TWOutputStream theOutputStream) {
		super(theOutputStream);
		itsOutputStream = theOutputStream;
	}
	
	public void println(String s) {
		itsOutputStream.write(s);
		itsOutputStream.write('\n');
	}
	
	//etc...
}
