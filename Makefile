INCFLAGS  = -I /usr/include/GL -I ./utilities/include -I /usr/include/boost

#LINKFLAGS  = -lfreeimage -lGLEW -L /usr/lib/x86_64-linux-gnu/ -lboost_filesystem -lboost_system -lassimp
LINKFLAGS  = -lassimp -lfreeimage -lGLEW -L /usr/lib/x86_64-linux-gnu/ \
	     -lboost_filesystem -lboost_system -lboost_program_options 


CFLAGS    = -Wall -Wextra -g -std=c++17 -pthread #-fopenmp 
CC        = g++
SRC_DIR   = ./src
SRCS      = $(SRC_DIR)/main.cpp \
	    $(SRC_DIR)/Image.cpp \
	    $(SRC_DIR)/Ray.cpp \
	    $(SRC_DIR)/RayTracer.cpp \
	    $(SRC_DIR)/Scene.cpp \
            $(SRC_DIR)/SceneParser.cpp \
            $(SRC_DIR)/Object.cpp \
            $(SRC_DIR)/Sphere.cpp \
            $(SRC_DIR)/Triangle.cpp \
            $(SRC_DIR)/Lights.cpp \
            $(SRC_DIR)/Mesh.cpp \
            $(SRC_DIR)/Model.cpp
            
	    

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

