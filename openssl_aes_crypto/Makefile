GIT_COMMIT_HASH = $(shell git log -1 --pretty=format:"%h")
GIT_COMMIT_VERSION = $(shell git describe --tags --abbrev=0)

CFLAGS += -DVSAAS_VERSION=\"$(GIT_COMMIT_VERSION)-$(GIT_COMMIT_HASH)\"
LDFLAGS = -Os

CC		= gcc
CX		= g++ -std=c++11 
LIBNAME		= main
PROGS		= main
OUTPUT		= $(LIBNAME)
OUTPUTDIR 	= ../debug
LIBS 		+= -lssl -lcrypto
all:$(PROGS)
	@du -c -h $(OUTPUT)
	make install

install:
	mkdir -p $(OUTPUTDIR)
	mv $(OUTPUT) $(OUTPUTDIR)

clean:
	rm -rf `find . -name "*.o"`
	rm -rf *.o
	rm -rf v/*.o

$(PROGS):
	$(CX) $(LDFLAGS) -o $(OUTPUT) $^ $(LIBS)
%.o: %.cpp
	$(CX) $(CFLAGS) -c -o $@ $<
%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

#############################################
$(PROGS) +: main.o
$(PROGS) +: vcrypto.o
$(PROGS) +: ossl_evp.o
$(PROGS) +: ossl_aes.o
