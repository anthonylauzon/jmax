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
package ircam.jmax.dialogs;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.utils.*;

public class RegisterDialog {
  public static int popup()
  {
      JTextArea textArea = new JTextArea( 
"Register as jMax user\n\nPlease consider registering as jMax user.\n\nTo register, send a mail to jmax-register@ircam.fr\nwith bla bla bla\n\nhttp://www.ircam.fr/jmax\n\n\n\n\n\n\n\n\n"
);

      textArea.setEditable( false);

      JScrollPane scrollPane = new JScrollPane( textArea);
                
      Object[] message = new Object[1];

      message[0] = scrollPane;

      String[] options = { "OK", "Register later", "Cancel"};

      return JOptionPane.showOptionDialog( MaxWindowManager.getTopFrame(), message, "Register", JOptionPane.DEFAULT_OPTION, JOptionPane.INFORMATION_MESSAGE, null, options, options[0]);
  }
}

