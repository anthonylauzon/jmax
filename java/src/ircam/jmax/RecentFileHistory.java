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
// Authors: Maurizio Umberto Puxeddu.
// 

package ircam.jmax;

import javax.swing.event.ListDataListener;
import javax.swing.DefaultListModel;
import java.io.*;

public class RecentFileHistory extends DefaultListModel
{
    int max_size;

    public RecentFileHistory(int max_size0)
    {
	max_size = max_size0;

	load();
    }

    public void load()
    {
	File file = history_file_name();
	File recentFile;

	if (!file.exists())
	    return;

	if (!file.canRead())
	    {
		System.out.println("history file name '" + file + "' has wrong permissions");
		return;
	    }

	try
	    {
		FileReader fileWriter = new FileReader(file.toString());
		BufferedReader bufferedReader = new BufferedReader(fileWriter);
		String line;

		while ((line = bufferedReader.readLine()) != null)
		    {
			recentFile = new File(line);
			if (recentFile.exists() && recentFile.canRead())
			    addFile(recentFile);
		    }
		bufferedReader.close();
	    }
	catch (IOException e)
	    {
		System.out.println("I/O error reading recent file history '" + file + "':\n" + e);
	    }
    }

    public void save()
    {
	File file = history_file_name();
	
	try
	    {
		FileWriter fileWriter = new FileWriter(file.toString());
		BufferedWriter bufferedWriter = new BufferedWriter(fileWriter);

		// Save in reverse order
		for (int i = size() - 1; i >= 0; i--)
		    {
			bufferedWriter.write(((File)get(i)).toString());
			bufferedWriter.newLine();
		    }
		bufferedWriter.close();
	    }
	catch (IOException e)
	    {
		System.out.println("I/O error writing recent file history '" + file + "':\n" + e);
	    }
    }

    private File history_file_name()
    {
	return  new File(System.getProperty("user.home"), ".jmax_history");
    }

    public void addFile(File file)
    {
	for (int i = 0; i < size(); ++i)
	    if (((File)get(i)).compareTo(file) == 0)
		return;

	add(0, file);
	if (size() > max_size)
	    removeRange(max_size, size() - 1);
    }
}
