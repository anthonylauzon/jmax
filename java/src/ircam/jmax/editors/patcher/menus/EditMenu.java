package ircam.jmax.editors.patcher.menus;

import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;

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

    add(Actions.findAction, "Find", Event.CTRL_MASK, KeyEvent.VK_F);
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
    Transferable clipboardContent = MaxApplication.systemClipboard.getContents(this);
    DataFlavor[] flavors = clipboardContent.getTransferDataFlavors();

    if (window.isLocked())
      {
	lockItem.setText("Unlock");
	selectAllItem.setEnabled(false);
	cutItem.setEnabled(false);
	copyItem.setEnabled(false);
	duplicateItem.setEnabled(false);
	inspectItem.setEnabled(false);

	pasteItem.setEnabled((flavors != null) &&
			     clipboardContent.isDataFlavorSupported(ErmesSelection.patcherSelectionFlavor));
      }
    else
      {
	lockItem.setText("Lock");

	if (window.itsSketchPad.isTextEditingObject())
	  {
	    // Text editing, look at text selection

	    if (window.itsSketchPad.getSelectedText() != null)
	      {
		cutItem.setEnabled(true);
		copyItem.setEnabled(true);
	      }
	    else
	      {
		cutItem.setEnabled(false);
		copyItem.setEnabled(false);
	      }

	    duplicateItem.setEnabled(false);

	    pasteItem.setEnabled((flavors != null) &&
				 clipboardContent.isDataFlavorSupported(DataFlavor.stringFlavor));
	  }
	else
	  {
	    if (ErmesSelection.patcherSelection.isEmpty())
	      {
		// Empty selection

		cutItem.setEnabled(false);
		copyItem.setEnabled(false);
		duplicateItem.setEnabled(false);
	      }
	    else if (ErmesSelection.patcherSelection.getOwner() == window.itsSketchPad)
	      {
		// Object selection

		cutItem.setEnabled(true);
		copyItem.setEnabled(true);
		duplicateItem.setEnabled(true);
	      }

	    pasteItem.setEnabled((flavors != null) &&
				 clipboardContent.isDataFlavorSupported(ErmesSelection.patcherSelectionFlavor));
	  }

	inspectItem.setEnabled(true);
	selectAllItem.setEnabled(true);
      }
  }
}
