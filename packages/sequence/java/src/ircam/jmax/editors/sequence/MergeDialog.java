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

/**
 * A dialog used to specify a set of tracks to be merged/unmerged */
public class MergeDialog extends JDialog {

    /**
     * Constructor: creates its graphic components 
     * ( a TrackRow object for each track of the model containing a checkbox;
     * the three buttons Merge - Unmerge - Cancel) */
    public MergeDialog( SequenceDataModel model, Geometry g)
    {
	this.model = model;
	this.geometry = g;

	setTitle("Track Merging");
	getContentPane().setLayout( new BoxLayout( getContentPane(), BoxLayout.Y_AXIS));
	getContentPane().add(Box.createVerticalStrut(30));

	for (int i=0; i < model.trackCount(); i++)
	    {
		TrackRow row = new TrackRow( model.getTrackAt(i), i, consistencyCheck);
		rows.addElement( row);

		getContentPane().add(row);
	    }

	mergeButton = new JButton("Merge");
	unmergeButton = new JButton("Unmerge");
	cancelButton = new JButton("Cancel");

	mergeButton.addActionListener( new ActionListener() {

		public void actionPerformed( ActionEvent e)
		{
		    mergeTracks();
		    dismiss();

		}
	    });

	unmergeButton.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e)
		{
		    unmergeTracks();
		    dismiss();
		    
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
	buttonsPanel.add(unmergeButton);
	buttonsPanel.add(cancelButton);

	mergeButton.setEnabled(false);
	unmergeButton.setEnabled(false);

	getContentPane().add(buttonsPanel);

	pack();
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
		boolean enableUnmerge = true;
		boolean enableMerge = false;
		int numChecked = 0;
		int numMulti = 0;
		
		// if every selected row is a MultiSequence enable unmergeButton, otherwise disable it.
		// if there are at least two selected rows, not MultiSequence, enable
		// the mergeButton, otherwise disableIt
		for (Enumeration enum = rows.elements(); enum.hasMoreElements();)
		    {
			TrackRow row = (TrackRow) enum.nextElement();
			
			if (row.isSelected())
			    {
				numChecked +=1;

				if ( row.getTrack().getTrackDataModel().getNumTypes() == 1)
				    enableUnmerge = false;
				else numMulti += 1;

				enableMerge = true;
			    }
		    }
		
		enableUnmerge = enableUnmerge && numChecked > 0;
		enableMerge = numChecked >= 2 && numMulti == 0;
		
		mergeButton.setEnabled(enableMerge);
		unmergeButton.setEnabled(enableUnmerge);
		
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
	    String description;

	    if (track.getTrackDataModel().getNumTypes() > 1)
		{
		    description = "Merged track: ";
		    for (Enumeration enum = ((AbstractSequence) track.getTrackDataModel()).getTypes(); enum.hasMoreElements();)
			description = description + " + "+((ValueInfo) (enum.nextElement())).getName();
		}
	    else 
		description = ((ValueInfo)track.getTrackDataModel().getTypeAt(0)).getName()+" track";

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
	AbstractSequence as = new AbstractSequence();
	

	// 1. Prepare a new AbstractSequence containing all the
	// merged tracks. Remove the merged Track from
	// the SequenceDataModel 
	for (Enumeration enum = rows.elements(); enum.hasMoreElements();)
	    {
		TrackRow row = (TrackRow) enum.nextElement();
		if (row.isSelected())
		    {
			as.mergeModel(row.getTrack().getTrackDataModel());
			model.removeTrack(row.getTrack());
		    }
	    }
	
	
	// 2. Create a new Track containing the MultiSequence, add it to
	// the Sequencer. 
	
	model.addTrack( new TrackBase(as));
	
    }


    /**
     * If this method is called, every selected track is a MultiSequence.*/
    void unmergeTracks()
    {

	for (Enumeration enum = rows.elements(); enum.hasMoreElements();)
	    {
		TrackRow row = (TrackRow) enum.nextElement();
		if (row.isSelected())
		    {
			model.removeTrack(row.getTrack());

			//MultiSequence calderone = (MultiSequence)(row.getTrack().getTrackDataModel());
			AbstractSequence calderone = (AbstractSequence)(row.getTrack().getTrackDataModel());

			/** Little limitation of the Vector class:
			 * can't remove elements from a Vector in a loop based
			 * on the vector's enumeration. We make a copy of it before. */
			Vector types = new Vector();
			for (Enumeration t = calderone.getTypes(); t.hasMoreElements();)
			    types.addElement(t.nextElement());


			for (Enumeration e = types.elements(); e.hasMoreElements();)
			    {
				ValueInfo info = (ValueInfo) e.nextElement();
				TrackDataModel trackmodel = new AbstractSequence(model, info);
				calderone.unmergeModel(trackmodel, info);

				model.addTrack(new TrackBase(trackmodel)/*info.newTrackInstance(trackmodel)*/);
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
    JButton mergeButton;
    JButton unmergeButton;
    JButton cancelButton;
}
