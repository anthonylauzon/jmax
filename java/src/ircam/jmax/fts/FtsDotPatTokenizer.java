package ircam.jmax.fts;

import java.io.*;
import java.util.*;

/**
 * A lexical analyser for .pat files, similar to the StreamTokenizer class.  <p>
 * But: it reconize int as separate then floats, and
 * it parse the ending ';' in TT_EOC (end of command). <br>
 * Also, it do backslash quoting. <p>
 * It handle also variable substitution
 * for abstractions (in max, they are handled before the lexical
 * analisys: $0-foo is expanded to a single string). <p>
 *
 *  @see FtsDotPatParser
 */

class FtsDotPatTokenizer 
{
  static private int unique_count = 3333; // the unique number generation

  /* exported variables */

  int ttype;
  String sval;
  float fval;
  int   nval;
  Vector env;

  InputStream in;

  /* Token types */

  final static int TT_NUMBER = 0;
  final static int TT_FLOAT  = 1;
  final static int TT_STRING = 2;
  final static int TT_EOC    = 3;
  final static int TT_EOF    = 4;


  /* private variables */

  private boolean pushedBack = false;

  private boolean lookahead_valid = false;
  private int lookahead;	// one char that may have been stored 
				//here  between two calls.

  private int unique_var = unique_count++; // the unique number used in variable  0 substitution
  private StringBuffer buf = new StringBuffer();

  /* constructors */

  FtsDotPatTokenizer(InputStream in)
  {
    this.in = in;
  }

  FtsDotPatTokenizer(InputStream in, Vector env)
  {
    this.in = in;
    this.env = env;
  }

  /** Debug method: store the history 
    (actually for now just print it)
    */

  
  void registerInHistory(char c)
  {
    // System.err.println(">" + (char)c + "<");
  }

  void pushBack()
  {
    pushedBack = true;
  }

  /** Getting an env value. */

  Object getEnvValue(int idx)
  {
    if (idx > env.size())
      return new Integer(0);
    else
      return env.elementAt(idx - 1);
  }

  /* private predicates for character classes */

  boolean isDollar(int c)
  {
    return c == '$';
  }

  boolean isBlank(int c)
  {
    return (c == '\t') || (c == ' ') || (c == '\n') || (c == '\r');
  }

  boolean isEof(int c)
  {
    return (c == -1);
  }

  boolean isSemi(int c)
  {
    return (c == ';');
  }


  boolean isDigit(int c)
  {
    return ((c == '0') || (c == '1') || (c == '2') || (c == '3') || (c == '4') ||
	    (c == '5') || (c == '6') || (c == '7') || (c == '8') || (c == '9'));
  }

  boolean isDecimalPoint(int c)
  {
    return (c == '.');
  }

  boolean isSign(int c)
  {
    return (c == '-');
  }

  boolean isBackSlash(int c)
  {
    return (c == '\\');
  }

  /* states for the automata */

  private final static int tt_waiting    =  0;
  private final static int tt_in_var     =  1;
  private final static int tt_in_string  =  2;
  private final static int tt_in_quoted_char =  3;
  private final static int tt_in_qquoted_char =  4;
  private final static int tt_in_number  =  5;
  private final static int tt_in_number_or_sign  =  6;
  private final static int tt_in_float   =  7;

  void nextToken() throws java.io.IOException
  {
    buf.setLength(0);

    if (pushedBack)
      {
	pushedBack = false;
	return;
      }
    else
      {
	int c, status;

	if (lookahead_valid)
	  {
	    c = lookahead;
	    lookahead_valid = false;
	  }
	else
	  {
	    c = in.read();
	    registerInHistory((char) c);
	  }

	status = tt_waiting;

	while (true)
	  {
	    switch (status)
	      {
	      case tt_waiting:
		if (isEof(c))
		  {
		    ttype = TT_EOF;
		    return;
		  }
		else if (isDollar(c))
		  {
		    status = tt_in_var;
		  }
		else if (isSemi(c))
		  {
		    ttype = TT_EOC;
		    return;
		  }
		else if (isDigit(c))
		  {
		    buf.append((char) c);
		    status = tt_in_number;
		  }
		else if (isSign(c))
		  {
		    buf.append((char) c);
		    status = tt_in_number_or_sign;
		  }
		else if (isBackSlash(c))
		  {
		    status = tt_in_quoted_char;
		  }
		else if (isBlank(c))
		  {
		    status = tt_waiting;
		  }
		else
		  {	  
		    buf.append((char) c);
		    status = tt_in_string;
		  }
		break;
		
	      case tt_in_var:
		if (isDigit(c))
		  {
		    if (env != null)
		      {
			int v = c - '0'; 

			if (v == 0)
			  {
			    buf.append(Integer.toString(unique_var));
			    status = tt_in_string;
			  }
			else
			  {
			    Object value = getEnvValue(v);

			    if (value instanceof Integer)
			      {
				buf.append(value.toString());
				status = tt_in_number;
			      }
			    else if (value instanceof Float)
			      {
				buf.append(value.toString());
				status = tt_in_float;
			      }
			    else if (value instanceof String)
			      {
				buf.append((String) value);
				status = tt_in_string;
			      }
			    else
			      {
				buf.append(value.toString());
				status = tt_in_string;
			      }
			  }
			
		      }
		  }
		else
		  {
		    buf.append((char) '$');
		    buf.append((char) c);
		    status = tt_in_string;
		  }

		break;

	      case tt_in_string:
		if (isSemi(c))
		  {
		    sval = buf.toString();
		    ttype = TT_STRING;
		    lookahead = c;
		    lookahead_valid = true;
		    return;
		  }
		else if (isBlank(c) || isEof(c))
		  {
		    sval = buf.toString();
		    ttype = TT_STRING;
		    return;
		  }
		else if (isBackSlash(c))
		  {
		    status = tt_in_quoted_char;
		  }
		else
		  {	  
		    buf.append((char) c);
		    status = tt_in_string;
		  }
		break;

	      case tt_in_quoted_char:
		if (isBackSlash(c))
		  {
		    status = tt_in_qquoted_char;
		  }
		else if (isDollar(c))
		  {
		    buf.append((char) c);
		    sval = buf.toString();
		    ttype = TT_STRING;
		    return;
		  }
		else
		  {
		    buf.append((char) c);
		    status = tt_in_string;
		  }
		break;

	      case tt_in_qquoted_char:
		if (isBackSlash(c))
		  {
		    status = tt_in_quoted_char;
		  }
		else
		  {
		    buf.append((char) c);
		    status = tt_in_string;
		  }
		break;
		    

	      case tt_in_number:
		if (isSemi(c))
		  {
		    nval = Integer.parseInt(buf.toString());
		    ttype = TT_NUMBER;
		    lookahead = c;
		    lookahead_valid = true;
		    return;
		  }
		else if (isBlank(c) || isEof(c))
		  {
		    nval = Integer.parseInt(buf.toString());
		    ttype = TT_NUMBER;
		    return;
		  }
		else if (isDigit(c))
		  {
		    buf.append((char) c);
		    status = tt_in_number;
		  }
		else if (isDecimalPoint(c))
		  {
		    buf.append((char) c);
		    status = tt_in_float;
		  }
		else
		  {	  
		    buf.append((char) c);
		    status = tt_in_string;
		  }
		break;


	      case tt_in_number_or_sign:
		if (isSemi(c))
		  {
		    sval = buf.toString();
		    ttype = TT_STRING;
		    lookahead = c;
		    lookahead_valid = true;
		    return;
		  }
		else if (isBlank(c) || isEof(c))
		  {
		    sval = buf.toString();
		    ttype = TT_STRING;
		    return;
		  }
		else if (isDigit(c))
		  {
		    buf.append((char) c);
		    status = tt_in_number;
		  }
		else if (isDecimalPoint(c))
		  {
		    buf.append((char) c);
		    status = tt_in_float;
		  }
		else
		  {	  
		    buf.append((char) c);
		    status = tt_in_string;
		  }
		break;

	      case tt_in_float:
		if (isSemi(c))
		  {
		    fval = Float.valueOf(buf.toString()).floatValue();
		    ttype = TT_FLOAT;
		    lookahead = c;
		    lookahead_valid = true;
		    return;
		  }
		else if (isBlank(c) || isEof(c))
		  {
		    fval = Float.valueOf(buf.toString()).floatValue();
		    ttype = TT_FLOAT;
		    return;
		  }
		else if (isDigit(c))
		  {
		    buf.append((char) c);
		    status = tt_in_float;
		  }
		else
		  {	  
		    buf.append((char) c);
		    status = tt_in_string;
		  }
		break;
	      }

	    c = in.read();
	    registerInHistory((char) c);
	  }
      }
  }
}
    
    

