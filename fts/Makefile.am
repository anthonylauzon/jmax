lib_LTLIBRARIES= libfts.la

libfts_la_SOURCES= \
@PLATFORM@.c \
OLDclient.c \
OLDftsdata.c \
OLDpatcherdata.c \
OLDucs.c \
abstraction.c \
array.c \
atom.c \
atomfile.c \
audio.c \
autosave.c \
class.c \
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
inout.c \
loader.c \
mem.c \
message.c \
midi.c \
midifile.c \
object.c \
objectlist.c \
objectset.c \
objtable.c \
param.c \
patcher.c \
patparser.c \
post.c \
property.c \
saver.c \
sched.c \
selection.c \
sigconn.c \
soundfile.c \
soundfile_af.c \
srconv.c \
symbol.c \
template.c \
time.c \
variable.c \
version.c \
vm.c 

bin_PROGRAMS=fts
fts_SOURCES=main.c
fts_LDADD=-L.libs -lfts -ldl -laudiofile -lm
fts_LDFLAGS=-rpath @ABS_TOP_SRCDIR@/fts/.libs


INCLUDES=-I$(top_srcdir)/include
