/* configfile.c
**
** This file contains a small config file reader/writer.
** The config file format is comparable to .ini files (but don't have sections).
** For more info see configfile.h
**
** Why do I have to do this in C? It's #$@*?!....
** Why didn't I use lex/yacc? : simply because I want 0 dependancies except the 
** standard C lib.
** Moreover having a lex/yacc grammar may lead people to extend
** the grammar of the file to suit their needs (as it would be easy).
** It's not the goal, as it's supposed to stay as simple as possible!
** If you need something more powerful, just use a parser generator such 
** as lex/yacc, or XML tools.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


// The following struct should be identical to the one in configfile.h
// We don't include configfile.h in order to avoid include path problems
// that may discourage the user from using this library.
typedef struct
{
    char *filePath;
    
} config_file_t;

#define MAX_LINE_LENGTH 1023

/* Helper functions to parse config files. */


/* @return 
** <0 : error or eof. *ppBuffer then points to '\0' or isn't modified.
**  0 : didn't find a token (but another char). *ppBuffer then points to the character
**      found (which is the first non-delimiter).
** >0 : the token found. *ppBuffer then points to the character just after the token.
*/
static int getNextToken(char **ppBuffer, const char *tokens, const char *delimiters)
{
    char c;
    char *pSrc;
    int i;
    int numTokens, numDelimiters;
    
    /* Some pre-conditions */
    if(ppBuffer == NULL)
        return -2;
    if(tokens == NULL)
        return -2;
    if(delimiters == NULL)
        numDelimiters = 0;
    
    numTokens = strlen(tokens);
    
    if(delimiters == NULL)
        numDelimiters = 0;
    else
        numDelimiters = strlen(delimiters);
    
    pSrc = *ppBuffer;
    
    while (1) {
        /* If you don't like labels (in such cases) don't code in C ;o) */
label_continue_while:;
                     
         c = *pSrc;
         
         for (i=0; i<numDelimiters; i++) {
             if( c == delimiters[i] ) {
                 pSrc++;
                 goto label_continue_while;
             }
         }
         
         if( c==0 ) {
             *ppBuffer = pSrc;
             return -1;
         }
         
         for (i=0; i<numTokens; i++) {
             if( c == tokens[i] ) {
                 *ppBuffer = (pSrc+1);
                 return c;
             }
         }
         
         *ppBuffer = pSrc;
         return 0;
    }
}


/* @return 
** <0 : error or eof. *ppBuffer then points to '\0' or isn't modified.
**  0 : didn't find an ident. *ppBuffer then points to the character
**      found (which is the first non-delimiter).
** >0 : the ident's length. *ppBuffer then points to the character just after the ident.
*/
static int getIdent(char **ppBuffer, const char *delimiters, char *ident, int maxIdentLen)
{
    char c;
    char *pSrc;
    char *pDest;
    int i;
    int numDelimiters;
    int identLen = 0;
    
    /* Some pre-conditions */
    if(ppBuffer == NULL)
        return -2;
    if(ident == NULL)
        return -2;
    if(maxIdentLen<1)
        return -2;
    
    pDest = ident;
    
    if(delimiters == NULL)
        numDelimiters = 0;
    else
        numDelimiters = strlen(delimiters);
    
    pSrc = *ppBuffer;
    
    while(1) {
        /* If you don't like labels (in such cases) don't code in C ;o) */
label_continue_while:;
                     
         c = *pSrc;
         
         if( c==0 ) {
             *ppBuffer = pSrc;
             *pDest = 0;
             return -1;
         }
         
         if( identLen == 0 ) {
             for (i=0; i<numDelimiters; i++) {
                 if( c == delimiters[i] ) {
                     pSrc++;
                     goto label_continue_while;
                 }
             }
             
             if(    ( (c>='a') && (c<='z') )
                 || ( (c>='A') && (c<='Z') )  
                 || (c=='_') 
                 ) 
             {
                 identLen = 1;
                 *(pDest++) = c;
                 pSrc++;
                 continue;
             }
             else { 
                 *ppBuffer = pSrc;
                 return 0;
             }
         } 
         else {
             if(    ( (c>='a') && (c<='z') )
                 || ( (c>='A') && (c<='Z') )
                 || ( (c>='0') && (c<='9') )  
                 || (c=='_')
                 )  
             {
                 identLen++;
                 *(pDest++) = c;
                 pSrc++;
                 continue;
             }
             else
             {
                 *pDest = 0;
                 *ppBuffer = pSrc;
                 return identLen;
             }
         }
    }
}


/* @return 
** <0 : error or eof. *ppBuffer then points to '\0' or isn't modified.
** >=0 : the string's length. *ppBuffer then points to the character just after the ident.
*/
static int getString(char **ppBuffer, const char *delimiters, char *string, int maxStringLen)
{
    char c;
    char *pSrc;
    char *pDest;
    int i;
    int numDelimiters;
    int stringLen = 0;
    int bInString = 0;
    
    /* Some pre-conditions */
    if(ppBuffer == NULL)
        return -2;
    if(string == NULL)
        return -2;
    if(maxStringLen<1)
        return -2;
    
    pDest = string;
    
    if(delimiters == NULL)
        numDelimiters = 0;
    else
        numDelimiters = strlen(delimiters);
    
    pSrc = *ppBuffer;
    
    // Parse stuff before string
    while(1) {
        /* If you don't like labels (in such cases) don't code in C ;o) */
label_continue_1st_while:;
                         
         c = *pSrc;
         
         if( c==0 ) {
             *ppBuffer = pSrc;
             return -1;
         }
         
         if( c=='\"' ) {
             pSrc++;
             break;
         }
         
         for (i=0; i<numDelimiters; i++) {
             if( c == delimiters[i] ) {
                 pSrc++;
                 goto label_continue_1st_while;
             }
         }
         
         *ppBuffer = pSrc;
         return -4;
    }
    
    // Parse the string itself
    while(1) {
        
        c = *pSrc;
        
        switch(c) {
        case '\"':
            *ppBuffer = pSrc+1;
            *pDest = 0;
            return stringLen;
        case 0:
            *ppBuffer = pSrc;
            *pDest = 0;
            return -3;
        case '\\':
            c = *(++pSrc);
            if( c==0 ) {
                *ppBuffer = pSrc;
                *pDest = 0;
                return -3;
            }
            switch(c) {
            case 'n':
            case 'N':
                c = '\n';
            case 'r':
            case 'R':
                c = '\r';
            }
            // We continue to default case deliberately
            default:
                *(pDest++) = c;
                stringLen++;
                pSrc++;
                continue;
        }
    }
}


config_file_t *l_config_file = NULL;
const char *l_tokens=";:\"=";
const char *l_delimiters=" \t";


config_file_t *config_file_open(const char *filePath)
{
    config_file_t *cfgfile;
    FILE *f;
    f=fopen(filePath, "r");
    if(f==NULL)
        return NULL;
    fclose(f);
    cfgfile = (config_file_t *)malloc(sizeof(config_file_t));
    cfgfile->filePath = (char *) filePath;
    return cfgfile;
}


void config_file_close(config_file_t *pConfigFile)
{
    free(pConfigFile);
}


int config_file_get_int(config_file_t *pConfigFile, const char *name, int *pValue)
{
    FILE *f;
    char buffer[MAX_LINE_LENGTH + 1];
    char key[MAX_LINE_LENGTH + 1];
    
    f = fopen(pConfigFile->filePath , "rt");
    if (f==NULL)
        return -3;
    
    while(fgets(buffer, MAX_LINE_LENGTH, f) != NULL) {
        
        char *pSrc = buffer;
        int c, res;
        
        c = getNextToken(&pSrc, l_tokens, l_delimiters);
        if(c == ';')
            continue;
        
        if(c==0) {
            
            res = getIdent(&pSrc, l_delimiters, key, MAX_LINE_LENGTH);
            if(res<=0) {
                continue;
            }
            
            if(!strcmp(key, name)) {
                
                c = getNextToken(&pSrc, l_tokens, l_delimiters);
                
                if( (c=='=') || (c==':') ) {
                    
                    if(sscanf(pSrc, "%d", pValue) == 1) {
                        fclose(f);
                        return 0;
                    }
                }
                else {
                    fclose(f);
                    return -2;
                }
            }
        }
    }
    
    return -1;
}


int config_file_get_bool(config_file_t *pConfigFile, const char *name, int *pValue)
{
    FILE *f;
    char buffer[MAX_LINE_LENGTH + 1];
    char key[MAX_LINE_LENGTH + 1];
    
    f = fopen(pConfigFile->filePath , "rt");
    if (f==NULL)
        return -3;
    
    while(fgets(buffer, MAX_LINE_LENGTH, f) != NULL) {
        
        char *pSrc = buffer;
        int c, res;
        
        c = getNextToken(&pSrc, l_tokens, l_delimiters);
        if(c == ';')
            continue;
        
        if(c==0) {
            
            res = getIdent(&pSrc, l_delimiters, key, MAX_LINE_LENGTH);
            if(res<=0) {
                continue;
            }
            
            if(!strcmp(key, name)) {
                
                c = getNextToken(&pSrc, l_tokens, l_delimiters);
                
                if( (c=='=') || (c==':') ) {
                    int theValue;
                    
                    res = getIdent(&pSrc, l_delimiters, key, MAX_LINE_LENGTH);
                    if(res<=0) {
                        fclose(f);
                        return -2;
                    }
                    else {
                        
                        if(key[1] == 0) {
                            switch(key[0]) {
                                case 't':
                                case 'T':
                                case '1':
                                case 'y':
                                case 'Y':
                                    theValue = 1;
                                    break;
                                case 'f':
                                case 'F':
                                case '0':
                                case 'n':
                                case 'N':
                                    theValue = 0;
                                    break;
                                default:
                                    fclose(f);
                                    return -2;
                            }
                        }
                        else {
                            switch(key[0]) {
                                case 't':
                                    if(!strcmp(key + 1, "rue")) {
                                        theValue = 1;
                                        break;
                                    }
                                    fclose(f);
                                    return -2;
                                case 'T':
                                    if(!strcmp(key + 1, "rue")) {
                                        theValue = 1;
                                        break;
                                    }
                                    if(!strcmp(key + 1, "RUE")) {
                                        theValue = 1;
                                        break;
                                    }
                                    fclose(f);
                                    return -2;
                                case 'y':
                                    if(!strcmp(key + 1, "es")) {
                                        theValue = 1;
                                        break;
                                    }
                                    fclose(f);
                                    return -2;
                                case 'Y':
                                    if(!strcmp(key + 1, "es")) {
                                        theValue = 1;
                                        break;
                                    }
                                    if(!strcmp(key + 1, "ES")) {
                                        theValue = 1;
                                        break;
                                    }
                                    fclose(f);
                                    return -2;
                                case 'f':
                                    if(!strcmp(key + 1, "alse")) {
                                        theValue = 0;
                                        break;
                                    }
                                    fclose(f);
                                    return -2;
                                case 'F':
                                    if(!strcmp(key + 1, "alse")) {
                                        theValue = 0;
                                        break;
                                    }
                                    if(!strcmp(key + 1, "ALSE")) {
                                        theValue = 0;
                                        break;
                                    }
                                    fclose(f);
                                    return -2;
                                case 'n':
                                    if(!strcmp(key + 1, "o")) {
                                        theValue = 0;
                                        break;
                                    }
                                    fclose(f);
                                    return -2;
                                case 'N':
                                    if(!strcmp(key + 1, "o")) {
                                        theValue = 0;
                                        break;
                                    }
                                    if(!strcmp(key + 1, "O")) {
                                        theValue = 0;
                                        break;
                                    }
                                    fclose(f);
                                    return -2;
                                default:
                                    fclose(f);
                                    return -2;
                            }
                        }
                        
                        *pValue = theValue;
                        fclose(f);
                        return 0;
                    }
                }
                else {
                    fclose(f);
                    return -2;
                }
            }
        }
    }
    
    return -1;
}


int config_file_get_string(config_file_t *pConfigFile, const char *name,  char *pValue, int maxLen)
{
    FILE *f;
    char buffer[MAX_LINE_LENGTH + 1];
    char key[MAX_LINE_LENGTH + 1];
    
    f = fopen(pConfigFile->filePath , "rt");
    if (f==NULL)
        return -3;
    
    while(fgets(buffer, MAX_LINE_LENGTH, f) != NULL) {
        char *pSrc = buffer;
        int c, res;
        
        c = getNextToken(&pSrc, l_tokens, l_delimiters);
        if((c == ';') || (c<0))
            continue;
        
        if(c==0) {
            
            res = getIdent(&pSrc, l_delimiters, key, MAX_LINE_LENGTH);
            
            if(res<=0) {
                continue;
            }
            
            if(!strcmp(key, name)) {
                
                c = getNextToken(&pSrc, l_tokens, l_delimiters);
                
                if( (c=='=') || (c==':') ) {
                    
                    int len;
                    
                    if(len = getString(&pSrc, l_delimiters, pValue, maxLen)>=0) {
                        fclose(f);
                        return len;
                    }
                }
                else {
                    fclose(f);
                    return -2;
                }
            }
        }
    }
    
    return -1;
}


int config_file_get_float(config_file_t *pConfigFile, const char *name, float *pValue)
{
    FILE *f;
    char buffer[MAX_LINE_LENGTH + 1];
    char key[MAX_LINE_LENGTH + 1];
    
    f = fopen(pConfigFile->filePath , "rt");
    if (f==NULL)
        return -3;
    
    while(fgets(buffer, MAX_LINE_LENGTH, f) != NULL) {
        
        char *pSrc = buffer;
        int c, res;
        
        c = getNextToken(&pSrc, l_tokens, l_delimiters);
        if(c == ';')
            continue;
        
        if(c==0) {
            
            res = getIdent(&pSrc, l_delimiters, key, MAX_LINE_LENGTH);
            if(res<=0) {
                continue;
            }
            
            if(!strcmp(key, name)) {
                
                c = getNextToken(&pSrc, l_tokens, l_delimiters);
                
                if( (c=='=') || (c==':') ) {
                    
                    if(sscanf(pSrc, "%f", pValue) == 1) {
                        fclose(f);
                        return 0;
                    }
                }
                else {
                    fclose(f);
                    return -2;
                }
            }
        }
    }
    
    return -1;
}

