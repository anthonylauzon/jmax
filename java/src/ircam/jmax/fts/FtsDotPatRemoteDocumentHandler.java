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

package ircam.jmax.fts; 

import java.io.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.toolkit.*;
import javax.swing.*;

/** Document handler for .pat files.
 * An instance of this document handler can load MaxDocument from
 * .pat Max 0.26 file.
 * Ask FTS to load the file.
 */

public class FtsDotPatRemoteDocumentHandler extends MaxDocumentHandler
{
  private ImageIcon patDocumentIcon;

  public FtsDotPatRemoteDocumentHandler()
  {
    patDocumentIcon = Icons.get("_max_patcher_file_");
  }

  public boolean canLoadFrom(File file)
  {
    if (super.canLoadFrom(file))
      {
	try
	  {
	    FileReader fr = new FileReader(file);

	    char buf[] = new char[6];
    
	    fr.read(buf);
	    fr.close();

	    if ((new String(buf)).equals("max v2"))
	      return true;
	    else
	      return false;
	  }
	catch (FileNotFoundException e)
	  {
	    return false;
	  }
	catch (IOException e)
	  {
	    return false;
	  }
      }
    else
      return false;
  }

  /** Make the real document */

  protected MaxDocument loadDocument(MaxContext context, File file)
  {
    FtsServer server;
    FtsObject patcher;
    int id;

    if (! (context instanceof Fts))
      return null;

    Fts fts = (Fts) context;

    // Load the environment file if needed 

    MaxEnv.loadEnvFileFor(context, file);

    // ask fts to load the file 

    patcher = fts.loadPatFile(file);

    if (patcher != null)
      {
	FtsPatcherDocument obj = new FtsPatcherDocument(context);

	// Temporary hack to force the patcher uploading; really, MDA should allow for 
	// async edit of documents ...

	patcher.updateData();
	fts.sync();
	obj.setRootData((MaxData) patcher.getData());
	obj.setDocumentFile(file);
	obj.setDocumentHandler(this);
	    
	return obj;
      }
    else
      return null;
  }

  public void saveDocument(MaxDocument document, File file) throws MaxDocumentException
  {
    throw new MaxDocumentException("Cannot save in .pat file format");
  }

  // Overwrite upper class method; we cannot save to a .pat file.

  public boolean canSaveTo(File file)
  {
    return false;
  }

  public boolean canSaveTo(MaxDocument document, File file)
  {
    return false;
  }

  public String getDescription()
  {
    return "Max Patches";
  }

  public Icon getIcon()
  {
    return patDocumentIcon;
  }
}





