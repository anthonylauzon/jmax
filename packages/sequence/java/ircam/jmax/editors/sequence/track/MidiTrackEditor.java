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


package ircam.jmax.editors.sequence.track;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.renderers.*;
import ircam.jmax.editors.sequence.menus.*;
import ircam.jmax.toolkit.*;
import ircam.jmax.*;
import ircam.jmax.fts.*;
import java.awt.*;
import java.beans.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;

/**
 * A view for a Midi sequence (sequence composed of events whose Value field is
 * a Midi value) */
public class MidiTrackEditor extends TrackBaseEditor
{
  public MidiTrackEditor(Geometry geometry, Track trk)
  {
    super( geometry, trk);

    if(track.getProperty("maximumPitch")==null)
      track.setProperty("maximumPitch", new Integer(AmbitusValue.DEFAULT_MAX_PITCH));
    if(track.getProperty("minimumPitch")==null)
      track.setProperty("minimumPitch", new Integer(AmbitusValue.DEFAULT_MIN_PITCH));

    viewMode = PIANOROLL_VIEW;
  }

  public void reinit()
  {
    track.setProperty("maximumPitch", new Integer(AmbitusValue.DEFAULT_MAX_PITCH));
    track.setProperty("minimumPitch", new Integer(AmbitusValue.DEFAULT_MIN_PITCH));	
    setViewMode(PIANOROLL_VIEW);		
    ((FtsTrackObject)track.getTrackDataModel()).setUntitled();
  }
  
  void createPopupMenu()
  {
    popup = new MidiTrackPopupMenu( this, gc.isInSequence());
  }

  SequenceGraphicContext createGraphicContext(Geometry geometry, Track track)
  {
    selection = new SequenceSelection(track.getTrackDataModel());
    
    //--- make this selection the current one when the track is activated
    gc = new SequenceGraphicContext(track.getTrackDataModel(), selection, this);
    gc.setGraphicSource(this);
    gc.setGraphicDestination(this);

    PartitionAdapter ad = new PartitionAdapter(geometry, gc);
    track.getPropertySupport().addPropertyChangeListener(ad);
    gc.setAdapter(ad);
    
    renderer = new ScoreRenderer(gc);

    return gc;
  }
  
  /**
   * get the lenght (in milliseconds) of the window
   */
  public int windowTimeWidth() 
  {
    return (int) (gc.getAdapter().getInvX(getSize().width) - gc.getAdapter().getInvX(ScoreBackground.KEYEND)) - 1;
  }
  
  public void setViewMode(int viewType)
  {
    super.setViewMode(viewType);
    ((ScoreRenderer)renderer).setViewMode(viewMode);
  }

  String labelType = "none";
  public void setLabelType( String type)
  {
    labelType = type;
    gc.getAdapter().LabelMapper.setLabelType( type);
    repaint();
  }

  public String getLabelType()
  {
    return labelType;
  }

  public int getDefaultHeight()
  {
    return ((PartitionAdapter)gc.getAdapter()).getRangeHeight();
  }

   //--- MidiTrack fields
  transient MaxVector oldElements = new MaxVector();
  transient SequenceTableDialog listDialog = null;
  
  public static int DEFAULT_HEIGHT = 430;
  static public final int PIANOROLL_VIEW = 0;
  static public final int NMS_VIEW = 1;
}




