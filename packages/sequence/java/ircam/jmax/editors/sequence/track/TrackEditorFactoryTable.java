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
import java.util.*;
import ircam.jmax.toolkit.*;
/**
* A class used to register new TrackEditorFactories */

public class TrackEditorFactoryTable {
	
public static void setFactoryFor(ValueInfo info, TrackEditorFactory factory)
{
    if(!infos.contains(info))
		{
			factories.put(info.getName(), factory);
			infos.addElement(info);
		}
}

public static TrackEditor newEditor(Track track, Geometry geometry, boolean isInSequence)
{
	TrackEditorFactory tef = (TrackEditorFactory) factories.get(track.getTrackDataModel().getType().getName());
	if(tef!=null)
		return tef.newEditor(track, geometry, isInSequence);
	else
		return new AnythingTrackEditor(geometry, track, isInSequence);
}

public static Enumeration getTypes()
{
	return infos.elements();
}

public static void init()
{
	setFactoryFor(AmbitusValue.info, MidiTrackEditorFactory.instance);
	setFactoryFor(IntegerValue.info, IntegerTrackEditorFactory.instance);
	setFactoryFor(FloatValue.info, FloatTrackEditorFactory.instance);
  setFactoryFor(FmatValue.info, FmatTrackEditorFactory.instance);
	//setFactoryFor(MessageValue.info, MessageTrackEditorFactory.instance);
	//setFactoryFor(MidiValue.info, IntegerTrackEditorFactory.instance);
}
//---
private static Hashtable factories = new Hashtable();
private static Vector infos = new Vector();
}





