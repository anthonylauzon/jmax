package ircam.jmax.editors.patcher;

import ircam.jmax.fts.*;
import ircam.jmax.mda.*;

/*
 * The Ermes Data Editor; represent the sketch window in the Mda system.
 */

public class ErmesDataEditor extends AbstractMaxDataEditor
{
  private ErmesSketchWindow window = null;
  
  public ErmesDataEditor(FtsPatcherData p)
  {
    super(p);

    setSketchWindow(new ErmesSketchWindow(p));
    fireEditorReadyListeners();
  }

  public void setSketchWindow(ErmesSketchWindow window)
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
    if (window != null)
      {
	window.setVisible(true);
	window.toFront();
	window.itsPatcherData.startUpdates();
      }
  }

       
  /** This method ask to the stop editing the data.
   *  This probabily means that the data item or the editor is about to
   * be disposed (destroyed).
   *
   */

  public void quitEdit()
  {
    if (window != null)
      window.Destroy();

    window = null;
  }

  /*
   * Tell the editor to show a specif piece of data, here
   * represented by a Java Object.
   */

  public void showObject(Object object)
  {
    if (window.isLocked())
      window.setLocked(false);

    if (window != null)
      window.showObject(object);
  }
}





