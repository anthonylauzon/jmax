package ircam.jmax.editors.patcher;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

import ircam.jmax.toolkit.*;

//
// The graphic pop-up menu used to change the number of an inlet or an outlet in a subpatcher.
//

public class AddPopUp extends JPopupMenu
{
  private ErmesSketchPad sketch;  
  int x;
  int y;

  class AddPopUpListener implements ActionListener
  {
    String descr;

    AddPopUpListener(String descr)
    {
      this.descr = descr;
    }

    public void actionPerformed( ActionEvent e)
    { 
      sketch.makeObject(descr, x, y);
    }
  };

  //
  // Constructor accepting the number of in/out to show in the popup
  //

  public AddPopUp(ErmesSketchPad sketch)
  {
    this.sketch = sketch;

    insertButtons();
  }


  private void addButton( String descr, String iconName)
  {
    JMenuItem item;

    item = new JMenuItem(Icons.get(iconName));
    item.addActionListener(new AddPopUpListener(descr));
    add(item);
  }

  private void insertButtons()
  {
    addButton( "", "_object_");
    addButton( "messbox", "_message_box_");
    addButton( "jpatcher", "_patcher_");
    addButton( "inlet -1", "_inlet_");
    addButton( "outlet -1","_outlet_");
    addButton( "comment", "_comment_");
    addButton( "button",  "_button_");
    addButton( "toggle",  "_toggle_");
    addButton( "slider",  "_slider_");
    addButton( "intbox",   "_intbox_");
    addButton( "floatbox", "_floatbox_");
  }

  public void show(Component invoker, int x, int y)
  {
    this.x = x;
    this.y = y;
    super.show(invoker, x, y);
  }
}



