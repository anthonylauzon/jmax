lib_LTLIBRARIES= libfts.la

libfts_la_SOURCES= \
@PLATFORM@.c \
abstraction.c \
array.c \
atom.c \
atomfile.c \
audio.c \
audiofile.c \
autosave.c \
bytestream.c \
class.c \
client.c \
clipboard.c \
connection.c \
doctor.c \
dsp.c \
dspgraph.c \
errobj.c \
expression.c \
file.c \
fpe.c \
ftl.c \
ftlmem.c \
fts.c \
hashtable.c \
label.c \
list.c \
loader.c \
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
patparser.c \
post.c \
project.c \
property.c \
saver.c \
sched.c \
selection.c \
sigconn.c \
stack.c \
symbol.c \
template.c \
time.c \
tuple.c \
variable.c \
version.c \
vm.c 

bin_PROGRAMS=fts
fts_SOURCES=main.c
fts_LDADD= @DOT_LIBS@ -lfts @FTS_SYS_LIBS@ -lm
libfts_la_LDFLAGS = -no-undefined

EXTRA_DIST=linux.c sgi.c macosx.c win32.c

INCLUDES=-I$(top_srcdir)/include



