//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.sequence.menus;

import java.util.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.datatransfer.*;

import javax.swing.*;
import javax.swing.event.*;

import ircam.jmax.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.actions.*;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.actions.*;
import ircam.jmax.editors.sequence.track.*;


public class TrackBasePopupMenu extends JPopupMenu 
{
  int x;
  int y;
  TrackBaseEditor target = null;    
  boolean isInSequence = false;
  JMenuItem removeItem, nameItem;
  JMenu moveMenu;
	JCheckBoxMenuItem saveItem;
  int trackCount = 0;
	
  JLabel maxLabel, minLabel;
  JSlider maxSlider, minSlider;
  Box maxBox, minBox;
  MoveTrackToAction moveToAction;
  public EditorAction cutAction, copyAction, duplicateAction;
	
  public TrackBasePopupMenu( TrackBaseEditor editor, boolean isInSequence)
  {
    super();
    JMenuItem item;
		
    target = editor;
    this.isInSequence = isInSequence;
		
    addViewMenu();
		
    item = new JMenuItem("View as list");
    item.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent e)
		  {
				target.showListDialog();
		  }
		});
    add(item);
		
    addSeparator();
		
    if( addRangeMenu())
      addSeparator();
		
    item = new JMenuItem("Select All");
    item.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent e)
		  {
				target.getSelection().selectAll();
				target.getGraphicContext().getGraphicDestination().requestFocus();
			}
    });
    add(item);
		
    if(isInSequence)
		{
			addSeparator();
			
			nameItem = new JMenuItem("Name");
			nameItem.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e)
	      {
					ChangeTrackNameDialog.changeName(target.getTrack(),  
																					 target.getGraphicContext().getFrame(),
																					 SwingUtilities.convertPoint(target, x, y,
																																			 target.getGraphicContext().getFrame()));
				}
			});
			
			add(nameItem);
			
			moveMenu = new JMenu("Move to Position");
			item = new JMenuItem(""+trackCount);
			item.addActionListener( moveToAction);
			moveMenu.add(item);
			
			add(moveMenu);
			
			addSeparator();
			
			addAddTrackMenu();
			
			removeItem = new JMenuItem("Remove Track");
			removeItem.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e)
	    {
					((FtsSequenceObject)target.getGraphicContext().getFtsObject()).
					removeTrack( target.getTrack());
	    }
			});
			add(removeItem);
		}
		
    if(JMaxApplication.getProperty("no_menus") != null)
    {
      addSeparator();
			
      add( Actions.undoAction);
      add( Actions.redoAction);
			
      addSeparator();
			
      cutAction = new Actions.CutAction();
      copyAction = new Actions.CopyAction();
      duplicateAction = new Actions.DuplicateAction();
      add(cutAction);
      add(copyAction);
      add(Actions.pasteAction);
      add(duplicateAction);
    }
		
    addSeparator();
    item = new JMenuItem("Export Track");
    item.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent e)
		  {
				target.getTrack().getFtsTrack().export();
		  }
    });
    add(item);
		
		if(!isInSequence)
		{
			saveItem = new JCheckBoxMenuItem("Save Editor State");
			saveItem.addActionListener(new ActionListener(){
				public void actionPerformed(ActionEvent e)
		    {
					target.getTrack().getFtsTrack().requestSetSaveEditor(saveItem.getState());
				}
			});
			add(saveItem);    
		}
    moveToAction = new MoveTrackToAction(target);
		
    pack();
  }
	
  void addAddTrackMenu()
  {
    JMenu menu = new JMenu("Add Track");
    FtsSequenceObject seq = (FtsSequenceObject)target.getGraphicContext().getFtsObject();
    for(Enumeration e = TrackEditorFactoryTable.getTypes(); e.hasMoreElements();)
      menu.add( new AddTrackAction((ValueInfo)e.nextElement(), seq));
    add(menu);
  }
	
  boolean addRangeMenu()
  {
    return false;
  }
  boolean addViewMenu()
  {
    return false;
  }
  void updateViewMenu(){}
	
  public TrackBaseEditor getPopupTarget(){
    return target;
  }
	
  public void update()
  {
    if(isInSequence)
      updateMoveToMenu();
			
	    updateViewMenu();
		
    if(JMaxApplication.getProperty("no_menus") != null)
      updateCutCopyPaste();
	
		if(!isInSequence)
		{
			boolean saveEditor = target.getTrack().getFtsTrack().saveEditor;
			if(saveItem.getState() != saveEditor)
				saveItem.setState(saveEditor);			
		}		
	}
	
  void updateMoveToMenu()
  {
    JMenuItem item;
    int count =  target.trackCount()-1;
    if(trackCount==count)
      return;
    else
		{
			int dif = count-trackCount;
			
			if(dif>0)
				for(int i=1; i<=dif; i++)
				{
					item = new JMenuItem(""+(trackCount+i));
					item.addActionListener( moveToAction);
					moveMenu.add(item);			
				}		
					else
						for(int i=0; i<-dif; i++)
							moveMenu.remove(moveMenu.getItemCount()-1);
			trackCount = count;
		}
  }
	
  void updateCutCopyPaste()
  {
    if((SequenceSelection.getCurrent() == null)||(SequenceSelection.getCurrent().isSelectionEmpty()))
    {
      //Empty selection
      cutAction.setEnabled(false);
      copyAction.setEnabled(false);
      duplicateAction.setEnabled(false);
    }
    else
    {
      // Object selection
      cutAction.setEnabled(true);
      copyAction.setEnabled(true);
      duplicateAction.setEnabled(true);
    }
  }
  
  class SetViewAction extends AbstractAction {
    SetViewAction(int viewType, TrackBaseEditor editor)
	{
      super("Set View");
      this.viewType = viewType;
      this.editor = editor;
	}
    
    public void actionPerformed(ActionEvent e)
	{
      editor.setViewMode(viewType);
	}
		
    int viewType;
    TrackBaseEditor editor;
  }  
	
  public void show(Component invoker, int x, int y)
  {
    this.x = x;
    this.y = y;
    
    super.show(invoker, x, y);
  }
}











