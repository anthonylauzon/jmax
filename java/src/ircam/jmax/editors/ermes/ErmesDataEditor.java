package ircam.jmax.editors.ermes;

import ircam.jmax.fts.*;
import ircam.jmax.mda.*;

/*
 * The Ermes Data Editor; represent the sketch window in the Mda system.
 */

public class ErmesDataEditor implements  MaxDataEditor
{
  private ErmesSketchWindow window = null;
  private FtsContainerObject patcher;

  
  public ErmesDataEditor(FtsContainerObject p)
  {
    patcher = p;

    if (patcher.isDownloaded())
      setSketchWindow(new ErmesSketchWindow(patcher));
    else
      patcher.download(new Runnable() {
	public void run() { ErmesDataEditor.this.setSketchWindow(new ErmesSketchWindow(patcher));
	}
      });
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
	window.itsPatcher.startUpdates();
      }
  }

  /**
   * This method return the data instance the editor is editing
   */

  public MaxData getData()
  {
    if (window != null)
      return (MaxData) window.itsPatcher;
    else
      return null;
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
	window.itsPatcher = null; // (fd) ???
	window.Destroy();
      }
  }

  /** Tell the editor to syncronize, i.e. to store in the
   * data all the information possibly cached in the editor
   * and still not passed to the data instance; this usually
   * happen before saving an instance.
   */

  public void syncData()
  {
  }

  /*
   * Tell the editor to show a specif piece of data, here
   * represented by a Java Object.
   */

  public void showObject(Object object)
  {
    if (window != null)
      window.showObject(object);
  }
}





