lib_LTLIBRARIES= libfts.la

libfts_la_SOURCES= \
@PLATFORM@.c \
abstraction.c \
array.c \
atom.c \
atomfile.c \
audio.c \
audioconfig.c \
audiofile.c \
autosave.c \
binaryprotocol.c \
bmaxfile.c \
bytestream.c \
class.c \
client.c \
clientmanager.c \
clipboard.c \
config.c \
connection.c \
define.c \
dsp.c \
dspgraph.c \
dumper.c \
errobj.c \
expression.c \
fifo.c \
file.c \
fpe.c \
ftl.c \
ftlmem.c \
fts.c \
function.c \
hashtable.c \
label.c \
list.c \
loader.c \
mem.c \
message.c \
midi.c \
midievent.c \
midifile.c \
object.c \
objectlist.c \
objectset.c \
outlet.c \
package.c \
param.c \
patcher.c \
patcherobject.c \
patfile.c \
pipestream.c \
post.c \
preset.c \
project.c \
sched.c \
selection.c \
sigconn.c \
socketstream.c \
stack.c \
status.c \
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

BUILT_SOURCES=parser.h
noinst_HEADERS= parser.h

bin_PROGRAMS=fts
fts_SOURCES=main.c
fts_LDADD= @DOT_LIBS@ $(top_builddir)/fts/libfts.la @FTS_SYS_LIBS@ -lm
libfts_la_LDFLAGS = -no-undefined -release @JMAX_VERSION@ `pkg-config --libs sndfile`
AM_YFLAGS=-d --verbose
AM_LFLAGS=-olex.yy.c
EXTRA_DIST=linux.c sgi.c macosx.c win32.c

INCLUDES=-I$(top_srcdir)/include `pkg-config --cflags-only-I sndfile`
AM_CFLAGS=`pkg-config --cflags-only-other sndfile`


# explicit dependency between expression.c and parser.h 
# for first checkout
expression.c: parser.h


