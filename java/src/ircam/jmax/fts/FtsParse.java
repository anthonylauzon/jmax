package ircam.jmax.fts;

import java.util.*;
import java.text.*;

/**
 * This class implement a lexical analizer for object and message box
 * content; it is in the object layer
 * because the lexical conventions depend on the fts language, and not
 * on the editor; other clients can use the same lexical analizer. <p>
 * The parsing should do straight to FTS !!
 * 
 * The lexer is done in the  non-deterministic, multiple 
 * automata style, and is implemented  by the class FtsParse. <p>
 *
 * If a partial parser reconize a token, it put it in the
 * instance variable parsedToken and return true. <p>
 */

public class FtsParse
{
  final static int lex_long_start = 0;
  final static int lex_long_in_value = 1;
  final static int lex_long_in_sign = 2;
  final static int lex_long_end = 3;

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

  static private NumberFormat numberFormat;

  static
  {
    // Number format for messages coming from FTS (to be cleaned up:
    // the text should be sent by FTS as text alread).
    
    numberFormat = NumberFormat.getInstance(Locale.US);
    numberFormat.setMaximumFractionDigits(6);
    numberFormat.setMinimumFractionDigits(1);
    numberFormat.setGroupingUsed(false);
  }

  /* Operating variables */

  boolean toPort;
  Object parsedToken;
  FtsPort port;
  String str;
  StringBuffer token;
  int pos = 0; // counter for the string scan
  int backtrack_pos;


  FtsParse(String str)
  {
    this.str = str;
    toPort = false;
  }

  FtsParse(String str, FtsPort port)
  {
    this.str = str;
    this.port = port;
    toPort = true;
  }

  /** try/backtrack handling */

  final private void tryParse()
  {
    token = new StringBuffer();
    backtrack_pos = pos;
  }

  final private void backtrack()
  {
    pos = backtrack_pos;
  }

  /** manipulate the state */

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

  final private void storeChar()
  {
    token.append(str.charAt(pos));
  }

  final private void storeString(String s)
  {
    token.append(s);
  }

  final private int charValue()
  {
    return (int) str.charAt(pos) - (int) '0';
  }

  /* predicates that identify char properties */

  /** identify token separators that can be ignored*/

  final private boolean isSeparator()
  {
    return (str.charAt(pos) == ' ') || (str.charAt(pos) == '\n') || (str.charAt(pos) == '\t');
  }

  /**
   * Identify characters that always start a new token
   * also if not separated; they must be put in the new token.
   * '~' suppressed !!!
   */

  final private boolean isStartToken()
  {
    int c = str.charAt(pos);

    return ((c == '$') || (c == ',') ||
	    (c == '(') || (c == ')') ||
	    (c == '[') || (c == ']') ||
	    (c == '{') || (c == '}') ||
	    (c == '+') || (c == '-') ||
	    (c == '*') || (c == '/') ||
	    (c == '%') || 
	    (c == '&') || (c == '|') ||
	    (c == '^') || (c == '.') ||
	    (c == '<') || (c == '>') ||
	    (c == '!') || (c == '=') ||
	    (c == '?') || (c == ':') ||
	    (c == ';') || (c == '\''));
  }

  /** Identify the lexical char quote character */

  final private boolean isQuoteChar()
  {
    return (str.charAt(pos) == '\\');
  }

  /** Identify the lexical start quote and end quote character */

  final private boolean isQuoteStart()
  {
    return (str.charAt(pos) == '"');
  }

  final private boolean isQuoteEnd()
  {
    return (str.charAt(pos) == '"');
  }

  /** Identify digits */

  final private boolean isDigit()
  {
    int c = str.charAt(pos);

    return ((c == '0') || (c == '1') || (c == '2') || (c == '3') || (c == '4') ||
	    (c == '5') || (c == '6') || (c == '7') || (c == '8') || (c == '9'));
  }

  /** identify  the sign char */

  final private boolean isSign()
  {
    return (str.charAt(pos) == '-');
  }

  /** Identify decimal point, and so a float representation */

  final private boolean isDecimalPoint()
  {
    return (str.charAt(pos) == '.');
  }

  /* Identify the end of the parsed string */

  final private boolean isEndOfString()
  {
    return (pos >= str.length());
  }

  /* Semantic function: each of them parse a single value
     and put it in the parsedToken variable.
     */

  final private void ParseLong() throws java.io.IOException
  {
    if (toPort)
      port.sendInt(token);
    else
      parsedToken = new Integer(token.toString());
  }

  final private void ParseFloat() throws java.io.IOException
  {
    if (toPort)
      port.sendFloat(token);
    else
      parsedToken = new Float(token.toString());
  }

  final private void ParseString() throws java.io.IOException
  {
    if (toPort)
      port.sendString(token);
    else
      parsedToken = token.toString();
  }

  /* The keyword parser is not an automata
     Reconize tokens which are always tokens, i.e. tokens
     which end is predefined and do not depend on what follow.

     ~ is suppressed
     */

  private boolean tryKeywords() throws java.io.IOException
  {
    String keywords[] = { "+", "-", "*", "/", "%", "(", ")",
			  "[", "]", "{", "}", ",", "^", "&&",
			  "&", "||", "|", "==", "=", "!=", "!", ">=",
			  ">>", ">", "<<", "<=", "<", "?", ":", "$",
			  ".", ";", "'" };

    tryParse();

    if (isEndOfString())
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


  private boolean tryLong() throws java.io.IOException
  {
    int status;

    tryParse();
    status = lex_long_start;

    while (status != lex_long_end)
      {
	switch (status)
	  {
	  case lex_long_start:
	    if (isEndOfString())
	      {backtrack(); return false;}
	    else if (isDigit())
	      {storeChar(); status = lex_long_in_value;}
	    else if (isSign())
	      {storeChar(); status = lex_long_in_sign;}
	    else
	      {backtrack(); return false;}
	    break;

	  case lex_long_in_sign:
	    if (isEndOfString())
	      {backtrack(); return false;}
	    else if (isDigit())
	      {storeChar(); status = lex_long_in_value;}
	    else
	      {backtrack(); return false;}
	    break;

	  case lex_long_in_value:
	    if (isEndOfString() ||
		isSeparator()     ||
		isStartToken())
	      {ungetChar(); ParseLong(); status = lex_long_end;}
	    else if (isDigit())
	      {storeChar(); status = lex_long_in_value;}
	    else
	      {backtrack(); return false;}
	    break;

	  case lex_long_end:
	    break;
	  }

	nextChar();
      }

    return true;
  }


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

	    if (isEndOfString())
	      {backtrack(); return false;}
	    else if (isDigit())
	      {storeChar(); status = lex_float_in_value;}
	    else if (isSign())
	      {storeChar(); status = lex_float_in_sign;}
	    else
	      {backtrack(); return false;}
	    
	    break;

	  case lex_float_in_sign:
	    if (isEndOfString())
	      {backtrack(); return false;}
	    else if (isDigit())
	      {storeChar(); status = lex_float_in_value;}
	    else if (isDecimalPoint())
	      {status = lex_float_after_point;}
	    else
	      {backtrack(); return false;}
	    break;

	  case lex_float_in_value:

	    if (isEndOfString() ||
		isSeparator()     ||
		isStartToken())
	      {ungetChar(); ParseFloat(); status = lex_float_end;}
	    else if (isDigit())
	      {storeChar(); status = lex_float_in_value;}
	    else if (isDecimalPoint())
	      {storeChar(); status = lex_float_after_point;}
	    else
	      {backtrack(); return false;}

	    break;

	  case lex_float_after_point:

	    if (isEndOfString() ||
		isSeparator()     ||
		isStartToken())
	      {ungetChar(); ParseFloat(); status = lex_float_end;}
	    else if (isDigit())
	      {storeChar(); status = lex_float_in_value;}
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


  /**
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
	    if (isEndOfString())
	      {backtrack(); return false;}
	    else if (isQuoteStart())
	      status = lex_qstring_in_value;
	    else
	      {backtrack(); return false;}
	    break;

	  case lex_qstring_in_value:
	    if (isEndOfString())
	      {backtrack(); return false;}
	    else if (isQuoteChar())
	      status = lex_qstring_qchar;
	    else if (isQuoteEnd())
	      {ParseString(); status = lex_qstring_end;}
	    else
	      storeChar();
	    break;

	  case lex_qstring_qchar:
	    if (isEndOfString())
	      {backtrack(); return false;}
	    else
	      {storeChar(); status = lex_qstring_in_value;}
	    break;

	  case lex_qstring_end:
	    break;
	  }

	nextChar();
      }

    return true ;
  }


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
	    if (isEndOfString())
	      {backtrack(); return false;}
	    else if (isQuoteChar())
	      status = lex_string_qchar;
	    else
	      {storeChar(); status = lex_string_in_value;}
	    break;

	  case lex_string_in_value:
	    if (isEndOfString() || 
		     isSeparator()     ||
		     isStartToken())
	      {ungetChar(); ParseString(); status = lex_string_end;}
	    else if (isQuoteChar())
	      status = lex_string_qchar;
	    else
	      {storeChar(); status = lex_string_in_value;}
	    break;

	  case lex_string_qchar:
	    if (isEndOfString())
	      {backtrack(); return false;}
	    else
	      {storeChar(); status = lex_string_in_value;}
	      
	    break;
	  case lex_string_end:
	    break;
	  }

	nextChar();
      }

    return true;
  }

  /** Parse an object argument description (without the class Name),
   * and send it to an FtsPort (optimization to reduce object allocation
   * during editing).
   */

  public static void parseAndSendObject(String str, FtsPort port) throws java.io.IOException
  {
    FtsParse parser = new FtsParse(str, port);

    while (! parser.isEndOfString())
      {
	/* First, a multiple separator skip loop,
	   just to allow ignoring separators in the
	   single automata.

	   Should be cleaner and nicer :-< ...
	   */

	while ((! parser.isEndOfString()) && parser.isSeparator())
	  parser.nextChar();

	if (parser.isEndOfString())
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
	  if (! parser.tryLong())
	    if (! parser.tryFloat())
	      if (! parser.tryQString())
		parser.tryString();
      }
  }

  /** Parse an list of atoms in a Vector */

  public static void parseAtoms(String str, Vector values)
  {
    FtsParse parser = new FtsParse(str);

    /* Dummy try/catch pair: IOException never thrown in this
       case, and in this way we avoid to have the throws declaration
       in the method */

    try
      {
	while (! parser.isEndOfString())
	  {
	    /* First, a multiple separator skip loop,
	       just to allow ignoring separators in the
	       single automata.

	       Should be cleaner and nicer :-< ...
	       */

	    while ((! parser.isEndOfString()) && parser.isSeparator())
	      parser.nextChar();

	    if (parser.isEndOfString())
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
	      if (! parser.tryLong())
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

  /* Unparse an object description from a FTS message, starting
     from the given offset;
     */

  static private final boolean wantASpaceBefore(Object value)
  {
    if (value instanceof String)
      {
	String keywords[] = {"+", "-", "*", "/", "%", 
			     "&&", "&", "||", "|", "==", "=", "!=", "!", ">=",
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
	String keywords[] = {")", "[", "]", "}", ",", ".", ";"};


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
			      ">>", ">", "<<", "<=", "<", "?", ":", 
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
	String keywords[] = { "(", "[", "{", "^", 
			      "$", ".", "'" };

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
    String keywords[] = { "+", "-", "*", "/", "%", "(", ")",
			  "[", "]", "{", "}", ",", "^", "&&",
			  "&", "||", "|", "==", "=", "!=", "!", ">=",
			  ">>", ">", "<<", "<=", "<", "?", ":", "$",
			  ".", ";", "'" };

    for (int i = 0 ; i < keywords.length; i++)
      if (keywords[i].equals((String) value))
	return true;

    return false;
  }
      


  static final private boolean includeStartToken(String s)
  {
    char chars[] = { '$', ',', '(', ')', '[', ']',
		     '{', '}', '+', '-', '*', '/',
		     '%', '&', '|', '^', '.',
		     '<', '>', '!', '=', '?', ':',
		     ';', '\'', '\t', ' ' };

    for (int i = 0 ; i < chars.length; i++)
      if (s.indexOf(chars[i]) != -1)
	return true;
    
    return false;
  }


  static String unparseObjectDescription(int offset, FtsMessage msg)
  {
    boolean doNewLine = true;
    boolean addBlank = false;
    boolean noNewLine = false;
    Object value1 = null;
    Object value2 = null;

    StringBuffer descr = new StringBuffer();

    if (msg.getNumberOfArguments() == offset)
      return "";

    value2 = msg.getArgument(offset);

    for (int i = offset + 1; i < (msg.getNumberOfArguments() + 1); i++)
      {
	if (doNewLine)
	  descr.append("\n");
	else if (addBlank)
	  descr.append(" ");

	doNewLine = false;

	value1 = value2;

	if (i < msg.getNumberOfArguments())
	  value2 = msg.getArgument(i);
	else
	  value2 = null;

	if (value1 instanceof Float)
	  descr.append(numberFormat.format(value1));
	else if (value1 instanceof Integer)
	  descr.append(value1);
	else if (value1 instanceof String)
	  {
	    /* Lexical quoting check */

	    if ((! isAKeyword((String) value1)) &&
		includeStartToken((String) value1))
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
      }

    return descr.toString();
  }

  // Version used for the comments, to avoid introducing quotes
  // in comments

  static String simpleUnparseObjectDescription(int offset, FtsMessage msg)
  {
    boolean doNewLine = true;
    boolean addBlank = false;
    boolean noNewLine = false;
    Object value1 = null;
    Object value2 = null;

    StringBuffer descr = new StringBuffer();

    if (msg.getNumberOfArguments() == offset)
      return "";

    value2 = msg.getArgument(offset);

    for (int i = offset + 1; i < (msg.getNumberOfArguments() + 1); i++)
      {
	if (doNewLine)
	  descr.append("\n");
	else if (addBlank)
	  descr.append(" ");

	doNewLine = false;

	value1 = value2;

	if (i < msg.getNumberOfArguments())
	  value2 = msg.getArgument(i);
	else
	  value2 = null;

	if (value1 instanceof Float)
	  descr.append(numberFormat.format(value1));
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
      }

    return descr.toString();
  }
}



	


