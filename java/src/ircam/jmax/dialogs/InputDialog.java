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

package ircam.jmax.dialogs;

import java.awt.*;
import java.awt.event.*;

// import javax.swing.*;
import javax.swing.JButton;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JTextField;

// import javax.swing.border.*;
import javax.swing.border.EmptyBorder;

import ircam.jmax.*;

/**
 * This class implements a simple question dialog.  Use it thru the
 * static "ask" function; it provide the complete
 * handling. InputDialog is implemented as a JFrame and not as a
 * Dialog for the simple reason that it should be possible the ask a
 * question to the user from the inerpreter shell.  */

public class InputDialog extends JFrame implements ActionListener
{
    JButton yesButton;
    JButton noButton;
    String answer;
    JTextField input;

    public static String ask(String question) {
	return ask(question, "OK", "Cancel");
    }

    public static String ask(String question, String yes, String no) {
	String answer;
	InputDialog dialog = new InputDialog(question, yes, no);
	answer = dialog.answer;
	dialog.dispose();
	return answer;
    }

    public InputDialog(String question, String yes, String no) {
	super(question);

	JPanel p1 = new JPanel(new GridLayout(0, 1));

	JLabel label = new JLabel(question);
	label.setBorder(new EmptyBorder(10, 20, 10, 20));
	label.setHorizontalAlignment(JLabel.CENTER);
	p1.add(label);
	
	input = new JTextField(32);
	p1.add(input);

	//Create bottom row.
	JPanel p2 = new JPanel(new GridLayout(1,3));

	yesButton = new JButton(yes);
	yesButton.addActionListener(this);
	p2.add(yesButton);

	noButton = new JButton(no);
	noButton.addActionListener(this);
	p2.add(noButton);

	p1.add(p2);
	getContentPane().add(p1);

	pack();

	Dimension screen_dim = Toolkit.getDefaultToolkit().getScreenSize();
	Dimension dialog_dim = getSize();
	setLocation((screen_dim.width - dialog_dim.width) / 2, (screen_dim.height - dialog_dim.height) / 2);
	show();

	waitForAnswer();
    }

    protected synchronized void waitForAnswer() {    
	try {
	    wait();
	} catch (Exception e) {
	}
    }

    protected synchronized void endWaiting() {    
	notify();
    }

    public void actionPerformed(ActionEvent e) {    
	if (e.getSource() == yesButton) {
	    answer = input.getText().trim();
	    setVisible(false);
	    endWaiting();
	} else if(e.getSource() == noButton) {
	    answer = null;
	    setVisible(false);
	    endWaiting();
	}
    }
}






