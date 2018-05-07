TARGETS = \
    run

all All: $(TARGETS)

INCPATH = -I/usr/local/include
LIBPATH = -L. -L/usr/local/lib/hpux32
LIBS = -lglut -lGLU -lGL -lglut -lXt -lXext -lX11 -ljpeg -lm
CFLAGS = $(INCPATH)

SRCS = \
	main.c stl.c colors.c text.c helpers.c InitJPG.c

OBJS = $(SRCS:.c=.o)

.SUFFIXES:	.c

.c.o:	
	cc -c $< $(CFLAGS)


run:  clean $(OBJS)
	LDOPTS= \
	cc -o run $(CFLAGS) $(OBJS) $(LIBPATH) $(LIBS)

clean:
	rm -f run $(OBJS) core

print:
	listing main.c
