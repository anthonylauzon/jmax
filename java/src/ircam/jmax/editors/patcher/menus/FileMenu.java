package ircam.jmax.editors.patcher.menus;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

import ircam.jmax.editors.patcher.*;
import ircam.jmax.editors.patcher.actions.*;


/** Implement the patcher editor File Menu */

public class FileMenu extends PatcherMenu
{
  public FileMenu(ErmesSketchWindow window)
  {
    super("File", window);

    setHorizontalTextPosition(AbstractButton.LEFT);

    add(new NewAction(getEditor()));
    add(new OpenAction(getEditor()));

    addSeparator();

    add(new SaveAction(getEditor()));
    add(new SaveAsAction(getEditor()));
    add(new SaveToAction(getEditor()));
    add(new CloseAction(getEditor()));

    addSeparator();

    add(new PrintAction(getEditor()));
    add(new StatisticsAction(getEditor()));
    add(new QuitAction(getEditor()));
  }
}

