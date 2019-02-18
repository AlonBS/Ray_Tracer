INCFLAGS  = -I /usr/include/GL -I ./utilities/include -I /usr/include/boost

#LINKFLAGS  = -lfreeimage -lGLEW -L /usr/lib/x86_64-linux-gnu/ -lboost_filesystem -lboost_system -lassimp
LINKFLAGS  = -lassimp -lfreeimage -lGLEW -L /usr/lib/x86_64-linux-gnu/ \
	     -lboost_filesystem -lboost_system -lboost_program_options 


CFLAGS    = -Wall -Wextra -g -std=c++17 -pthread #-fopenmp 
CC        = g++
SRCS      = main.cpp \
	    Image.cpp \
	    Ray.cpp \
	    RayTracer.cpp \
	    Scene.cpp \
            SceneParser.cpp \
            Object.cpp \
            Sphere.cpp \
            Triangle.cpp \
            Lights.cpp \
            Mesh.cpp \
            Model.cpp
            
	    

OBJS      = $(SRCS:.cpp=.o)
PROG      = ray_tracer

all: $(SRCS) $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(INCFLAGS) $(LINKFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -c -o $@ $(INCFLAGS)

depend:
	makedepend $(INCFLAGS) -Y $(SRCS)

clean:
	rm -rf $(OBJS) $(PROG)

