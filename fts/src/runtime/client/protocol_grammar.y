%token STRING_CODE
%token STRING_END_CODE
%token INT_CODE
%token FLOAT_CODE
%token SYMBOL_CACHED_CODE
%token SYMBOL_AND_DEF_CODE
%token SYMBOL_CODE
%token OBJECT_CODE
%token CONNECTION_CODE
%token DATA_CODE
%token EOM_CODE
%token BYTE

%%
stream : message stream
	| 
	;

message : cmd args EOM_CODE;

cmd : BYTE ;

args: arg args
	|
	;

arg : int_arg
	| float_arg
	| symbol_cached_arg
	| symbol_and_def_arg
	| symbol_arg
	| string_arg
	| object_arg
	| connection_arg
	| data_arg
	;

int_arg: INT_CODE int;

int : BYTE BYTE BYTE BYTE;

float_arg: FLOAT_CODE int ;

symbol_cached_arg : SYMBOL_CACHED_CODE int ;

symbol_and_def_arg : SYMBOL_AND_DEF_CODE int string STRING_END_CODE ;

string : string BYTE 
	|
	;

symbol_arg : SYMBOL_CODE string STRING_END_CODE ;

string_arg : STRING_CODE string STRING_END_CODE ;

object_arg : OBJECT_CODE int ;

connection_arg : CONNECTION_CODE int ;

data_arg : DATA_CODE int;
