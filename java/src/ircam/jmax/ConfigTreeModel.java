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
// Authors: Francois Dechelle, Norbert Schnell.
// 

package ircam.jmax;

import javax.swing.*;
import javax.swing.tree.*;

import java.util.*;

import ircam.jmax.toolkit.*;
import ircam.jmax.fts.*;
import ircam.fts.client.*;

/**
 * A table model used to represent the content of an objectset of error objects
 * in a JTable. 
 */
class ConfigTreeModel extends DefaultTreeModel 
{
  
  ConfigTreeModel(String[] rootNodes)
  {
    super( new DefaultMutableTreeNode("root"));
    
    DefaultMutableTreeNode node;
    for(int i = 0; i < rootNodes.length; i++)
      {
	node = new DefaultMutableTreeNode( rootNodes[i]);
	((DefaultMutableTreeNode)getRoot()).add( node);
	rootsTable.put(rootNodes[i], node);
      }
  }

  public void setFtsPackage(FtsPackage pkg)
  {
    ftsPackage = pkg;
  }

  public void addRow( String rootName)
  {
    ((DefaultMutableTreeNode)rootsTable.get( rootName)).add( new DefaultMutableTreeNode());    
  }

  public void addRow(String rootName, String value)
  {
    DefaultMutableTreeNode newNode = new DefaultMutableTreeNode( value);
    ((DefaultMutableTreeNode)rootsTable.get( rootName)).add( newNode);
  }

  /*public void removeRow(int rowId)
    {
    if(size > 0)
    {
    size--;    
    if(rowId >= 0)
    {
    for(int i = rowId; i < size; i++)
    {
    data[i][0] = data[i+1][0];
    data[i][1] = data[i+1][1];
    }
    }
    data[size][0] = null;
    data[size][1] = null;
    
    fireTableDataChanged();
    }
    }*/

  public void save()
  {
    if( ftsPackage != null)
      ftsPackage.save();
  }

  /*public Object getValueAt(int row, int col) 
    { 
    if(row > size) return null;
    else
    return data[row][col];
    }
    public void setValueAt(Object value, int row, int col) 
    {
    if(row > size) return;
    
    data[row][col] = value;
    fireTableCellUpdated(row, col);
    
    if( ftsPackage != null)
    {
    if(( col==1) && ( data[row][0] != null) && ( data[row][1] != null))
    ftsPackage.set( (String)data[row][0], (String)data[row][1]);
    }
    }*/

  FtsPackage ftsPackage;
  private Hashtable rootsTable = new Hashtable();
}





