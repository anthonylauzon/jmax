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
// Author: Peter Hanappe
//
import java.io.*;

public class KawaMethodPrinter implements MethodPrinter {

    public void printConstructor(String f, String c, String[] a) {
	System.out.print("(define (" + f );
	int alen = a.length;
	for (int i = 0; i < alen; i++) {
	    System.out.print(" a");
	    System.out.print(i);
	}
	System.out.println(')');
	System.out.println("  ((primitive-constructor");
	System.out.println("    <" + c + '>');
	System.out.print("    (");
	if (alen > 0) {
	    for (int i = 0; i < alen - 1; i++) {
		System.out.print("<"+ a[i] + "> ");
	    }
	    System.out.print("<"+ a[alen - 1] + ">");
	}
	System.out.println("))");
	System.out.print("  ");
	for (int i = 0; i < alen; i++) {
	    System.out.print(" a");
	    System.out.print(i);
	}
	System.out.println("))");	
	System.out.println();	
    }

    public void printMethod(String f, String c, String m, String t, String r, String[] a) {
	System.out.print("(define (" + f );
	int alen = a.length;
	int numArg = (t.equals("static")) ? alen : alen + 1;
	for (int i = 0; i < numArg; i++) {
	    System.out.print(" a");
	    System.out.print(i);
	}
	System.out.println(')');
	System.out.println("  ((primitive-" + t + "-method");
	System.out.println("    <" + c + '>');
	System.out.print("    \"" + m + "\" ");
	System.out.print("<" + r + "> (");
	if (alen > 0) {
	    for (int i = 0; i < alen - 1; i++) {
		System.out.print("<"+ a[i] + "> ");
	    }
	    System.out.print("<"+ a[alen - 1] + ">");
	}
	System.out.println("))");
	System.out.print("  ");
	for (int i = 0; i < numArg; i++) {
	    System.out.print(" a");
	    System.out.print(i);
	}
	System.out.println("))");	
	System.out.println();	
    }
}


