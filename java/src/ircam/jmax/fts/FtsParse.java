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

import java.util.*;
import java.text.*;

import ircam.jmax.utils.*;

/** Lexical analyzer/unparser.
 * This class implement a lexical analizer for object and message box
 * content; it is in the object layer
 * because the lexical conventions depend on the fts language, and not
 * on the editor; other clients can use the same lexical analizer. <p>
 *
 * The lexical parsing should be done in FTS, actually !!
 * 
 * The lexer is done in the  non-deterministic, multiple 
 * automata style, and is implemented  by the class FtsParse. <p>
 *
 * If a partial parser reconize a token, it put it in the
 * instance variable parsedToken and return true. <p>
 * 
 * Waiting for multiple convention support, we support
 * an almost Max 0.26 compatible lexical convention
 */


public class FtsParse
{
  final static int lex_int_start = 0;
  final static int lex_int_in_value = 1;
  final static int lex_int_in_sign = 2;
  final static int lex_int_end = 3;

  final static int lex_float_start = 0;
  final static int lex_float_in_value = 1;
  final static int lex_float_in_sign = 2;
  final static int lex_float_after_point = 3;
  final static int lex_float_end = 4;

  final static int lex_qstring_start = 0;
  final static int lex_qstring_in_value = 1;
  final static int lex_qstring_qchar = 2;
  final static int lex_qstring_end = 3;

  final static int lex_string_start = 0;
  final static int lex_string_in_value = 1;
  final static int lex_string_qchar = 2;
  final static int lex_string_end = 3;

  /* To unparse floating point numbers */

  static private DecimalFormat formatter;

  static
  {
    // Number format for messages coming from FTS (to be cleaned up:
    // the text should be sent by FTS as text alread).
    
    formatter  = new DecimalFormat("0.######;-0.######");
    formatter.setGroupingUsed(false);
    formatter.setDecimalSeparatorAlwaysShown(true);
  }

  /* Operating variables */

  boolean toStream;
  Object parsedToken;
  FtsStream stream;
  String str;
  StringBuffer token;
  int pos = 0; // counter for the string scan
  int backtrack_pos;


  FtsParse(String str)
  {
    this.str = str;
    toStream = false;
  }

  FtsParse(String str, FtsStream stream)
  {
    this.str = str;
    this.stream = stream;
    toStream = true;
  }

  /** try handling */

  final private void tryParse()
  {
    token = new StringBuffer();
    backtrack_pos = pos;
  }

  /** backtrack handling */

  final private void backtrack()
  {
    pos = backtrack_pos;
  }


  final private char currentChar()
  {
    return str.charAt(pos);
  }

  final private boolean currentStringIs(String keyword)
  {
    return str.startsWith(keyword, pos);
  }

  final private void nextChar()
  {
    pos++;
  }

  final private void nextChar(int i)
  {
    pos += i;
  }

  final private void ungetChar()
  {
    pos--;
  }

  final private void storeChar(char c)
  {
    token.append(c);
  }

  final private void storeString(String s)
  {
    token.append(s);
  }

  static final private int charValue(char c)
  {
    return (int) c - (int) '0';
  }

  /* predicates that identify char properties */

  /** identify token separators that can be ignored*/

  static final private boolean isSeparator(char c)
  {
    return (c == ' ') || (c == '\n') || (c == '\t');
  }

  /**
   * Identify characters that always start a new token.
   * Also if not separated; they must be put in the new token.
   */

  static final private boolean isStartToken(char c)
  {
    return ((c == '$') || (c == ',') ||
	    (c == '(') || (c == ')') ||
	    (c == '[') || (c == ']') ||
	    (c == '{') || (c == '}') ||
	    (c == ':') || 
	    (c == ';') || (c == '\''));
  }

  /** Identify the lexical char quote character */

  static final private boolean isQuoteChar(char c)
  {
    return (c == '\\');
  }

  /** Identify the lexical start quote and end quote character */

  static final private boolean isQuoteStart(char c)
  {
    return (c == '"');
  }

  static final private boolean isQuoteEnd(char c)
  {
    return (c == '"');
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

  /* Identify the end of the parsed string */

  final private boolean atEndOfString()
  {
    return (pos >= str.length());
  }

  /* Semantic function: each of them parse a single value
     and put it in the parsedToken variable.
     */

  final private void ParseInt() throws java.io.IOException
  {
    if (toStream)
      stream.sendInt(token);
    else
      parsedToken = new Integer(token.toString());
  }

  final private void ParseFloat() throws java.io.IOException
  {
    if (toStream)
      stream.sendFloat(token);
    else
      parsedToken = new Float(token.toString());
  }

  final private void ParseString() throws java.io.IOException
  {
    if (toStream)
      stream.sendString(token);
    else
      parsedToken = token.toString();
  }

  /** Reconize keywords. The keyword parser is not an automata
     Reconize tokens which are always tokens, i.e. tokens
     which end is predefined and do not depend on what follow.
     */

  private boolean tryKeywords() throws java.io.IOException
  {
    // The order of this list is important

    String keywords[] = { "(", ")",
			  "[", "]", "{", "}", ",", 
			  ":", "$", ".",
			  ";", "'"};

    tryParse();

    if (atEndOfString())
      {
	backtrack();
	return false;
      }
    
    for (int i = 0 ; i < keywords.length; i++)
      {
	if (currentStringIs(keywords[i]))
	  {
	    nextChar(keywords[i].length());
	    storeString(keywords[i]);
	    ParseString();
	    return true;
	  }
      }

    // It is not a keyword.

    backtrack();
    return false;
  }


  /** Reconize an integer */

  private boolean tryInt() throws java.io.IOException
  {
    int status;

    tryParse();
    status = lex_int_start;

    while (status != lex_int_end)
      {
	switch (status)
	  {
	  case lex_int_start:
	    if (atEndOfString())
	      {backtrack(); return false;}
	    else if (isDigit(currentChar()))
	      {storeChar(currentChar()); status = lex_int_in_value;}
	    else if (isSign(currentChar()))
	      {storeChar(currentChar()); status = lex_int_in_sign;}
	    else
	      {backtrack(); return false;}
	    break;

	  case lex_int_in_sign:
	    if (atEndOfString())
	      {backtrack(); return false;}
	    else if (isDigit(currentChar()))
	      {storeChar(currentChar()); status = lex_int_in_value;}
	    else
	      {backtrack(); return false;}
	    break;

	  case lex_int_in_value:
	    if (atEndOfString() ||
		isSeparator(currentChar())     ||
		isStartToken(currentChar()))
	      {ungetChar(); ParseInt(); status = lex_int_end;}
	    else if (isDigit(currentChar()))
	      {storeChar(currentChar()); status = lex_int_in_value;}
	    else
	      {backtrack(); return false;}
	    break;

	  case lex_int_end:
	    break;
	  }

	nextChar();
      }

    return true;
  }

  /** Reconize an float */

  private boolean tryFloat() throws java.io.IOException
  {
    int status;

    tryParse();
    status = lex_float_start;

    while (status != lex_float_end)
      {
	switch (status)
	  {
	  case lex_float_start:
	    if (atEndOfString())
	      {backtrack(); return false;}
	    else if (isDigit(currentChar()))
	      {storeChar(currentChar()); status = lex_float_in_value;}
	    else if (isSign(currentChar()))
	      {storeChar(currentChar()); status = lex_float_in_sign;}
	    else if (isDecimalPoint(currentChar()))
	      {storeChar(currentChar()); status = lex_float_after_point;}
	    else
	      {backtrack(); return false;}
	    
	    break;

	  case lex_float_in_sign:

	    if (atEndOfString())
	      {backtrack(); return false;}
	    else if (isDigit(currentChar()))
	      {storeChar(currentChar()); status = lex_float_in_value;}
	    else if (isDecimalPoint(currentChar()))
	      {storeChar(currentChar()); status = lex_float_after_point;}
	    else
	      {backtrack(); return false;}
	    break;

	  case lex_float_in_value:

	    if (atEndOfString())
	      {backtrack(); return false;}
	    else if (isDigit(currentChar()))
	      {storeChar(currentChar()); status = lex_float_in_value;}
	    else if (isDecimalPoint(currentChar()))
	      {storeChar(currentChar()); status = lex_float_after_point;}
	    else
	      {backtrack(); return false;}
	    break;

	  case lex_float_after_point:

	    if (atEndOfString() ||
		isSeparator(currentChar())     ||
		isStartToken(currentChar()))
	      {ungetChar(); ParseFloat(); status = lex_float_end;}
	    else if (isDigit(currentChar()))
	      {storeChar(currentChar()); status = lex_float_after_point;}
	    else
	      {backtrack(); return false;}
	    break;

	  case lex_float_end:
	    break;
	  }

	nextChar();
      }


    return true;
  }


  /** Reconize a quoted string.
   * A qstring is a string surrounded by quote-start
   * quote-end pairs (usually, double quotes)
   */

  private boolean tryQString() throws java.io.IOException
  {
    int  status;

    tryParse();
    status = lex_qstring_start;

    while (status != lex_qstring_end)
      {
	switch (status)
	  {
	  case lex_qstring_start:
	    if (atEndOfString())
	      {backtrack(); return false;}
	    else if (isQuoteStart(currentChar()))
	      status = lex_qstring_in_value;
	    else
	      {backtrack(); return false;}
	    break;

	  case lex_qstring_in_value:
	    if (atEndOfString())
	      {backtrack(); return false;}
	    else if (isQuoteChar(currentChar()))
	      status = lex_qstring_qchar;
	    else if (isQuoteEnd(currentChar()))
	      {ParseString(); status = lex_qstring_end;}
	    else
	      storeChar(currentChar());
	    break;

	  case lex_qstring_qchar:
	    if (atEndOfString())
	      {backtrack(); return false;}
	    else
	      {storeChar(currentChar()); status = lex_qstring_in_value;}
	    break;

	  case lex_qstring_end:
	    break;
	  }

	nextChar();
      }

    return true ;
  }


  /** Reconize a string (symbol) */

  private boolean tryString() throws java.io.IOException
  {
    int  status;

    tryParse();
    status = lex_string_start;

    while (status != lex_string_end)
      {
	switch (status)
	  {
	  case lex_string_start:
	    if (atEndOfString())
	      {backtrack(); return false;}
	    else if (isQuoteChar(currentChar()))
	      status = lex_string_qchar;
	    else
	      {storeChar(currentChar()); status = lex_string_in_value;}
	    break;

	  case lex_string_in_value:
	    if (atEndOfString() || 
		     isSeparator(currentChar())     ||
		     isStartToken(currentChar()))
	      {ungetChar(); ParseString(); status = lex_string_end;}
	    else if (isQuoteChar(currentChar()))
	      status = lex_string_qchar;
	    else
	      {storeChar(currentChar()); status = lex_string_in_value;}
	    break;

	  case lex_string_qchar:
	    if (atEndOfString())
	      {backtrack(); return false;}
	    else
	      {storeChar(currentChar()); status = lex_string_in_value;}
	      
	    break;
	  case lex_string_end:
	    break;
	  }

	nextChar();
      }

    return true;
  }

  /** Parse an object argument description. Without the class Name.
   * and send it to an FtsStream (optimization to reduce object allocation
   * during editing).
   */

  public static void parseAndSendObject(String str, FtsStream stream) throws java.io.IOException
  {
    FtsParse parser = new FtsParse(str, stream);

    while (! parser.atEndOfString())
      {
	/* First, a multiple separator skip loop,
	   just to allow ignoring separators in the
	   single automata.

	   Should be cleaner and nicer :-< ...
	   */

	while ((! parser.atEndOfString()) && parser.isSeparator(parser.currentChar()))
	  parser.nextChar();

	if (parser.atEndOfString())
	  break;

	/* The order is important, beacause the 
	   last parser get accept everything as a symbol,
	   for easiness of implementation; also, a float is made
	   first of an int, followed by the decimal point; so
	   float must come before ints.
	   
	   Every parser return 1 and advance the pointer to the
	   char after the end of the reconized token only
	   if the parsing has been succesfull.
	 */

	if (! parser.tryKeywords())
	  if (! parser.tryFloat())
	    if (! parser.tryInt())
	      if (! parser.tryQString())
		parser.tryString();
      }
  }

  /** Parse an list of atoms in a MaxVector */

  public static void parseAtoms(String str, MaxVector values)
  {
    FtsParse parser = new FtsParse(str);

    /* Dummy try/catch pair: IOException never thrown in this
       case, and in this way we avoid to have the throws declaration
       in the method */

    try
      {
	while (! parser.atEndOfString())
	  {
	    /* First, a multiple separator skip loop,
	       just to allow ignoring separators in the
	       single automata.

	       Should be cleaner and nicer :-< ...
	       */

	    while ((! parser.atEndOfString()) && isSeparator(parser.currentChar()))
	      parser.nextChar();

	    if (parser.atEndOfString())
	      break;

	    /* The order is important, beacause the 
	       last parser get accept everything as a symbol,
	       for easiness of implementation; also, the float parser
	       accept also ints, so the int parser must be called
	       before the float parser.
	   
	       Every parser return 1 and advance the pointer to the
	       char after the end of the reconized token only
	       if the parsing has been succesfull.
	       */

	    if (! parser.tryKeywords())
	      if (! parser.tryInt())
		if (! parser.tryFloat())
		  if (! parser.tryQString())
		    parser.tryString();

	    values.addElement(parser.parsedToken);
	  }
      }
    catch (java.io.IOException e)
      {
	// Ignore
      }
  }


  // Unparser.
  // Functions to classify values and tokens.

  static private final boolean wantASpaceBefore(Object value)
  {
    if (value instanceof String)
      {
	String keywords[] = {"+", "-", "*", "/", "%", 
			     "&&", "&", "||", "|", "==", "=", "!=", "!", ">=", "^",
			     ">>", ">", "<<", "<=", "<", "?", ":" };

	for (int i = 0 ; i < keywords.length; i++)
	  if (keywords[i].equals((String) value))
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
	String keywords[] = {")", "[", "]", "}", ",", ";", "."};


	for (int i = 0 ; i < keywords.length; i++)
	  if (keywords[i].equals((String) value))
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
			      ">>", ">", "<<", "<=", "<", "?", ":", "^",
			      ";" };

	for (int i = 0 ; i < keywords.length; i++)
	  if (keywords[i].equals((String) value))
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
			      "$", "'", "." };

	for (int i = 0 ; i < keywords.length; i++)
	  if (keywords[i].equals((String) value))
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
			  ":", "$", ".",
			  ";", "'" };
    
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

  static String unparseObjectDescription(FtsStream stream)
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    return unparseObjectDescription(stream.getNextArgument(), stream);
  }

  /** Unparse an object description from a FTS message.  */
  static String removeZeroAtEnd(String buff){
    while(buff.endsWith("0")){
      buff = buff.substring(0, buff.length()-1);
    }
    return buff;
  }

  static String unparseObjectDescription(Object initValue, FtsStream stream)
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    boolean doNewLine = false;
    boolean addBlank = false;
    boolean noNewLine = false;
    Object value1 = null;
    Object value2 = null;

    StringBuffer descr = new StringBuffer();

    value2 = initValue;
    value1 = value2;

    while (value1 != null)
      {
	if (doNewLine)
	  descr.append("\n");
	else if (addBlank)
	  descr.append(" ");

	doNewLine = false;

	if (stream.endOfArguments())
	  value2 = null;
	else
	  value2 = stream.getNextArgument();

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
	    else if (value1.equals(";"))
	      {
		if (noNewLine)
		  noNewLine = false;
		else
		  doNewLine = true;
	      }
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

  /** Version used for the comments, to avoid introducing quotes
    in comments. Note that comments now use property for the text;
    may be this is not needed anymore.*/

  static String simpleUnparseObjectDescription(FtsStream stream)
       throws java.io.IOException, FtsQuittedException, java.io.InterruptedIOException
  {
    boolean doNewLine = false;
    boolean addBlank = false;
    boolean noNewLine = false;
    Object value1 = null;
    Object value2 = null;

    StringBuffer descr = new StringBuffer();

    if (stream.endOfArguments())
      value2 = null;
    else
      value2 = stream.getNextArgument();

    value1 = value2;

    while (value1 != null)
      {
	if (doNewLine)
	  descr.append("\n");
	else if (addBlank)
	  descr.append(" ");

	doNewLine = false;

	if (stream.endOfArguments())
	  value2 = null;
	else
	  value2 = stream.getNextArgument();

	if (value1 instanceof Float)
	  descr.append(formatter.format(value1));
	else if (value1 instanceof Integer)
	  descr.append(value1);
	else if (value1 instanceof String)
	  {
	    /* Lexical quoting check */

	    descr.append(value1);

	    if (value1.equals("'"))
	      noNewLine = true;
	    else if (value1.equals(";"))
	      {
		if (noNewLine)
		  noNewLine = false;
		else
		  doNewLine = true;
	      }
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

  /*  Unparse a description passed as a vector of values
      Used by atom list, available as a service for anybody.*/

  static String unparseDescription(MaxVector values)
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
	  descr.append(formatter.format(value1));
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
	    else if (value1.equals(";"))
	      {
		if (noNewLine)
		  noNewLine = false;
		else
		  doNewLine = true;
	      }
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



	


