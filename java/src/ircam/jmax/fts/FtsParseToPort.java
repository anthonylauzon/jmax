
package ircam.jmax.fts;
import java.util.*;

/**
 * This class implement a lexical analizer for object and message box
 * content (and TCL commands, of course) and send it directly to the FtsPort
 * without any indirect representation, as FtsParse.
 * 
 * The lexer is done in the  non-deterministic, multiple 
 * automata style.<p>
 *
 * If a partial parser reconize a token, it put it in the
 * instance variable parsedToken and return true. <p>
 */

public class FtsParseToPort
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


  FtsPort port;
  String str;
  StringBuffer token;
  int pos = 0; // counter for the string scan
  int backtrack_pos;


  FtsParseToPort(String str, FtsPort port)
  {
    this.str = str;
    this.port = port;
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

  final private void nextChar()
  {
    pos++;
  }

  final private void ungetChar()
  {
    pos--;
  }

  final private void storeChar()
  {
    token.append(str.charAt(pos));
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
   */

  final private boolean isStartToken()
  {
    return ((str.charAt(pos) == '$') || (str.charAt(pos) == ',') ||
	    (str.charAt(pos) == ';') || (str.charAt(pos) == '\''));
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

  /* Semantic function: each of them parse/send a single value
     */

  final private void ParseLong() throws java.io.IOException
  {
    port.sendInt(token);
  }

  final private void ParseFloat() throws java.io.IOException
  {
    port.sendFloat(token);
  }

  final private void ParseString() throws java.io.IOException
  {
    port.sendString(token);
  }

  /* The keyword parser is not an automata
     Reconize tokens which are always tokens, i.e. tokens
     which end is predefined and do not depend on what follow.

     Current only example is "$", but there will be more.

     */

  private boolean tryKeywords() throws java.io.IOException
  {
    tryParse();
    
    /* For now, work only with "$" */

    if (isEndOfString())
      {
	backtrack();
	return false;
      }
    else if (currentChar() == '$')
      {
	storeChar();
	nextChar();
	ParseString();
	return true;
      }
    else if (currentChar() == '\'')
      {
	storeChar();
	nextChar();
	ParseString();
	return true;
      }
    else
      {
	backtrack();
	return false;
      }
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


  /** Parse an object argument description (without the class Name) */

  public static void parseAndSendObject(String str, FtsPort port) throws java.io.IOException
  {
    FtsParseToPort parser = new FtsParseToPort(str, port);

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
}



	

