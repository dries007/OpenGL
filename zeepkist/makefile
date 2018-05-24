TARGETS = \
    run

all All: $(TARGETS)

INCPATH = -I/usr/local/include
LIBPATH = -L. -L/usr/local/lib/hpux32
LIBS = -lglut -lGLU -lGL -lGLw -lXt -lXext -lX11 -lm 
DEBUGFLAGS = +O3
CFLAGS = +DD32 $(DEBUGFLAGS) $(INCPATH)

SRCS = \
	main.c stl.c input.c text.c textures.c draw.c

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
