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

package ircam.jmax.fts;

import java.io.*;
import java.util.*;

import ircam.fts.client.*;
import ircam.jmax.*;
import ircam.jmax.editors.project.*;

public class FtsBidonMidiManager extends FtsMidiManager
{
  public FtsBidonMidiManager() throws IOException
  {
    super();
    initLabels();
  }
  void initLabels()
  {
    bidonLabels = new Vector();
    bidonLabels.add( new FtsMidiManager.MidiLabel("meo", "source1", "dest1"));
    bidonLabels.add( new FtsMidiManager.MidiLabel("cucu", "internal", "internal"));
    bidonLabels.add( new FtsMidiManager.MidiLabel("pret", "external", "external"));
    bidonLabels.add( new FtsMidiManager.MidiLabel("pio", "source3", "dest2"));
    bidonLabels.add( new FtsMidiManager.MidiLabel("frifri", "source4", "dest3"));
  }

  public String[] getSources()
  {
    return bidonSources;
  }

  public String[] getDestinations()
  {
    return bidonDestinations;
  }

  public Enumeration getLabels()
  {
    return bidonLabels.elements();
  }

  void setInput( int index, String input)
  {
    ((MidiLabel)bidonLabels.elementAt( index)).setInput( input);
  }

  void setOutput( int index, String output)
  {
    ((MidiLabel)bidonLabels.elementAt( index)).setOutput( output);
  }

  public class MidiLabel extends Object
  {
    public MidiLabel( String label, String input, String output)
    {
      this.label = label;
      this.input = input;
      this.output = output;
    }
    public void setInput( String in){ input = in;}
    public void setOutput( String out){ output = out;}
    public String label, input, output;
  }

  private String[] bidonSources = { "source1", "sourc2", "source3", "source4", "export", "internal"};
  private String[] bidonDestinations = { "dest1", "dest2", "dest3", "dest4", "export", "internal"};
  private Vector bidonLabels; 
  private FtsActionListener listener;
}



