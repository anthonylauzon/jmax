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

public class FilePanel extends SettingsPanel 
{
    JScrollPane scrollPane;
    JTree fileTree;
    DefaultTreeModel treeModel;

    public FilePanel(ProjectEditor editor) 
    {
	super(editor);

	TreeNode root = makeTree(editor.getProject());
	treeModel = new DefaultTreeModel(root);

	JTree tree = new JTree(treeModel);
	DefaultTreeCellRenderer renderer = new DefaultTreeCellRenderer();
	renderer.setOpenIcon(null);
	renderer.setClosedIcon(null);
	renderer.setLeafIcon(null);
	tree.setCellRenderer(renderer);

	scrollPane = new JScrollPane(tree);
	add(scrollPane);
    }

    public void settingsChanged() 
    {
	TreeNode root = makeTree(editor.getProject());
	treeModel = new DefaultTreeModel(root);
	treeModel.reload(root); 
	repaint();
    }

    private TreeNode makeTree(Project project) 
    {
	DefaultMutableTreeNode root = new DefaultMutableTreeNode(project.getName());

	DefaultMutableTreeNode node;
	Enumeration enum;
	SettingsTable settings;

	settings = project.getSettings();
	enum = settings.enumerateSetting("patcher");
	if (enum != null) {
	    node = new DefaultMutableTreeNode("Patchers");
	    root.add(node);
	    while (enum.hasMoreElements()) {
		String s = (String) enum.nextElement();
		File f = new File(s);
		node.add(new DefaultMutableTreeNode(f.getName()));
	    }
	}
	enum = settings.enumerateSetting("template");
	if (enum != null) {
	    node = new DefaultMutableTreeNode("Templates");
	    root.add(node);
	    while (enum.hasMoreElements()) {
		String s = (String) enum.nextElement();
		File f = new File(s);
		node.add(new DefaultMutableTreeNode(f.getName()));
	    }
	}
	enum = settings.enumerateSetting("abstraction");
	if (enum != null) {
	    node = new DefaultMutableTreeNode("Abstractions");
	    root.add(node);
	    while (enum.hasMoreElements()) {
		String s = (String) enum.nextElement();
		File f = new File(s);
		node.add(new DefaultMutableTreeNode(f.getName()));
	    }
	}
	enum = settings.enumerateSetting("data");
	if (enum != null) {
	    node = new DefaultMutableTreeNode("Data");
	    root.add(node);
	    while (enum.hasMoreElements()) {
		String s = (String) enum.nextElement();
		File f = new File(s);
		node.add(new DefaultMutableTreeNode(f.getName()));
	    }
	}
	enum = settings.enumerateSetting("script");
	if (enum != null) {
	    node = new DefaultMutableTreeNode("Scripts");
	    root.add(node);
	    while (enum.hasMoreElements()) {
		String s = (String) enum.nextElement();
		File f = new File(s);
		node.add(new DefaultMutableTreeNode(f.getName()));
	    }
	}
	return root;
    }

}
