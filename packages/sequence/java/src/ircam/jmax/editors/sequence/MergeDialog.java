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


package ircam.jmax.editors.sequence;

import ircam.jmax.editors.sequence.track.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.*;

import ircam.jmax.toolkit.*;
/**
 * A dialog used to specify a set of tracks to be merged/unmerged */
public class MergeDialog extends JDialog {

    /**
     * Constructor: creates its graphic components 
     * ( a TrackRow object for each track of the model containing a checkbox;
     * the three buttons Merge - Unmerge - Cancel) */
    public MergeDialog( SequenceDataModel model, Geometry g, SequencePanel container)
    {
	super(((FtsSequenceObject)model).getEditorFrame(), "Track Merging", true);
	this.model = model;
	this.geometry = g;
	this.container = container;

	//setTitle("Track Merging");
	getContentPane().setLayout( new BoxLayout( getContentPane(), BoxLayout.Y_AXIS));
	getContentPane().add(Box.createVerticalStrut(30));

	for (int i=0; i < model.trackCount(); i++)
	    {
		TrackRow row = new TrackRow( model.getTrackAt(i), i, consistencyCheck);
		rows.addElement( row);

		getContentPane().add(row);
	    }

	mergeButton = new JButton("Merge");
	cancelButton = new JButton("Cancel");

	mergeButton.addActionListener( new ActionListener() {

		public void actionPerformed( ActionEvent e)
		{
		    int result = JOptionPane.showConfirmDialog( mergeButton,
						"Merging tracks is not Undoable.\nOK to merge ?",
						"Warning",
						JOptionPane.YES_NO_OPTION,
						JOptionPane.WARNING_MESSAGE);
		    if (result == JOptionPane.OK_OPTION)
			{
			    mergeTracks();
			    dismiss();
			}		
		    
		}
	    });

	cancelButton.addActionListener( new ActionListener() {
		public void actionPerformed(ActionEvent e) 
		{
		    dismiss();
		}
	    });

	JPanel buttonsPanel = new JPanel();
	buttonsPanel.add(mergeButton);
	buttonsPanel.add(cancelButton);

	mergeButton.setEnabled(false);

	getContentPane().add(buttonsPanel);

	pack();
	Point location = ((FtsSequenceObject)model).getEditorFrame().getLocation();
	setLocation(location.x + 200, location.y + 200);
	setVisible(true);
    }

    private void dismiss()
    {
	setVisible(false);
	dispose();
    }

    /** a CheckListener used to keep the enable state of the merge, unmerge button consistent */
    CheckListener consistencyCheck  = new CheckListener() {
	    public void checkBoxChanged(Track track)
	    {
		boolean first = true;
		boolean enableMerge;
		int numChecked = 0;
		boolean sameType = true;
		ValueInfo firstType = null;
		
		// if every selected row is a MultiSequence enable unmergeButton, otherwise disable it.
		// if there are at least two selected rows, not MultiSequence, enable
		// the mergeButton, otherwise disableIt
		for (Enumeration enum = rows.elements(); enum.hasMoreElements();)
		    {
			TrackRow row = (TrackRow) enum.nextElement();
			
			if (row.isSelected())
			    {
				if(first) 
				    {
					firstType = row.getTrack().getTrackDataModel().getType();
					first = false;
				    }
				else
				    sameType = sameType && (firstType == row.getTrack().getTrackDataModel().getType());
					
				numChecked +=1;
			    }
		    }
		
		enableMerge = (numChecked >= 2) && sameType;
		
		mergeButton.setEnabled(enableMerge);
	    }
	};

    
    
    /**
     * a class that represents a single track description with a checkbox */
    class TrackRow extends Box {

	TrackRow(Track track, int order, CheckListener cbl) 
	{
	    super(BoxLayout.X_AXIS);
	    this.track = track;
	    this.cbl = cbl;

	    selectBox = new JCheckBox();

	    add(Box.createHorizontalStrut(50));
	    String description = track.getTrackDataModel().getType().getName()+" track";

	    add(new JLabel(order+".  "+description));
	    add(Box.createHorizontalStrut(20));

	    add(Box.createHorizontalGlue());
	    add(selectBox);
	    add(Box.createHorizontalStrut(100));

	    selectBox.addActionListener( new ActionListener() {
		    public void actionPerformed( ActionEvent e)
		    {
			TrackRow.this.cbl.checkBoxChanged(TrackRow.this.track);
		    } 
		});
	}

	public boolean isSelected()
	{
	    return selectBox.isSelected();
	}

	public void setSelected(boolean b)
	{
	    selectBox.setSelected(b);
	}

	public Track getTrack()
	{
	    return track;
	}

	public void setTrack(Track track)
	{
	    this.track = track;
	}

	//--- TrackRow fields
	Track track;
	JCheckBox selectBox;
	CheckListener cbl;
    }
    
    /**
     * According to the status of the checkboxes, merge the selected tracks
     * into one  */
    void mergeTracks()
    {
	boolean first = true;
	TrackDataModel firstTrackModel = null;

	for (Enumeration enum = rows.elements(); enum.hasMoreElements();)
	    {
		TrackRow row = (TrackRow) enum.nextElement();
		if (row.isSelected())
		    {
			if(first) 
			    {
				firstTrackModel = row.getTrack().getTrackDataModel();
				container.reinitTrackEditor(row.getTrack());
				first = false;
			    }
			else
			    {				
				firstTrackModel.mergeModel(row.getTrack().getTrackDataModel());
				((FtsSequenceObject)model).requestTrackRemove(row.getTrack());
			    }
		    }
	    }	
    }

    /**
     * A convenience interface for passing data between the checkboxes and the dialog */
    interface CheckListener {
	public void checkBoxChanged(Track track);
    }

    //--- Fields
    SequenceDataModel model;
    Vector rows = new Vector();
    Geometry geometry;
    SequencePanel container;
    JButton mergeButton;
    JButton cancelButton;
}
