package ircam.jmax.editors.patcher.menus;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.event.*;

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
  class EditMenuListener implements MenuListener
  {
    public void menuSelected(MenuEvent e)
    {
      updateMenu();
    }

    public void menuDeselected(MenuEvent e)
    {
    }

    public void menuCanceled(MenuEvent e)
    {
    }
  }

  ErmesSketchWindow window;

  JMenuItem cutItem;
  JMenuItem copyItem;
  JMenuItem pasteItem;
  JMenuItem duplicateItem;
  JMenuItem selectAllItem;
  JMenuItem inspectItem;
  JMenuItem lockItem;
  
  public EditMenu(ErmesSketchWindow window)
  {
    super("Edit");

    this.window = window;

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

    addMenuListener(new EditMenuListener());
  }

  private void updateMenu()
  {
    if (window.isLocked())
      {
	lockItem.setText("Unlock");
	selectAllItem.setEnabled(false);
	cutItem.setEnabled(false);
	copyItem.setEnabled(false);
	duplicateItem.setEnabled(false);
	inspectItem.setEnabled(false);
      }
    else
      {
	lockItem.setText("Lock");

	if (ErmesSelection.patcherSelection.isEmpty())
	  {
	    cutItem.setEnabled(false);
	    copyItem.setEnabled(false);
	    duplicateItem.setEnabled(false);
	  }
	else if (ErmesSelection.patcherSelection.getOwner() == window.itsSketchPad)
	  {
	    cutItem.setEnabled(true);
	    copyItem.setEnabled(true);
	    duplicateItem.setEnabled(true);
	  }

	inspectItem.setEnabled(true);
	selectAllItem.setEnabled(true);
      }

    pasteItem.setEnabled(! ErmesSketchWindow.ftsClipboardIsEmpty());
  }
}
