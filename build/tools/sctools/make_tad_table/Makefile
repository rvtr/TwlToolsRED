PACKAGE	= make_tad_table

SRCS	= make_tad_table.c

HEADS	= 

OBJS	= $(SRCS:.c=.o)

CC	:= C:/Cygwin/bin/gcc


CFLAGS	= -mno-cygwin 


CPPFLAGS= -I. 

LD	= gcc
LDFLAGS	= -Wl,--subsystem,console -mwindows -mno-cygwin
LDLIBS	= 

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

