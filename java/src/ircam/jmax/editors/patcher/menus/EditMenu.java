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
  
  public EditMenu(ErmesSketchWindow window)
  {
    super("Edit", window);
    setHorizontalTextPosition(AbstractButton.LEFT);

    cutItem       = add(new CutAction(getEditor()));
    copyItem      = add(new CopyAction(getEditor()));
    pasteItem     = add(new PasteAction(getEditor()));
    duplicateItem = add(new DuplicateAction(getEditor()));

    addSeparator();

    selectAllItem = add(new SelectAllAction(getEditor()));

    addSeparator();

    add(new FindAction(getEditor()));
    add(new FindErrorsAction(getEditor()));

    addSeparator();

    inspectItem = add(new InspectAction(getEditor()));

    addSeparator();

    add( new AlignMenu(getEditor()));

    addSeparator();

    lockItem = add(new LockAction(getEditor()));

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
	lockItem.setLabel("Unlock");
	enableSelectAll(false);
	enableCut(false);
	enableCopy(false);
	enableDuplicate(false);
	enableInspect(false);
      }
    else
      {
	lockItem.setLabel("Lock");

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

    enablePaste(! getEditor().ftsClipboardIsEmpty());
  }
}
