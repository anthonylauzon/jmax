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

package ircam.jmax.editors.qlist;

import java.util.*;
import java.text.*;

import ircam.jmax.*;
import ircam.fts.client.*;

public class QListUnparse
{
  final static int lex_float_start = 0;
  final static int lex_float_in_value = 1;
  final static int lex_float_in_sign = 2;
  final static int lex_float_after_point = 3;
  final static int lex_float_end = 4;

  /* To unparse floating point numbers */

  static private DecimalFormat formatter;

  static
  {
    // Number format for messages coming from FTS (to be cleaned up:
    // the text should be sent by FTS as text alread).
    formatter = new DecimalFormat("0.######;-0.######");
    formatter.setGroupingUsed(false);
    formatter.setDecimalSeparatorAlwaysShown(true);

    DecimalFormatSymbols formsym = new DecimalFormatSymbols();
    formsym.setDecimalSeparator('.');
    formatter.setDecimalFormatSymbols(formsym);
  }

  /** Identify digits */

  static final private boolean isDigit(char c)
  {
    return ((c == '0') || (c == '1') || (c == '2') || (c == '3') || (c == '4') ||
	    (c == '5') || (c == '6') || (c == '7') || (c == '8') || (c == '9'));
  }

  /** identify  the sign char */

  static final private boolean isSign(char c)
  {
    return ((c == '-') || (c == '+'));
  }

  /** Identify decimal point, and so a float representation */

  static final private boolean isDecimalPoint(char c)
  {
    return (c == '.');
  }

  // Unparser.
  // Functions to classify values and tokens.

  static private final boolean wantASpaceBefore(Object value)
  {
    if (value instanceof String)
      {
	String keywords[] = {"+", "-", "*", "/", "%", 
			     "&&", "&", "||", "|", "==", "=", "!=", "!", ">=", "^",
			     ">>", ">", "<<", "<=", "<", "?", "::", ":" };

	for (int i = 0 ; i < keywords.length; i++)
	  if (keywords[i].equals((String) value))
	    return true;

	return false;
      }
    else
      return false;
  }
      
  static private final boolean wantASpaceBefore(FtsAtom value)
  {
    if (value.isString() || value.isSymbol())
      {
	String stringVal = (value.isString()) ? value.stringValue : value.symbolValue.toString();
	String keywords[] = {"+", "-", "*", "/", "%", 
			     "&&", "&", "||", "|", "==", "!=", "!", ">=", "^",
			     ">>", ">", "<<", "<=", "<", "?", "::", ":" };

	for (int i = 0 ; i < keywords.length; i++)
	  if (keywords[i].equals( stringVal))
	    return true;

	return false;
      }
    else
      return false;
  }
      

  static private final boolean dontWantASpaceBefore(Object value)
  {
    if (value instanceof String)
      {
	String keywords[] = {")", "[", "]", "}", ",", ";", ".", "="};


	for (int i = 0 ; i < keywords.length; i++)
	  if (keywords[i].equals((String) value))
	    return true;

	return false;
      }
    else
      return false;
  }
      
  static private final boolean dontWantASpaceBefore(FtsAtom value)
  {
    if (value.isString() || value.isSymbol())
      {
	String stringVal = (value.isString()) ? value.stringValue : value.symbolValue.toString();
	String keywords[] = {")", "[", "]", "}", ",", ";", ".", "="};


	for (int i = 0 ; i < keywords.length; i++)
	  if (keywords[i].equals( stringVal))
	    return true;

	return false;
      }
    else
      return false;
  }
      
  static private final boolean wantASpaceAfter(Object value)
  {
    if (value instanceof String)
      {
	String keywords[] = { "+", "-", "*", "/", "%", 
			      ",", "&&", "&", "||", "|", "==", "=", "!=", "!", ">=",
			      ">>", ">", "<<", "<=", "<", "?", "::", ":", "^",
			      ";" };

	for (int i = 0 ; i < keywords.length; i++)
	  if (keywords[i].equals((String) value))
	    return true;

	return false;
      }
    else
      return false;
  }

  static private final boolean wantASpaceAfter(FtsAtom value)
  {
    if (value.isString() || value.isSymbol())
      {
	String stringVal = (value.isString()) ? value.stringValue : value.symbolValue.toString();
	String keywords[] = { "+", "-", "*", "/", "%", 
			      ",", "&&", "&", "||", "|", "==", "!=", "!", ">=",
			      ">>", ">", "<<", "<=", "<", "?", "::", ":", "^",
			      ";" };

	for (int i = 0 ; i < keywords.length; i++)
	  if (keywords[i].equals( stringVal))
	    return true;

	return false;
      }
    else
      return false;
  }

  static private final boolean dontWantASpaceAfter(Object value)
  {
    if (value instanceof String)
      {
	String keywords[] = { "(", "[", "{", 
			      "$", "'", "." , "=" };

	for (int i = 0 ; i < keywords.length; i++)
	  if (keywords[i].equals((String) value))
	    return true;

	return false;
      }
    else
      return false;
  }
      
  static private final boolean dontWantASpaceAfter(FtsAtom value)
  {
    if (value.isString() || value.isSymbol())
      {
	String stringVal = (value.isString()) ? value.stringValue : value.symbolValue.toString();	
	String keywords[] = { "(", "[", "{", 
			      "$", "'", ".", "=" };

	for (int i = 0 ; i < keywords.length; i++)
	  if (keywords[i].equals(stringVal))
	    return true;

	return false;
      }
    else
      return false;
  }
      
  static private final boolean isAKeyword(String value)
  {
    String keywords[] = { "(", ")",
			  "[", "]", "{", "}", ",", 
			  "::", ":", "$", ".",
			  ";", "'",
			  "==", "!=", "<=", ">=", "="};
    
    for (int i = 0 ; i < keywords.length; i++)
      if (keywords[i].equals((String) value))
	return true;

    return false;
  }
      
  static private final boolean isAnInt(String value)
  {
    int i = 0;

    if (isSign(value.charAt(i)))
      if (value.length() == 1)
	return false;
      else
	i++;

    while (i < value.length())
      if (! isDigit(value.charAt(i++)))
	return false;

    return true;
  }

  static private final boolean isAFloat(String value)
  {
    int status;
    int i = 0;

    status = lex_float_start;

    while (status != lex_float_end)
      {
	switch (status)
	  {
	  case lex_float_start:
	    if (i >= value.length())
	      return false;
	    else if (isDigit(value.charAt(i)))
	      status = lex_float_in_value;
	    else if (isSign(value.charAt(i)))
	      status = lex_float_in_sign;
	    else if (isDecimalPoint(value.charAt(i)))
	      status = lex_float_after_point;
	    else
	      return false;
	    break;

	  case lex_float_in_sign:

	    if (i >= value.length())
	      return false;
	    else if (isDigit(value.charAt(i)))
	      status = lex_float_in_value;
	    else if (isDecimalPoint(value.charAt(i)))
	      status = lex_float_after_point;
	    else
	      return false;
	    break;

	  case lex_float_in_value:

	    if (i >= value.length())
	      return false;
	    else if (isDigit(value.charAt(i)))
	      status = lex_float_in_value;
	    else if (isDecimalPoint(value.charAt(i)))
	      status = lex_float_after_point;
	    else
	      return false;
	    break;

	  case lex_float_after_point:

	    if (i >= value.length())
	      return true;
	    else if (isDigit(value.charAt(i)))
	      status = lex_float_after_point;
	    else
	      return false;
	    break;
	  }

	i++;
      }


    return true;
  }

  static final private boolean includeStartToken(String s)
  {
    char chars[] = { '$', ',', '(', ')', '[', ']',
		     '{', '}', 
		     ':',
		     ';', '\'', '\t', ' ' };

    for (int i = 0 ; i < chars.length; i++)
      if (s.indexOf(chars[i]) != -1)
	return true;
    
    return false;
  }

  /** Unparse an object description from a FTS message.  */
  static String removeZeroAtEnd(String buff){
    while(buff.endsWith("0")){
      buff = buff.substring(0, buff.length()-1);
    }
    return buff;
  }

  /*  Unparse a description passed as a vector of values
      Used by atom list, available as a service for anybody. */
  public static String unparseDescription(MaxVector values)
  {
    boolean doNewLine = false;
    boolean addBlank = false;
    boolean noNewLine = false;
    Object value1 = null;
    Object value2 = null;

    Enumeration en = values.elements();
    StringBuffer descr = new StringBuffer();

    if ( ! en.hasMoreElements())
      return "";

    value2 = en.nextElement();
    value1 = value2;

    while (value1 != null)
      {
	if (doNewLine)
	  descr.append("\n");
	else if (addBlank)
	  descr.append(" ");

	doNewLine = false;

	if (! en.hasMoreElements())
	  value2 = null;
	else
	  value2 = en.nextElement();

	if (value1 instanceof Float)
	  descr.append(removeZeroAtEnd(formatter.format(value1)));
	else if (value1 instanceof Integer)
	  descr.append(value1);
	else if (value1 instanceof String)
	  {
	    /* Lexical quoting check */

	    if (isAnInt((String) value1) || 
		isAFloat((String) value1) ||
		((! isAKeyword((String) value1)) &&
		includeStartToken((String) value1)))
	      {
		descr.append("\"");
		descr.append(value1);
		descr.append("\"");
	      }
	    else
	      descr.append(value1);

	    if (value1.equals("'"))
	      noNewLine = true;
	    else
	      noNewLine = false;
	  }
	else
	  descr.append(value1);

	/* decide to put or not a blank between the two */
	if (wantASpaceAfter(value1))
	  addBlank = true;
	else if (dontWantASpaceAfter(value1))
	  addBlank = false;
	else if (value2 != null)
	  {
	    if (wantASpaceBefore(value2))
	      addBlank = true;
	    else if (dontWantASpaceBefore(value2))
	      addBlank = false;
	    else
	      addBlank = true;	// if no body care, do a blank
	  }

	value1 = value2;
      }

    return descr.toString();
  }  
}


