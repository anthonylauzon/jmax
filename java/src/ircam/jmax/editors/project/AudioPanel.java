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

package ircam.jmax.editors.project; 

import java.util.*;
import java.io.*;
import java.awt.*;
import java.awt.event.*;  
import javax.swing.*;
import javax.swing.tree.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
import ircam.jmax.editors.project.menus.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.toolkit.menus.*;
import ircam.jmax.script.pkg.*;

public class AudioPanel extends SettingsPanel 
{
//      Assignment sampleRateLine;
//      Assignment bufferSizeLine;
//      Assignment AudioInLine;
//      Assignment AudioOutLine;
//      Assignment midiLine;

    JComboBox sampleRate;
    JComboBox bufferSize;
    JTextField audioInText;
    JTextField audioOutText;
    JTextField midiText;

    public AudioPanel(ProjectEditor editor) 
    {
	super(editor);

	setLayout(new GridLayout(0, 2));
	
	Project project = editor.getProject();

	add(new JLabel("Sample rate"));
	String sr[] = { "22050", "32000", "44100", "48000"  };
	sampleRate = new JComboBox(sr);
	sampleRate.setEditable(false);
	//sampleRate.addItemListener(this);
	add(sampleRate);

	add(new JLabel("Buffer size"));
	String bs[] = { "512", "1024", "2048", "4096" };
	bufferSize = new JComboBox(bs);
	bufferSize.setEditable(false);
	//bufferSize.addItemListener(this);
	add(bufferSize);

	add(new JLabel("Audio in device"));
	audioInText = new JTextField(project.getSetting("audio-in"), 32);
	add(audioInText);

	add(new JLabel("Audio out device"));
	audioOutText = new JTextField(project.getSetting("audio-out"), 32);
	add(audioOutText);

	add(new JLabel("Midi device"));
	midiText = new JTextField(project.getSetting("midi"), 32);
	add(midiText);
    }

    public void settingsChanged() 
    {
    }
}
