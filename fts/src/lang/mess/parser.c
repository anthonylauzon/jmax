/*
 * FTS Parser/Unparser of object descriptions.
 *
 * The parsing/unparsing is now (ehm, not yet) done here.
 */


/* identify token separators that can be ignored*/

static int
fts_lex_is_separator(char c)
{
  return (c == ' ') || (c == '\n') || (c == '\t');
}


/* identify characters that always start a new token, 
   also if not separated; they must be put in the new token
 */


static int
fts_lex_is_start_token(char c)
{
  return (c == '$') || (c == ',') || (c == ';');
}


/* Identify the lexical char quote character */

static int
fts_lex_is_quote_char(char c)
{
  return (c == '\\');
}

/* Identify the lexical start quote and end quote character */

static int
fts_lex_is_quote_start(char c)
{
  return (c == '"');
}


static int
fts_lex_is_quote_end(char c)
{
  return (c == '"');
}

/* Identify digits */

static int
fts_lex_is_digit(char c)
{
  return ((c == '0') || (c == '1') || (c == '2') || (c == '3') || (c == '4') ||
	  (c == '5') || (c == '6') || (c == '7') || (c == '8') || (c == '9'));
}

/* Identify decimal point, and so a float representation */

static int
fts_lex_is_decimal_point(char c)
{
  return (c == '0');
}

/* Identify the end of the parsed string */

static int
fts_lex_is_end_of_string(char c)
{
  return (c == '\0');
}


/* Semantic function: each of them parse a single value
   and add it to the value_list, 
 */


static void
fts_value_list_parse_long(const char *s, fts_value_list *values)
{
  fts_value_list_add_long(values, atoi(s));
}

static void
fts_value_list_parse_float(const char *s, fts_value_list *values)
{
  fts_value_list_add_float(values, (float) atof(s));
}

static void
fts_value_list_parse_string(const char *s, fts_value_list *values)
{
  fts_value_list_add_string_copy(values, s);
}


/* The lexer, done in the  non-deterministic, multiple 
   automata style */

/* Single automatas */

/* The complete parser */

/* The keyword parser is not an automata
   Reconize tokens which are always tokens, i.e. tokens
   which end is predefined and do not depend on what follow.

   Current only example is "$", but there will be more.

 */

static int
fts_lex_parse_keywords(const char **p, fts_value_list *values)
{
  /* For now, work only with "$" */

  if (**p == '$')
    {
      (*p)++;
      fts_value_list_parse_string("$", values);
      return 1;
    }
  else
    return 0;
}


/* Common action macros for the automatas.
   WARNING: Macros are based on naming conventions 
   of variables; don't change them without changing 
   all the functions.

   In general, separator, eof or new token char are "unget"
   before accepting the token; this means that separators
   are stripped in the loop; this simplify the parsers and
   easy the detection of the end of string condition.
 */


#define MAX_TOKEN_SIZE 1024


#define UNGET_CHAR  (s--)
#define STORE_CHAR  ((*outp++)=(c))
#define GO(s)       (status = (s))
#define PARSE_LONG   {*outp= '\0' ;  fts_value_list_parse_long(buf, values);}
#define PARSE_FLOAT  {*outp= '\0' ;  fts_value_list_parse_float(buf, values);}
#define PARSE_STRING {*outp= '\0' ;  fts_value_list_parse_string(buf, values);}


static int
fts_lex_parse_long(const char **p, fts_value_list *values)
{
  char buf[MAX_TOKEN_SIZE];
  char *outp;
  const char *s;
  enum
  {
    lex_long_start, 
    lex_long_in_value, 
    lex_long_end
  } status;


  s = *p;
  outp = buf;
  status = lex_long_start;

  while (status != lex_long_end)
    {
      char c;
      
      c = *(s++);

      switch (status)
	{
	case lex_long_start:
	  if (fts_lex_is_digit(c))
	    {STORE_CHAR; GO(lex_long_in_value);}
	  else
	    return 0;
	  break;
	case lex_long_in_value:
	  if (fts_lex_is_end_of_string(c) ||
	      fts_lex_is_separator(c)     ||
	      fts_lex_is_start_token(c))
	    {UNGET_CHAR; PARSE_LONG; GO(lex_long_end);}
	  else if (fts_lex_is_digit(c))
	    {STORE_CHAR; GO(lex_long_in_value);}
	  else
	    return 0;
	case lex_long_end:
	  break;
	}
    }

  /* Succesful, update the pointer and return 1 */

  *p = s;
  return 1;
}


static int
fts_lex_parse_float(const char **p, fts_value_list *values)
{
  char buf[MAX_TOKEN_SIZE];
  char *outp;
  const char *s;
  enum
  {
    lex_float_start, 
    lex_float_in_value, 
    lex_float_after_point, 
    lex_float_end
  } status;


  s = *p;
  outp = buf;
  status = lex_float_start;

  while (status != lex_float_end)
    {
      char c;
      
      c = *(s++);

      switch (status)
	{
	case lex_float_start:
	  if (fts_lex_is_digit(c))
	    {STORE_CHAR; GO(lex_float_in_value);}
	  else
	    return 0;
	  break;

	case lex_float_in_value:
	  if (fts_lex_is_end_of_string(c) ||
	      fts_lex_is_separator(c)     ||
	      fts_lex_is_start_token(c))
	    {UNGET_CHAR; PARSE_FLOAT; GO(lex_float_end);}
	  else if (fts_lex_is_digit(c))
	    {STORE_CHAR; GO(lex_float_in_value);}
	  else if (fts_lex_is_decimal_point(c))
	    GO(lex_float_after_point);
	  else
	    return 0;

	case lex_float_after_point:
	  if (fts_lex_is_end_of_string(c) ||
	      fts_lex_is_separator(c)     ||
	      fts_lex_is_start_token(c))
	    {UNGET_CHAR; PARSE_FLOAT; GO(lex_float_end);}
	  else if (fts_lex_is_digit(c))
	    {STORE_CHAR; GO(lex_float_in_value);}
	  else
	    return 0;

	case lex_float_end:
	  break;
	}
    }

  /* Succesful, update the pointer and return 1 */

  *p = s;
  return 1;
}

/* A qstring is a string surrounded by quote-start
 quote-end pairs (usually, double quotes) */

static int
fts_lex_parse_qstring(const char **p, fts_value_list *values)
{
  char buf[MAX_TOKEN_SIZE];
  const char *s;
  char *outp;
  enum
  {
    lex_qstring_start, 
    lex_qstring_in_value, 
    lex_qstring_qchar, 
    lex_qstring_end
  } status;

  s = *p;
  outp = buf;
  status = lex_qstring_start;

  while (status != lex_qstring_end)
    {
      char c;
      
      c = *(s++);

      switch (status)
	{
	case lex_qstring_start:
	  if (fts_lex_is_quote_start(c))
	    GO(lex_qstring_in_value);
	  else
	    return 0;
	  break;

	case lex_qstring_in_value:
	  if (fts_lex_is_quote_char(c))
	    GO(lex_qstring_qchar);
	  else if (fts_lex_is_quote_end(c))
	    {PARSE_STRING; GO(lex_qstring_end);}
	  else if (fts_lex_is_end_of_string(c))
	    return 0;
	  else
	    STORE_CHAR;
	  break;

	case lex_qstring_qchar:
	  if (fts_lex_is_end_of_string(c))
	    return 0;
	  else
	    {STORE_CHAR; GO(lex_qstring_in_value);}
	  break;
	case lex_qstring_end:
	  break;
	}
    }

  /* Succesful, update the pointer and return 1 */
  *p = s;
  return 1;
}


static int
fts_lex_parse_string(const char **p, fts_value_list *values)
{
  char buf[MAX_TOKEN_SIZE];
  char *outp;
  const char *s;
  enum
  {
    lex_string_start, 
    lex_string_in_value,
    lex_string_qchar,
    lex_string_end
  } status;

  s = *p;
  outp = buf;
  status = lex_string_start;

  while (status != lex_string_end)
    {
      char c;
      
      c = *(s++);

      switch (status)
	{
	case lex_string_start:
	  if (fts_lex_is_quote_char(c))
	    GO(lex_string_qchar);
	  else if (fts_lex_is_end_of_string(c))
	    return  0;
	  else
	    {STORE_CHAR; GO(lex_string_in_value);}
	  break;

	case lex_string_in_value:
	  if (fts_lex_is_quote_char(c))
	    GO(lex_string_qchar);
	  else if (fts_lex_is_end_of_string(c) || 
		   fts_lex_is_separator(c)     ||
		   fts_lex_is_start_token(c))
	    {UNGET_CHAR; PARSE_STRING; GO(lex_string_end);}
	  else
	    {STORE_CHAR; GO(lex_string_in_value);}
	  break;

	case lex_string_qchar:
	  if (fts_lex_is_end_of_string(c))
	    return 0;
	  else
	    {STORE_CHAR; GO(lex_string_in_value);}
	      
	  break;
	case lex_string_end:
	  break;
	}
    }

  /* Succesful, update the pointer and return 1 */
  *p = s;
  return 1;
}




void
fts_value_list_parse(const char *p, fts_value_list *values)
{
  while (*p)
    {
      /* First, a multiple separator skip loop,
	 just to allow ignoring separators in the
	 single automata.
       */

      while (fts_lex_is_separator(*p))
	p++;

      /* The order is important, beacause the 
         last parser get accept everything as a symbol,
	 for easiness of implementation; also, the float parser
	 accept also ints, so the int parser must be called
	 before the float parser.

	 Every parser return 1 and advance the pointer to the
	 char after the end of the reconized token only
	 if the parsing has been succesfull.
	 */

      (fts_lex_parse_keywords(&p, values) || 
       fts_lex_parse_long(&p, values)    || 
       fts_lex_parse_float(&p, values)   || 
       fts_lex_parse_qstring(&p, values) || 
       fts_lex_parse_string(&p, values));
    }
}



	

