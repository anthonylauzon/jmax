 //
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

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
import ircam.jmax.editors.patcher.objects.*;

/** Implement the patcher editor File Menu */

public class TextMenu extends JMenu
{
  JMenuItem biggerItem;
  JMenuItem smallerItem;
  private JMenu itsSizesMenu;
  private ButtonGroup itsSizesMenuGroup;
  JRadioButtonMenuItem fakeSizeButton;

  private JMenu itsFontsMenu;
  private ButtonGroup itsFontMenuGroup;
  JRadioButtonMenuItem fakeFontButton;

  ErmesSketchWindow editor;

  class TextMenuListener implements MenuListener
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

  public TextMenu(ErmesSketchWindow window)
  {
    super("Text");
    setHorizontalTextPosition(AbstractButton.LEFT);

    JMenuItem item;
    editor = window;

    biggerItem = new JMenuItem("Bigger");
    biggerItem.addActionListener(new ActionListener()
				 {
				   public void actionPerformed(ActionEvent e)
				     {
				       ErmesSelection.patcherSelection.fontBigger();
				     }
				 });
    biggerItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_ADD, Event.CTRL_MASK));
    add(biggerItem);

    smallerItem = new JMenuItem("Smaller");
    smallerItem.addActionListener(new ActionListener()
				 {
				   public void actionPerformed(ActionEvent e)
				     {
				       ErmesSelection.patcherSelection.fontSmaller();
				     }
				 });

    smallerItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_SUBTRACT, Event.CTRL_MASK));
    add(smallerItem);

    itsSizesMenu = new JMenu( "Sizes");
    FillSizesMenu( itsSizesMenu);
    add( itsSizesMenu);

    itsFontsMenu =  new JMenu( "Fonts");
    FillFontMenu( itsFontsMenu);
    add( itsFontsMenu);
  }

  class SizesMenuAdapter implements ActionListener
  {
    JRadioButtonMenuItem item;
    int size;

    SizesMenuAdapter( JRadioButtonMenuItem item, int size)
    {
      this.item = item;
      this.size = size;
    }

    public void actionPerformed(ActionEvent ev) 
    {
      //if we are here, a font size have been choosen from the FONT menu

      if (ErmesSelection.patcherSelection.ownedBy(editor.itsSketchPad) && 
	  ErmesSelection.patcherSelection.hasObjects())
	{
	  try
	    {
	      ErmesSelection.patcherSelection.setFontSize(size);
	    }
	  catch (Exception e)
	    {
	      ErrorDialog aErr = new ErrorDialog(editor, "This font does not exist on this platform");
	      aErr.setLocation( 100, 100);
	      aErr.show();  
	    }
	}
      else
	editor.itsSketchPad.setDefaultFontSize(size);
    }
  }

  private void FillSizesMenu( JMenu menu)
  {
    int sizes[] = {8, 9, 10, 12, 14, 18, 24, 36, 48};
    JRadioButtonMenuItem item; 
    itsSizesMenuGroup = new ButtonGroup();

    fakeSizeButton = new JRadioButtonMenuItem( "fake");
    itsSizesMenuGroup.add(fakeSizeButton);

    for (int i = 0; i < sizes.length; i++)
      {
	item = new JRadioButtonMenuItem( Integer.toString(sizes[i]));
	menu.add( item);
	item.addActionListener( new SizesMenuAdapter( item, sizes[i]));
	itsSizesMenuGroup.add(item);
      }
  }


  class FontMenuAdapter implements ActionListener
  {
    JRadioButtonMenuItem item;
    String font;

    FontMenuAdapter( JRadioButtonMenuItem item, String font)
    {
      this.item = item;
      this.font = font;
    }

    public void actionPerformed(ActionEvent ev) 
    {
      if (ErmesSelection.patcherSelection.ownedBy(editor.itsSketchPad) && 
	  ErmesSelection.patcherSelection.hasObjects())
	{
	  try
	    {
	      ErmesSelection.patcherSelection.setFontName( font);
	    }
	  catch (Exception e) 
	    {
	      ErrorDialog aErr = new ErrorDialog(editor, "This font does not exist on this platform");
	      aErr.setLocation( 100, 100);
	      aErr.show();  
	    }
	}
      else
	editor.itsSketchPad.setDefaultFontName(font);
    }
  }

  private void FillFontMenu( JMenu theFontMenu)
  {
    String[] itsFontList = Toolkit.getDefaultToolkit().getFontList();
    JRadioButtonMenuItem item;
    itsFontMenuGroup = new ButtonGroup();

    fakeFontButton = new JRadioButtonMenuItem( "fake");
    itsFontMenuGroup.add(fakeFontButton);

    for ( int i = 0; i < itsFontList.length; i++)
      {
	item = new JRadioButtonMenuItem(itsFontList[i]);
	theFontMenu.add(item);
	itsFontMenuGroup.add(item);
	item.addActionListener( new FontMenuAdapter( item, itsFontList[i]));
      }
  }

  /** Set the font and size menu settings like this:
    If the selection is empty, set the default font.
    If the selection is a singleton, set the object font.
    Otherwise reset everything.

    For the moment, still compare the font names.
    */

  private void updateMenu()
  {
    JRadioButtonMenuItem item;
    String fontName;
    int    fontSize;

    if (ErmesSelection.patcherSelection.isEmpty())
      {
	biggerItem.setEnabled(false);
	smallerItem.setEnabled(false);

	fontName = editor.itsSketchPad.getDefaultFontName();
	fontSize = editor.itsSketchPad.getDefaultFontSize();
      }
    else if (ErmesSelection.patcherSelection.isSingleton())
      {
	biggerItem.setEnabled(true);
	smallerItem.setEnabled(true);

	GraphicObject object = ErmesSelection.patcherSelection.getSingleton();

	fontName = object.getFontName();
	fontSize = object.getFontSize();
      }
    else
      {
	biggerItem.setEnabled(true);
	smallerItem.setEnabled(true); 
	fakeFontButton.setSelected(true);
	fakeSizeButton.setSelected(true);

	return;
      }

    for( int i = 0; i < itsFontsMenu.getItemCount(); i++)
      {
	item = (JRadioButtonMenuItem)itsFontsMenu.getItem( i);

	if (item.getText().equals(fontName))
	  {
	    item.setSelected(true);
	    break;
	  }
      }

    for ( int i = 0; i < itsSizesMenu.getItemCount(); i++)
      {
	item = (JRadioButtonMenuItem)itsSizesMenu.getItem( i);

	if (Integer.parseInt(item.getText()) == fontSize)
	  {
	    item.setSelected(true);
	    break;
	  }
      }
  }
}

