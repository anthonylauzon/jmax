/*
 * jMax
 * Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * See file LICENSE for further informations on licensing terms.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */
#ifdef WIN32
#include <windows.h>
#include <direct.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include <jni.h>
#include <stdio.h>

/* type definitions for the shared libraries */
#ifdef WIN32
typedef HINSTANCE jmax_dl_t;
typedef FARPROC jmax_dlsymbol_t;
#else 
typedef void* jmax_dl_t;
typedef void* jmax_dlsymbol_t;
#endif

/* common public functions */
int jmax_run(int argc, char** argv);
void jmax_set_root(char* s);
char* jmax_get_root(void);

/* common private functions */
static void jmax_get_default_jvm(char* buffer, int size);
static void jmax_append_jvm(char* path);
static void jmax_free_available_jvm(void);
static char* jmax_get_jvm(int i);
static char* jmax_get_classpath(char* path);
static int jmax_log(const char *format, ...);
jint JNICALL jmax_vfprintf(FILE *fp, const char *format, va_list args);

/* platform specific functions */
static void jmax_init_root(void);
static void jmax_get_available_jvm(void);
static jmax_dl_t jmax_dl_load(char* path);
static jmax_dlsymbol_t jmax_dl_symbol(jmax_dl_t lib, char* name);
static void jmax_dl_close(jmax_dl_t lib);
static int jmax_vlog(const char *format, va_list args);

/* some macros to make the code more portable */
#ifdef WIN32
#define PATH_SEPARATOR     ';'
#define FILE_SEPARATOR     '\\'
#define JVM_LIBRARY        "jvm.dll"
#define snprintf           _snprintf
#else
#define PATH_SEPARATOR     ':'
#define FILE_SEPARATOR     '/'
#define JVM_LIBRARY        "libjvm.so"
#endif

/* signatures of the JVM entry functions */
typedef jint (JNICALL *jni_init_args_t)(void *);
typedef jint (JNICALL *jni_create_jvm_t)(JavaVM **, void **, void *);

/* global variables */
#define MAX_JVM  8
static char* jmax_classpath = NULL;
static char* jmax_jvm[MAX_JVM];
static int jmax_num_jvm = 0;
static char* jmax_root = NULL;

/*
 * start the JVM and run MaxApplication
 *
 */
int jmax_run(int argc, char** argv)
{ 
  JDK1_1InitArgs vm_args;
  JavaVM *jvm;
  JNIEnv *env;
  jclass cls;
  jmethodID mid;
  jobjectArray args;
  jstring jstr;
  char* classpath;
  char* library;
  jni_init_args_t init_args;
  jni_create_jvm_t create_jvm;
  jmax_dl_t handle;
  int i, err;

  /* find the available virtual machines */
  jmax_get_available_jvm();

  /* load the jvm library */
  for (i = 0; ; i++) {

    library = jmax_get_jvm(i);

    if (library == NULL) {
      return -2;
    }

    handle = jmax_dl_load(library);
    if (handle == NULL) {
      continue;
    }

    init_args  = (jni_init_args_t) jmax_dl_symbol(handle, "JNI_GetDefaultJavaVMInitArgs");
    create_jvm  = (jni_create_jvm_t) jmax_dl_symbol(handle, "JNI_CreateJavaVM");

    if ((init_args != NULL) && (create_jvm != NULL)) {
      break;
    }

    jmax_dl_close(handle);
  }

  /* get the default initialization arguments and set the class 
   * path */
  vm_args.version = 0x00010001; /* New in 1.1.2: VM version */
  init_args((void*) &vm_args);

  classpath = jmax_get_classpath(vm_args.classpath);
  if (classpath == NULL) {
    return -1;
  }
  vm_args.classpath = classpath;

  vm_args.vfprintf = jmax_vfprintf;

  /* load and initialize a Java VM, return a JNI interface 
   * pointer in env */
  err = create_jvm(&jvm, (void**) &env, (void*) &vm_args);
  if (err != 0) {
    return -3;
  }

  /* get a reference to the MaxApplication class */
  cls = (*env)->FindClass(env, "ircam/jmax/MaxApplication");
  if (cls == NULL) {
    return -4;
  }

  /* get a reference to the main function */
  mid = (*env)->GetStaticMethodID(env, cls, "main", "([Ljava/lang/String;)V");
  if (mid == NULL) {
    return -5;
  }

  /* create the array of string arguments */
  args = (*env)->NewObjectArray(env, argc, (*env)->FindClass(env, "java/lang/String"), NULL);
  if (args == 0) {
    return -1;
  }

  for (i = 0; i < argc; i++) {
    jstr = (*env)->NewStringUTF(env, argv[i]);
    if (jstr == 0) {
      return -1;
    }
    (*env)->SetObjectArrayElement(env, args, i, jstr);
  }

  /* invoke the MaxApplication.main method using the JNI */
  (*env)->CallStaticVoidMethod(env, cls, mid, args);
	
  /* cleanup */
  (*jvm)->DestroyJavaVM(jvm);

  jmax_dl_close(handle);

  jmax_free_available_jvm();

  return 0;
} 

static char* 
jmax_get_classpath(char* path)
{
  char* root;

  if (jmax_classpath != NULL) {
    return jmax_classpath;
  }

  root = jmax_get_root();
  if (root == NULL) {
    return NULL;
  }

  jmax_classpath = (char*) malloc(strlen(path) + 1 + 3 * (strlen(root) + 24));
  if (jmax_classpath == NULL) {
    return NULL;
  }

  sprintf(jmax_classpath, "%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s%c%s", 
	  path, PATH_SEPARATOR,
	  root, FILE_SEPARATOR, "java", FILE_SEPARATOR, "jmax.jar", PATH_SEPARATOR,
	  root, FILE_SEPARATOR, "java", FILE_SEPARATOR, "jacl.jar", PATH_SEPARATOR,
	  root, FILE_SEPARATOR, "java", FILE_SEPARATOR, "tcljava.jar");

  return jmax_classpath;
}

static void 
jmax_free_available_jvm(void)
{
  int i = 0;

  for (i = 0; i < jmax_num_jvm; i++) {
    if (jmax_jvm[i] != NULL) {
      free(jmax_jvm[i]);
    }
  }
}


static char* 
jmax_get_jvm(int i)
{
  return (i < jmax_num_jvm)? jmax_jvm[i] : NULL;
}

static void 
jmax_get_default_jvm(char* buffer, int size)
{
  snprintf(buffer, 256, "%s%c%s%c%s%c%s%c%s", 
	   jmax_root, FILE_SEPARATOR, "jre", FILE_SEPARATOR, "bin", FILE_SEPARATOR, 
	   "classic", FILE_SEPARATOR, JVM_LIBRARY);
}

static void jmax_append_jvm(char* s)
{
  if (jmax_num_jvm < MAX_JVM) {
    jmax_jvm[jmax_num_jvm++] = strcpy((char*) malloc(strlen(s) + 1), s);  
  }
}

static char* 
jmax_get_root(void)
{
  if (jmax_root == NULL) {
    jmax_init_root();
  }
  return jmax_root;
}

static void 
jmax_set_root(char* s)
{
  if (jmax_root != NULL) {
    free(jmax_root);
  }
  if (s == NULL) {
    return;
  }
  jmax_root = strcpy((char*) malloc(strlen(s) + 1), s);
}

static int 
jmax_log(const char *format, ...)
{
  va_list args; 
  int r = -1;

  va_start (args, format); 
  r = jmax_vlog(format, args); 
  va_end (args); 
  return r;
}

jint JNICALL
jmax_vfprintf(FILE *fp, const char *format, va_list args)
{
  return jmax_vlog(format, args);
}


#ifdef WIN32

/***************************************************************
 *
 *   Platform dependent function for the Windows platform
 *
 */

#define JMAX_KEY        "Software\\Ircam\\jMax\\3.0" 
#define IBM_RELEASE_13  "1.3" 
#define IBM_JRE_13	"Software\\IBM\\Java2 Runtime Environment"
#define SUN_RELEASE_13  "1.3" 
#define SUN_JRE_13	"Software\\JavaSoft\\Java Runtime Environment"
#define LOG_FILE        "C:\\jmax-log.txt"

static int jmax_get_jvm_from_registry(char *buf, jint bufsize, char* key, char* release);
static int jmax_get_root_from_registry(char *buf, jint bufsize);
static int jmax_get_string_from_registry(HKEY key, const char *name, char *buf, jint bufsize);

static jmax_dl_t 
jmax_dl_load(char* path)
{
  return LoadLibrary(path);
}

static jmax_dlsymbol_t 
jmax_dl_symbol(jmax_dl_t lib, char* name)
{
  return GetProcAddress(lib, name);
}

static void 
jmax_dl_close(jmax_dl_t lib)
{
  FreeLibrary(lib);
}

static void 
jmax_init_root(void)
{
  char root[_MAX_PATH];
  int i;

  /* first check the registry */
  if (!jmax_get_root_from_registry(root, _MAX_PATH)) {

    /* otherwise, calculate the root from the current directory */
    if (_getcwd(root, _MAX_PATH) == NULL) {
      return;
    }
    /* move one directory up */
    i = strlen(root);
    while (--i >= 0) {
      if (root[i] == FILE_SEPARATOR) {
	root[i] = 0;
	break;
      }
    }
  }

  jmax_set_root(root);
}

static void 
jmax_get_available_jvm(void)
{
  int i = 0;
  char buf[_MAX_PATH];
  struct _stat statbuf;

  jmax_num_jvm = 0;
  for (i = 0; i < MAX_JVM; i++) {
    jmax_jvm[i] = NULL;
  }

  /* first check whether the default jre in the jmax directory exists */
  jmax_get_default_jvm(buf, _MAX_PATH);
  if (_stat(buf, &statbuf) == 0) {
    jmax_append_jvm(buf);
  }
  
  /* check in the windows registry for IBM's and Sun's JVMs */
  if (jmax_get_jvm_from_registry(buf, _MAX_PATH, IBM_JRE_13, IBM_RELEASE_13)) {
    if (_stat(buf, &statbuf) == 0) {
      jmax_append_jvm(buf);
    }
  }

  if (jmax_get_jvm_from_registry(buf, _MAX_PATH, SUN_JRE_13, SUN_RELEASE_13)) {
    if (_stat(buf, &statbuf) == 0) {
      jmax_append_jvm(buf);
    }
  }
}

static int
jmax_get_string_from_registry(HKEY key, const char *name, char *buf, jint bufsize)
{
  DWORD type, size;
  
  if (RegQueryValueEx(key, name, 0, &type, 0, &size) == 0
      && type == REG_SZ
      && (size < (unsigned int)bufsize)) {
    if (RegQueryValueEx(key, name, 0, 0, buf, &size) == 0) {
      return 1;
    }
  }
  return 0;
}

static int 
jmax_get_root_from_registry(char *buf, jint bufsize)
{
  HKEY key;

  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, JMAX_KEY, 0, KEY_READ, &key) != 0) {
    jmax_log("Error opening registry key '%s'\n", JMAX_KEY);
    return 0;
  }
  
  if (!jmax_get_string_from_registry(key, "jmaxRoot", buf, bufsize)) {
    jmax_log("Failed reading value of registry key:\n\t%s\\jmaxRoot\n", JMAX_KEY);
    RegCloseKey(key);
    return 0;
  }

  return 1;
}

static int
jmax_get_jvm_from_registry(char *buf, jint bufsize, char* jre, char* release)
{
  HKEY key, subkey;
  char version[_MAX_PATH];
  
  /* Find the current version of the JRE */
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, jre, 0, KEY_READ, &key) != 0) {
    jmax_log("Error opening registry key '%s'\n", jre);
    return 0;
  }
  
  if (!jmax_get_string_from_registry(key, "CurrentVersion", version, sizeof(version))) {
    jmax_log("Failed reading value of registry key:\n\t%s\\CurrentVersion\n", jre);
    RegCloseKey(key);
    return 0;
  }
  
  if (strcmp(version, release) != 0) {
    jmax_log("Registry key '%s\\CurrentVersion'\nhas value '%s', but '%s' is required.\n", jre, version, release);
    RegCloseKey(key);
    return 0;
  }
  
  /* Find directory where the current version is installed. */
  if (RegOpenKeyEx(key, version, 0, KEY_READ, &subkey) != 0) {
    jmax_log("Error opening registry key '%s\\%s'\n", jre, version);
    RegCloseKey(key);
    return 0;
  }
  
  if (!jmax_get_string_from_registry(subkey, "RuntimeLib", buf, bufsize)) {
    jmax_log("Failed reading value of registry key:\n\t%s\\%s\\JavaHome\n", jre, version);
    RegCloseKey(key);
    RegCloseKey(subkey);
    return 0;
  }
  
  RegCloseKey(key);
  RegCloseKey(subkey);
  return 1;
}

static int 
jmax_vlog(const char *format, va_list args)
{
  FILE* log;
  int r = -1;

  log = fopen(LOG_FILE, "ab");
  if (log) {
    r = vfprintf(log, format, args); 
    fclose(log);
  }
  return r;
}

static void
jmax_new_log()
{
  /* FIXME */
  FILE* log = fopen(LOG_FILE, "wb");
  if (log) {
    fclose(log);  
  }
}


#ifdef HAVE_JMAX_MAIN

#define is_white(_c)   (strchr(" \t\n\r", _c) != NULL)
#define is_black(_c)   (strchr(" \t\n\r", _c) == NULL)
#define is_quote(_c)   ((_c) == '"')

/*  main function of the Win32 version */
int PASCAL 
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, 
	LPSTR lpszCmdLine, int nCmdShow) 
{
#define MAX_ARGC  64
  char* argv[MAX_ARGC];
  int argc = 0;
  int err;
  char* mess = NULL;
  char* s;
  int state = 0;  
  int i;

  if (hPrevInstance != NULL) {
    MessageBox(NULL, "A jMax application is already running", "jMax", MB_OK | MB_ICONSTOP | MB_APPLMODAL); 
    return 0;
  }

  /* the default arguments */
  argv[argc++] = "-jmaxRoot";
  argv[argc++] = jmax_get_root();
  argv[argc++] = "-jmaxConnection";
  argv[argc++] = "tcp";
  argv[argc++] = "-jmaxPort";
  argv[argc++] = "2023";
  argv[argc++] = "-jmaxInterp";
  argv[argc++] = "silk";

  jmax_new_log();

  /* tokenize the command line and append them to the command line */
  s = lpszCmdLine;

  /* state 0 = in white, 
     state 1 = in unquoted string, 
     state 2 = in quoted string */
  while (*s != 0) {
    switch (state) {
    case 0:
      if (is_quote(*s)) {
	argv[argc++] = s + 1;
	state = 2;
      } else if (is_black(*s)) {
	argv[argc++] = s;
	state = 1;	
      }
      break;

    case 1:
      if (is_quote(*s)) {
	*s = 0;
	argv[argc++] = s + 1;
	state = 2;
      } else if (is_white(*s)) {
	*s = 0;
	state = 0;	
      }
      break;

    case 2:
      if (is_quote(*s)) {
	*s = 0;
	state = 0;
      }
      break;
    }
    s++;
  }
  argv[argc] = NULL;

  /* print out the command line in the log file */
  jmax_log("jmax ");
  for (i = 0; i < argc; i++) {
    jmax_log("%s ", argv[i]);
  }
  jmax_log("\n");

  /* run jmax and inform the user if an error occured */
  err = jmax_run(argc, argv);
  
  switch (err) {
  case -1: 
    mess = "Out of memory";
    break;
  case -2: 
    mess = "Couldn't find an installed Java Virtual Machine";
    break;
  case -3: 
    mess = "Couldn't instantiate the Java Virtual Machine";
    break;
  case -4: 
    mess = "Couldn't find the MaxApplication class";
    break;
  case -5: 
    mess = "Couldn't find the MaxApplication main method";
    break;
  default:
    mess = "Unknown error";
    break;
  }

  if (mess) {
    MessageBox(NULL, mess, "jMax", MB_OK | MB_ICONSTOP | MB_APPLMODAL); 
  }

  return 0;
}
#endif /* HAVE_JMAX_MAIN */


#else
#error Platform dependent functions not yet implemented
#endif
