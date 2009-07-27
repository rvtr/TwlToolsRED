PACKAGE	= cryptopc

OPENSSL_DIR = c:/OpenSSL/openssl-1.0.0-beta2

# SRCS	= main.c
SRCS	= m.c

HEADS	= my_sign.h

OBJS	= $(SRCS:.c=.o)

CC	:= C:/Cygwin/bin/gcc


CFLAGS	= -mno-cygwin -Wall

# openssl-1.0.0-beta2

CPPFLAGS= -I. -I$(OPENSSL_DIR)/include -I$(OPENSSL_DIR)/crypto/ec 

LD	= C:/Cygwin/bin/gcc
LDFLAGS	= -Wl,--subsystem,console -mwindows -mno-cygwin -L$(OPENSSL_DIR)
LDLIBS	= -lcrypto -lssl


.SUFFIXES:
.SUFFIXES: .o .c

all: $(PACKAGE)

# install: $(PACKAGE)
#	install -c -m 777 $(PACKAGE) ../bin

$(PACKAGE): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)

$(OBJS): $(HEADS) Makefile

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@


.PHONY: clean clobber
clean clobber:
	$(RM) $(OBJS) $(PACKAGE).exe

