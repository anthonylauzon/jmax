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
import ircam.jmax.editors.patcher.actions.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;

/** Implement the patcher editor File Menu */

public class TextMenu extends EditorMenu
{
  JMenuItem biggerItem;
  JMenuItem smallerItem;
  JMenuItem fitItem;
  JRadioButtonMenuItem automaticFitItem; 
  private JMenu itsSizesMenu;
  private ButtonGroup itsSizesMenuGroup;
  JRadioButtonMenuItem fakeSizeButton;

  private JMenu itsFontsMenu;
  private ButtonGroup itsFontMenuGroup;
  JRadioButtonMenuItem fakeFontButton;

  private JMenu itsStylesMenu;
  private ButtonGroup itsStylesMenuGroup;
  JRadioButtonMenuItem fakeStylesButton;

  ErmesSketchPad sketch;

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

  public TextMenu(ErmesSketchPad sketch)
  {
    super("Text");
    setHorizontalTextPosition(AbstractButton.LEFT);

    JMenuItem item;
    this.sketch = sketch;

    biggerItem  = add(Actions.fontBiggerAction, "Bigger", Event.CTRL_MASK, KeyEvent.VK_ADD);
    smallerItem = add(Actions.fontSmallerAction, "Smaller", Event.CTRL_MASK, KeyEvent.VK_SUBTRACT);

    addSeparator();

    /*****************/
    //jdk117-->jdk1.3//
    automaticFitItem = new JRadioButtonMenuItem("Automatic Fit To Text");
    //automaticFitItem = new AntialiasingRadioButtonMenuItem("Automatic Fit To Text");
    /*****************/

    add(automaticFitItem);
    automaticFitItem.addActionListener(Actions.setAutomaticFitAction);

    fitItem = add(Actions.fitToTextAction, "Fit To Text", Event.CTRL_MASK, KeyEvent.VK_T);

    addSeparator();

    /*****************/
    //jdk117-->jdk1.3//
    itsSizesMenu = new JMenu("Sizes");
    //itsSizesMenu = new AntialiasingMenu("Sizes");
    /*****************/

    FillSizesMenu( itsSizesMenu);    
    add(itsSizesMenu);

    /*****************/
    //jdk117-->jdk1.3//
    itsFontsMenu = new JMenu("Fonts");
    //itsFontsMenu = new AntialiasingMenu("Fonts");
    /*****************/

    FillFontMenu(itsFontsMenu);
    add(itsFontsMenu);

    /*****************/
    //jdk117-->jdk1.3//
    itsStylesMenu = new JMenu("Styles");
    //itsStylesMenu = new AntialiasingMenu("Styles");
    /*****************/

    FillStylesMenu(itsStylesMenu);
    add(itsStylesMenu);

    addMenuListener(new TextMenuListener());
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

	  /*****************/
	  //jdk117-->jdk1.3//
	  item = new JRadioButtonMenuItem(Integer.toString(sizes[i]));
	  //item = new AntialiasingRadioButtonMenuItem(Integer.toString(sizes[i]));
	  /*****************/
	  menu.add(item);
	  item.addActionListener(Actions.fontSizesAction);
	  itsSizesMenuGroup.add(item);
      }
  }

  private void FillStylesMenu( JMenu menu)
  {
    String styles[] = {"plain", "bold", "italic"};
    JRadioButtonMenuItem item; 
    itsStylesMenuGroup = new ButtonGroup();

    fakeStylesButton = new JRadioButtonMenuItem( "fake");
    itsSizesMenuGroup.add(fakeSizeButton);

    for (int i = 0; i < styles.length; i++)
      {
	  /*****************/
	  //jdk117-->jdk1.3//
	  item = new JRadioButtonMenuItem(styles[i]);
	  //item = new AntialiasingRadioButtonMenuItem(styles[i]);
	  /*****************/
	  menu.add(item);
	  item.addActionListener(Actions.fontStylesAction);
	  itsStylesMenuGroup.add(item);
      }
  }
  private void FillFontMenu( JMenu theFontMenu)
  {
      /*****************/
      //jdk117-->jdk1.3//
      String[] itsFontList = Toolkit.getDefaultToolkit().getFontList();
      //String[] itsFontList = GraphicsEnvironment.getLocalGraphicsEnvironment().getAvailableFontFamilyNames();
      /*****************/

      JRadioButtonMenuItem item;
      itsFontMenuGroup = new ButtonGroup();

      fakeFontButton = new JRadioButtonMenuItem( "fake");
      itsFontMenuGroup.add(fakeFontButton);

      for ( int i = 0; i < itsFontList.length; i++)
	  {
	      /*****************/
	      //jdk117-->jdk1.3//
	      item = new JRadioButtonMenuItem(itsFontList[i]);
	      //item = new AntialiasingRadioButtonMenuItem(itsFontList[i]);	      
	      /*****************/
	      theFontMenu.add(item);
	      itsFontMenuGroup.add(item);
	      item.addActionListener(Actions.fontAction);
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
    int    fontStyle;

    if (ErmesSelection.patcherSelection.isEmpty())
      {
	biggerItem.setEnabled(false);
	smallerItem.setEnabled(false);
	fitItem.setEnabled(false);

	fontName = sketch.getDefaultFontName();
	fontSize = sketch.getDefaultFontSize();
	fontStyle = sketch.getDefaultFontStyle();
      }
    else if (ErmesSelection.patcherSelection.isSingleton())
      {
	biggerItem.setEnabled(true);
	smallerItem.setEnabled(true);
	fitItem.setEnabled(true);
	
	GraphicObject object = ErmesSelection.patcherSelection.getSingleton();

	fontName = object.getFontName();
	fontSize = object.getFontSize();
	fontStyle = object.getFontStyle();
      }
    else
      {
	biggerItem.setEnabled(true);
	smallerItem.setEnabled(true); 
	fitItem.setEnabled(true); 
	fakeFontButton.setSelected(true);
	fakeSizeButton.setSelected(true);
	fakeStylesButton.setSelected(true);

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
    boolean sizeExist = false;
    for ( int i = 0; i < itsSizesMenu.getItemCount(); i++)
      {
	item = (JRadioButtonMenuItem)itsSizesMenu.getItem( i);

	if (Integer.parseInt(item.getText()) == fontSize)
	  {
	    item.setSelected(true);
	    sizeExist = true;
	    break;
	  }
      }
    
    if(!sizeExist) fakeSizeButton.setSelected(true);
  
    switch(fontStyle)
	{
	case Font.PLAIN:
	    itsStylesMenu.getItem(0).setSelected(true);
	    break;
	case Font.BOLD:
	    itsStylesMenu.getItem(1).setSelected(true);
	    break;
	case Font.ITALIC:
	    itsStylesMenu.getItem(2).setSelected(true);
	    break;
	default:
	    itsStylesMenu.getItem(0).setSelected(true);
	}
  }
}









