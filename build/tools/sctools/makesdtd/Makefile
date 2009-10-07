
OPENSSL_DIR = c:/OpenSSL/openssl-1.0.0-beta2


PACKAGE	= makesdtd

SRCS	= makesdtd.c my_sign.c

HEADS	= my_sign.h


OBJS	= $(SRCS:.c=.o)

CC	:= C:/Cygwin/bin/gcc


CFLAGS	= -mno-cygwin  -Wall


CPPFLAGS= -I. -I$(OPENSSL_DIR)/include -I$(OPENSSL_DIR)/crypto/ec 

LD	= gcc
LDFLAGS	= -Wl,--subsystem,console -mwindows -mno-cygwin -L$(OPENSSL_DIR)
LDLIBS	= -lcrypto -lssl

RM = rm.exe -f

.SUFFIXES:
.SUFFIXES: .o .c

all: install

install: $(PACKAGE)
	install -c -m 777 $(PACKAGE) ../bin

$(PACKAGE): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)

$(OBJS): $(HEADS) Makefile

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@


.PHONY: clean clobber
clean clobber:
	$(RM) $(OBJS) $(PACKAGE).exe

