package ircam.jmax.editors.patcher;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.utils.*;
import ircam.jmax.editors.patcher.actions.*;

/** Implement the key handling for the sketchpad, using the registerKeyboardAction
    mechanism; note that this can be easily extend for adding scripts commands.

    Actually, should be splitted in two classes, one generic and one specific.
    */


class KeyMap
{
  ErmesSketchPad  sketch;
  ErmesSketchWindow editor;

  KeyMap(ErmesSketchPad sketch)
  {
    // Set local variables

    this.sketch = sketch;
    this.editor = sketch.getSketchWindow();


    // Install the keyboard actions

    // Move actions

    add(new MoveAction(editor, -10,   0, Event.SHIFT_MASK, KeyEvent.VK_LEFT));
    add(new MoveAction(editor,  10,   0, Event.SHIFT_MASK, KeyEvent.VK_RIGHT));
    add(new MoveAction(editor,   0, -10, Event.SHIFT_MASK, KeyEvent.VK_UP));
    add(new MoveAction(editor,   0,  10, Event.SHIFT_MASK, KeyEvent.VK_DOWN));

    add(new MoveAction(editor, -1,  0, 0, KeyEvent.VK_LEFT));
    add(new MoveAction(editor,  1,  0, 0, KeyEvent.VK_RIGHT));
    add(new MoveAction(editor,  0, -1, 0, KeyEvent.VK_UP));
    add(new MoveAction(editor,  0,  1, 0, KeyEvent.VK_DOWN));

    // Fixed Resize Actions

    add(new ResizeAction(editor, -10,   0, Event.CTRL_MASK | Event.SHIFT_MASK, KeyEvent.VK_LEFT));
    add(new ResizeAction(editor,  10,   0, Event.CTRL_MASK | Event.SHIFT_MASK, KeyEvent.VK_RIGHT));
    add(new ResizeAction(editor,   0, -10, Event.CTRL_MASK | Event.SHIFT_MASK, KeyEvent.VK_UP));
    add(new ResizeAction(editor,   0,  10, Event.CTRL_MASK | Event.SHIFT_MASK, KeyEvent.VK_DOWN));

    add(new ResizeAction(editor, -1,  0, Event.CTRL_MASK, KeyEvent.VK_LEFT));
    add(new ResizeAction(editor,  1,  0, Event.CTRL_MASK, KeyEvent.VK_RIGHT));
    add(new ResizeAction(editor,  0, -1, Event.CTRL_MASK, KeyEvent.VK_UP));
    add(new ResizeAction(editor,  0,  1, Event.CTRL_MASK, KeyEvent.VK_DOWN));

    // Resize Align operations

    add(new ResizeToMaxWidthAction(editor));
    add(new ResizeToMaxHeightAction(editor));

    // Annotation 

    add(new ShowErrorAction(editor));
  }


  /** Add a PatcherAction to the keymap */

  public void add(PatcherAction action)
  {
    add(action, JComponent.WHEN_FOCUSED);
  }

  public void add(PatcherAction action, int condition)
  {
    if (action.haveMnemonic())
      {
	sketch.registerKeyboardAction(action, 
				      KeyStroke.getKeyStroke(action.getMnemonic(), action.getModifiers()),
				      condition);
      }
  }
}


