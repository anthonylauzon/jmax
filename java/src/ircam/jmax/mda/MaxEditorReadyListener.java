package ircam.jmax.mda;

/** An Editor may be create asynchoniously; what happen
  is that an MaxDataEditor is created immediately, and the 
  real editor can be created later; a MaxEditorReadyListener
  is an object that want to be informed when the editor is ready.
  */

public interface MaxEditorReadyListener
{
  public abstract void editorReady(MaxDataEditor editor);
}




