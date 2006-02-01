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

package ircam.jmax.editors.sequence.renderers;

import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import java.io.*;
import java.util.*;
import javax.swing.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.*;

import ircam.jmax.editors.sequence.*;

public class SequenceFonts 
{
  public static void init()
  {
    if(JMaxApplication.getProperty("ftm") == null)
    {
      String root = JMaxApplication.getProperty("jmaxRoot");
      String fontPath = root + "/packages/sequence/fonts/";
				
      try
      {
        Font musicFont;
        FileInputStream fileStream;
        fileStream = new FileInputStream(fontPath+"MusiSync.ttf");
        musicFont = Font.createFont(Font.TRUETYPE_FONT, fileStream);
        originalFont = musicFont;
        fileStream = new FileInputStream(fontPath+"MusiTone.ttf");
        musicFont = Font.createFont(Font.TRUETYPE_FONT, fileStream);
      }
      catch(java.io.FileNotFoundException  e)
      {
        System.err.println("[Sequence]: no such font file "+fontPath);
      }
      catch(java.awt.FontFormatException e)
      {
        System.err.println("[Sequence]: bad font format of "+fontPath);
      }
      catch(IOException e)
      {
        System.err.println("[Sequence]: I/O error creating font "+fontPath);
      }
    }
    else
    {
      try
      {
        originalFont = JMaxUtilities.loadFontFromResource( Font.TRUETYPE_FONT, "fonts/MusiSync.ttf");
      }
      catch(java.io.FileNotFoundException  e)
      {
        System.err.println("[Sequence]: no font file MusiSync.ttf found");
      }
      catch(java.awt.FontFormatException e)
      {
        System.err.println("[Sequence]: bad font format of MusiSync.ttf");
      }
      catch(IOException e)
      {
        System.err.println("[Sequence]: I/O error creating font MusiSync.ttf");
      }
    }
  }

	public static Font getFont(int size)
  {
		Integer key = new Integer(size);
		Font font;
		if(fonts.contains(key))
			font = (Font)fonts.get(key);
		else
		{
			font = originalFont.deriveFont((float)size);
			fonts.put(key, font);
		}
		return font;
	}
	
	static Font originalFont;
	static String violinKey = "G";
	static String bassKey = "?";
	static String bemolle = "b";
	static String diesis = "B";
	static Hashtable fonts = new Hashtable();
}
