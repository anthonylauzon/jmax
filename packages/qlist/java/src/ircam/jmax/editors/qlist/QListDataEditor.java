package ircam.jmax.editors.qlist;

import ircam.jmax.fts.*;
import ircam.jmax.mda.*;

/*
 * The QList Data Editor; represent the QList editor in the Mda system.
 */


public class QListDataEditor extends AbstractMaxDataEditor
{
  private QList window = null;
  
  public QListDataEditor(FtsAtomList data)
  {
    super(data);

    setQListWindow(new QList(data));
    fireEditorReadyListeners();
  }

  public void setQListWindow(QList window)
  {
    this.window = window;
  }

  /**
   * This method is called when an edit is asked
   * for the data the editor is already editing; the semantic
   * is editor dependent.
   */

  public void reEdit()
  {
    // Should call a method of QList, should not 
    // do the work here.

    if (! window.isVisible())
      window.setVisible(true);

    window.toFront();
  }

  /** This method ask to the stop editing the data.
   *  This probabily means that the data item or the editor is about to
   * be disposed (destroyed).
   *
   */

  public void quitEdit()
  {
    if (window != null)
      {
	window.setVisible(false);
	window.dispose();
      }
  }
}





