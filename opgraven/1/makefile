#-*-makefile-*-

TARGETS = \
    staaf

all All: $(TARGETS)

################################################################################
INCPATH = -I/usr/local/include
LIBPATH = -L. -L/usr/local/lib/hpux32
LIBS = -lglut -lGLU -lGL -lGLw -lXt -lXext -lX11 -ljpeg -lm 
DEBUGFLAGS = +O3
CFLAGS = +DD32 $(DEBUGFLAGS) $(INCPATH)


SRCS = \
	staaf.c lees.c

OBJS = $(SRCS:.c=.o)

.SUFFIXES:	.c

.c.o:	
	cc -c $< $(CFLAGS)


staaf:  clean $(OBJS)
	LDOPTS= \
	cc -o staaf $(CFLAGS) $(OBJS) $(LIBPATH) $(LIBS) 

clean:
	rm -f staaf $(OBJS) core
