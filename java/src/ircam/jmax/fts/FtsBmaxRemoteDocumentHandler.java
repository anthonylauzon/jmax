//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.fts; 

import java.io.*;

import ircam.jmax.*;
import ircam.jmax.mda.*;
import ircam.jmax.toolkit.*;
import javax.swing.*;

/** Document handler for jmax binary files. 
 * An instance of this document handler can load MaxDocument from
 * a  remote binary file.
 */

public class FtsBmaxRemoteDocumentHandler extends MaxDocumentHandler
{
  private ImageIcon jMaxDocumentIcon;

  public FtsBmaxRemoteDocumentHandler()
  {
    jMaxDocumentIcon = Icons.get("_jmax_patcher_file_");
  }

  public boolean canLoadFrom(File file)
  {
    if (super.canLoadFrom(file))
      {
	try
	  {
	    FileReader fr = new FileReader(file);
	    char buf[] = new char[4];
	    String code;

	    fr.read(buf);
	    fr.close();
	    
	    code = new String(buf);

	    if (code.equals("bMax") || code.equals("Mbxa"))
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
    FtsObject patcher;
    int id;

    if (! (context instanceof Fts))
      return null;

    Fts fts = (Fts) context;

    // Load the environment file if needed 

    MaxEnv.loadEnvFileFor(context, file);

    // ask fts to load the file 

    patcher = fts.loadJMaxFile(file);

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
    MaxContext context = document.getContext();

    if (! (context instanceof Fts))
      return;

    Fts fts = (Fts) context;

    if (document instanceof FtsPatcherDocument) 
      {
	fts.saveJMaxFile(((FtsPatcherData) document.getRootData()).getContainerObject(),
					     file);
      }
    else
      throw new MaxDocumentException("Cannot save a " + document.getDocumentType() + " as Bmax file");
  }

  protected void saveSubDocument(MaxDocument document, MaxData data, File file) throws MaxDocumentException
  {
    MaxContext context = document.getContext();

    if (! (context instanceof Fts))
      return ;

    Fts fts = (Fts) context;

    if ((document instanceof FtsPatcherDocument) && (data instanceof FtsPatcherData))
      {
	fts.saveJMaxFile(((FtsPatcherData) data).getContainerObject(), file);
      }
    else
      throw new MaxDocumentException("Cannot save a " + document.getDocumentType() + " as Bmax file");

  }

  public boolean canSaveTo(File file)
  {
    return true;
  }

  public boolean canSaveTo(MaxDocument document, File file)
  {
    return (document instanceof FtsPatcherDocument);
  }

  public String getDescription()
  {
    return "jMax Patches";
  }

  public Icon getIcon()
  {
    return jMaxDocumentIcon;
  }
}





