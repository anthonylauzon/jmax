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
import ircam.jmax.toolkit.*;

public class RegisterDialog {

  private static String text = 
  "\nWelcome to jMax !\n\nPlease consider registering yourself as jMax user. \nNote that:\n - it is not required\n - it is free\n - it will not add you to mailing distributions\n\nThis registration is only intended to support the jMax \nproject by giving us an approximate idea of the number \nof users, and particularly the number of users of the \nLinux version.\n\nTo register, simply send a mail to jmax-register@ircam.fr,\nand indicate the platform that you run jMax on.\n\nPlease visit jMax home page at: http://www.ircam.fr/jmax\nand subscribe to the jMax mailing list.\n\nPlease also consider subscribing to the Ircam Forum:\nhttp://www.ircam.fr/forum\n\nNOTE:\n This panel is automatically disabled and will not show \n again next time you start jMax. If this does not happen,\n you can disable it by hand either by creating a file named\n .jmaxregistered in your home directory, start jMax with \n the option '-jmaxNoRegister true' or put in your .jmaxrc \n file the command 'set jmaxNoRegister true'.\n";

  public static void popup()
  {
    JTextArea textArea = new JTextArea( text, 30, 60);

    textArea.setEditable( false);

    JScrollPane scrollPane = new JScrollPane() {
      public Dimension getPreferredSize()
	{
	  return new Dimension( 400, 300);
	}
    };

    scrollPane.setViewportView( textArea);
                     
    Object[] message = new Object[1];

    message[0] = scrollPane;

    String[] options = { "OK" };

    JOptionPane.showOptionDialog( MaxWindowManager.getTopFrame(), 
				  message, 
				  "Register", 
				  JOptionPane.DEFAULT_OPTION, 
				  JOptionPane.INFORMATION_MESSAGE, 
				  IconCache.getIcon( "jmax_logo_tiny.gif"), 
				  options, 
				  options[0]);
  }
}

