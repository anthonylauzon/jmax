package ircam.jmax.editors.patcher.menus;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

import ircam.jmax.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;
import ircam.jmax.dialogs.*;
import ircam.jmax.utils.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.actions.*;

/** Implement the patcher editor File Menu */

public class EditMenu extends PatcherMenu
{
  boolean locked;

  JMenuItem cutItem;
  JMenuItem copyItem;
  JMenuItem pasteItem;
  JMenuItem duplicateItem;
  JMenuItem selectAllItem;
  JMenuItem inspectItem;
  JMenuItem lockItem;
  
  public EditMenu()
  {
    super("Edit");
    setHorizontalTextPosition(AbstractButton.LEFT);

    cutItem       = add(Actions.cutAction, "Cut", Event.CTRL_MASK, KeyEvent.VK_X);
    copyItem      = add(Actions.copyAction, "Copy", Event.CTRL_MASK, KeyEvent.VK_C);
    pasteItem     = add(Actions.pasteAction, "Paste", Event.CTRL_MASK, KeyEvent.VK_V);
    duplicateItem = add(Actions.duplicateAction, "Duplicate", Event.CTRL_MASK, KeyEvent.VK_D);

    addSeparator();

    selectAllItem = add(Actions.selectAllAction, "SelectAll", Event.CTRL_MASK, KeyEvent.VK_A);

    addSeparator();

    add(Actions.findAction, "Find");
    add(Actions.findErrorsAction, "Find Errors");

    addSeparator();

    inspectItem = add(Actions.inspectAction, "Inspect");

    addSeparator();

    add( new AlignMenu());

    addSeparator();

    lockItem = add(Actions.lockAction, "Lock", Event.CTRL_MASK, KeyEvent.VK_E);

    enableCut(true);
    enableCopy(true);
    enablePaste(true);
    enableDuplicate(true);
  }


  // @@@ ALL THIS stuff should be done using actions.

  void enableCut(boolean v)
  {
    cutItem.setEnabled(v);
  }

  void enableCopy(boolean v)
  {
    copyItem.setEnabled(v);
  }

  void enablePaste(boolean v)
  {
    pasteItem.setEnabled(v);
  }

  void enableDuplicate(boolean v)
  {
    duplicateItem.setEnabled(v);
  }

  void enableSelectAll(boolean v)
  {
    selectAllItem.setEnabled(v);
  }

  void enableInspect(boolean v)
  {
    inspectItem.setEnabled(v);
  }

  public void setLocked(boolean v)
  {
    locked = v;
    updateMenus();
  }

  public void selectionChanged()
  {
    updateMenus();
  }

  private void updateMenus()
  {
    if (locked)
      {
	lockItem.setText("Unlock");
	enableSelectAll(false);
	enableCut(false);
	enableCopy(false);
	enableDuplicate(false);
	enableInspect(false);
      }
    else
      {
	lockItem.setText("Lock");

	if (ErmesSelection.patcherSelection.isEmpty())
	  {
	    enableCut(false);
	    enableCopy(false);
	    enableDuplicate(false);
	  }
	else
	  {
	    enableCut(true);
	    enableCopy(true);
	    enableDuplicate(true);
	  }

	enableInspect(true);
	enableSelectAll(true);
      }

    enablePaste(! ErmesSketchWindow.ftsClipboardIsEmpty());
  }
}
