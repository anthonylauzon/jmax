//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
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

