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

package ircam.jmax.editors.console;

import java.awt.*;

public class ConsoleTextArea extends TextArea {
  static final int CHAR_SAVED=1000; 

  public ConsoleTextArea(int r, int c) {
    super(r, c);
  }
  
  public void append(String str) {
    super.append(str);
    /*if (getText().length() < CHAR_SAVED) {
      super.append(str);
    }
    else {
      String temp = getText();
      setText(temp.substring(temp.length()-CHAR_SAVED, temp.length()));
      setCaretPosition(getText().length());
    }*/
  }
}

