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
import ircam.jmax.editors.patcher.objects.*;

/** Implement the patcher editor File Menu */

class TextMenu extends JMenu
{
  private JMenu itsSizesMenu;
  private ButtonGroup itsSizesMenuGroup;

  private JMenu itsFontsMenu;
  private ButtonGroup itsFontMenuGroup;

  JRadioButtonMenuItem itsSelectedSizeItem;
  JRadioButtonMenuItem itsSelectedFontItem;

  ErmesSketchWindow editor;

  TextMenu(ErmesSketchWindow window)
  {
    super("Text");

    JMenuItem item;
    editor = window;

    itsSizesMenu = new JMenu( "Sizes");
    FillSizesMenu( itsSizesMenu);
    add( itsSizesMenu);

    itsFontsMenu =  new JMenu( "Fonts");
    FillFontMenu( itsFontsMenu);
    add( itsFontsMenu);

    selectionChanged();
  }

  class SizesMenuAdapter implements ChangeListener
  {
    JRadioButtonMenuItem item;
    int size;

    SizesMenuAdapter( JRadioButtonMenuItem item, int size)
    {
      this.item = item;
      this.size = size;
    }

    public  void stateChanged(ChangeEvent ev)
    {
      if (item.isSelected())
	  {
	    itsSelectedSizeItem = item;

	    //if we are here, a font size have been choosen from the FONT menu

	    if (ErmesSelection.patcherSelection.hasObjectsIn(editor.itsSketchPad))
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
  }

  private void FillSizesMenu( JMenu menu)
  {
    int sizes[] = {8, 9, 10, 12, 14, 18, 24, 36, 48};
    JRadioButtonMenuItem item; 
    itsSizesMenuGroup = new ButtonGroup();

    for (int i = 0; i < sizes.length; i++)
      {
	item = new JRadioButtonMenuItem( Integer.toString(sizes[i]));
	menu.add( item);
	item.addChangeListener( new SizesMenuAdapter( item, sizes[i]));
	itsSizesMenuGroup.add(item);
      }
  }


  class FontMenuAdapter implements ChangeListener
  {
    JRadioButtonMenuItem item;
    String font;

    FontMenuAdapter( JRadioButtonMenuItem item, String font)
    {
      this.item = item;
      this.font = font;
    }

    public  void stateChanged( ChangeEvent ev)
    {
      if (item.isSelected())
	{
	  itsSelectedFontItem = item;

	  if (ErmesSelection.patcherSelection.hasObjectsIn(editor.itsSketchPad))
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
  }

  private void FillFontMenu( JMenu theFontMenu)
  {
    String[] itsFontList = Toolkit.getDefaultToolkit().getFontList();
    JRadioButtonMenuItem item;
    itsFontMenuGroup = new ButtonGroup();

    for ( int i = 0; i < itsFontList.length; i++)
      {
	item = new JRadioButtonMenuItem(itsFontList[i]);
	theFontMenu.add(item);
	itsFontMenuGroup.add(item);
	item.addChangeListener( new FontMenuAdapter( item, itsFontList[i]));
      }
  }

  /** Set the font and size menu settings like this:
    If the selection is empty, set the default font.
    If the selection is a singleton, set the object font.
    Otherwise reset everything.

    For the moment, still compare the font names.
    */

  void selectionChanged()
  {
    JRadioButtonMenuItem item;
    String fontName;
    int    fontSize;

    if (ErmesSelection.patcherSelection.isEmpty())
      {
	fontName = editor.itsSketchPad.getDefaultFontName();
	fontSize = editor.itsSketchPad.getDefaultFontSize();
      }
    else if (ErmesSelection.patcherSelection.isSingleton())
      {
	ErmesObject object = ErmesSelection.patcherSelection.getSingleton();

	fontName = object.getFontName();
	fontSize = object.getFontSize();
      }
    else
      {
	itsSelectedSizeItem.setSelected(false);
	itsSelectedFontItem.setSelected(false);
	return;
      }

    for( int i = 0; i < itsFontsMenu.getItemCount(); i++)
      {
	item = (JRadioButtonMenuItem)itsFontsMenu.getItem( i);

	if (item.getLabel().toLowerCase().equals(fontName))
	  {
	    item.setSelected(true);
	    itsSelectedFontItem = item;
	    break;
	  }
      }

    for ( int i = 0; i < itsSizesMenu.getItemCount(); i++)
      {
	item = (JRadioButtonMenuItem)itsSizesMenu.getItem( i);

	if (Integer.parseInt(item.getLabel()) == fontSize)
	  {
	    item.setSelected(true);
	    itsSelectedSizeItem = item;
	    break;
	  }
      }
  }
}
