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


package ircam.jmax.editors.mat;

import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;

import java.util.Enumeration;
import java.awt.*;

public interface MatDataModel {

    public abstract int getSize();
 
    public abstract int getRows();
    
    public abstract String getColumnName(int col_id);
    
    public abstract String getType();
    
    public abstract String getName();
    
    public abstract boolean haveRowIdCol();
    
    public abstract boolean canAppendColumn();
    
    public abstract int getColumns();
    
    public abstract void setSize(int m, int n);
    
    public abstract Dimension getDefaultSize();
    
    public abstract void setRows(int m);
    
    public abstract void setColumns(int n);
        
    public abstract Object getValueAt(int m, int n);
    
    public abstract void setValueAt(int m, int n, Object value);
    
    public abstract void addMatListener(MatDataListener theListener);
    public abstract void removeMatListener(MatDataListener theListener);    
    
    public abstract void requestSetValue( java.lang.Object aValue, int rowIndex, int columnIndex);
    public abstract void requestAppendRow();
    public abstract void requestInsertRow(int index);
    public abstract void requestAppendColumn();
    public abstract void requestInsertColumn(int index);
    public abstract void requestDeleteRows(int startIndex, int size);
}









