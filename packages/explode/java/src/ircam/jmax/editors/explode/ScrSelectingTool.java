//@@@ cambia il costruttore (rendilo simile a ScrAddingTool)
//adotta lo stesso stile sulla sorg. e dest. grafica (no ScrPanel, perche'?)
package ircam.jmax.editors.explode;

import java.awt.*;
import java.awt.event.*;
import com.sun.java.swing.ImageIcon;

/**
 * the tool used to perform selections in the editor.
 */ 
public class ScrSelectingTool extends ScrTool implements SelectionListener{

  ScrPanel itsPanel;
  Selecter itsSelecter;
  SelectionHandler itsSelectionHandler;

  /**
   * Constructor. It needs to know the panel and the SelectionHandler
   * on which to operate 
   */
  public ScrSelectingTool(ScrPanel thePanel, SelectionHandler theSelectionHandler) {
    super("select", new ImageIcon("/u/worksta/maggi/projects/max/images/tool_bang.gif"));
    itsPanel = thePanel;
    itsSelectionHandler = theSelectionHandler;
    itsSelecter = new Selecter(this, itsPanel);
  }

  public void activate() {

    mountIModule(itsSelecter);
  }

  public void deactivate(){}

  public void selectionChoosen(int x, int y, int w, int h) {
    itsSelectionHandler.selectArea(x, y, w, h);
    itsPanel.repaint();
  }
}





