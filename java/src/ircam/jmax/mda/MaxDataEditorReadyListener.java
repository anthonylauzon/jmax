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
package ircam.jmax.mda;

/** An Editor may be create asynchoniously; what happen
  is that an MaxDataEditor is created immediately, and the 
  real editor can be created later; a MaxDataEditorReadyListener
  is an object that want to be informed when the editor is ready.
  */

public interface MaxDataEditorReadyListener
{
  public abstract void editorReady(MaxDataEditor editor);
}




