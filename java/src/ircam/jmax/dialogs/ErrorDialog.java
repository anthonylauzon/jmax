package ircam.jmax.dialogs;

import java.awt.*;
import java.awt.event.*;

/**
 * A generic error dialog for extreme situations
 */

public class ErrorDialog extends Dialog implements ActionListener{
  Frame parent;
  String itsError;
  Button okButton;

  public ErrorDialog(Frame dw, String theError) {
    super(dw, "ermes Error", false);
    parent = dw;
    itsError = theError;

    //Create middle section.
    Panel p1 = new Panel();
    Label label = new Label(itsError);
    p1.add(label);
    add("Center", p1);

    //Create bottom row.
    Panel p2 = new Panel();
    p2.setLayout(new FlowLayout(FlowLayout.RIGHT));
    Button okButton = new Button("OK");
    okButton.addActionListener(this);
    p2.add(okButton);
    add("South", p2);

    //Initialize this dialog to its preferred size.
    pack();
  }

  public void actionPerformed(ActionEvent e){    
    setVisible(false);
    dispose();
  }
}





