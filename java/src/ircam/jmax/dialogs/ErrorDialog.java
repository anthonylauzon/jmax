package ircam.jmax.dialogs;

import java.awt.*;


/**
 * A generic error dialog for extreme situations
 */

public class ErrorDialog extends Dialog {
    Frame parent;
    String itsError;
    Button b;

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
        Button b = new Button("OK");
        p2.add(b);
        add("South", p2);

        //Initialize this dialog to its preferred size.
        pack();

    }

    public boolean action(Event event, Object arg) {
        if ( (event.target == b)
           /*| (event.target == field)*/) {
        //    parent.setText(field.getText());
        }
        //field.selectAll();
        hide();
        return true;
    }
}
