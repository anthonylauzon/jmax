# To recompile, make ARCH=<architecture>
#	where architecture can be:
#		irix65r10k for IRIX6.5 R10000 processor
#		irix65r4k for IRIX6.5 R4000 processor
#		linuxpc for Linux-x86

# Change ROOTDIR according to your installation
ROOTDIR=../../../..

LIB_PROTOCOL=$(ROOTDIR)/lib/protocol/c/lib/$(ARCH)/libProtocol.a

BINS=udptest udpmessagetest
CFLAGS=-I$(ROOTDIR)/lib/protocol/c/src

all: $(BINS)

udptest: udptest.c
	$(CC) $(CFLAGS) -o udptest udptest.c $(LIB_PROTOCOL)

udpmessagetest: udpmessagetest.c
	$(CC) $(CFLAGS) -o udpmessagetest udpmessagetest.c $(LIB_PROTOCOL)

clean:
	-rm $(BINS)

