lib_LTLIBRARIES= libfts.la

libfts_la_SOURCES= \
@PLATFORM@.c \
abstraction.c \
array.c \
atom.c \
atomfile.c \
audio.c \
audioconfig.c \
audiolabel.c \
audiofile.c \
autosave.c \
bmaxfile.c \
bytestream.c \
class.c \
client.c \
clipboard.c \
config.c \
connection.c \
dsp.c \
dspgraph.c \
errobj.c \
expression.c \
fifo.c \
file.c \
fpe.c \
ftl.c \
ftlmem.c \
fts.c \
hashtable.c \
label.c \
list.c \
mem.c \
message.c \
midi.c \
midifile.c \
object.c \
objectlist.c \
objectset.c \
package.c \
param.c \
patcher.c \
patfile.c \
post.c \
preset.c \
project.c \
property.c \
sched.c \
selection.c \
sigconn.c \
stack.c \
symbol.c \
template.c \
thread.c \
time.c \
tuple.c \
update.c \
variable.c \
version.c \
parser.y \
tokenizer.l


noinst_HEADERS= parser.h

bin_PROGRAMS=fts
fts_SOURCES=main.c
fts_LDADD= @DOT_LIBS@ -lfts @FTS_SYS_LIBS@ -lm
libfts_la_LDFLAGS = -no-undefined -release @JMAX_VERSION@
AM_YFLAGS=-d --verbose
AM_LFLAGS=-olex.yy.c
EXTRA_DIST=linux.c sgi.c macosx.c win32.c

INCLUDES=-I$(top_srcdir)/include
AM_CFLAGS=

