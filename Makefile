INCFLAGS  = -I /usr/include/GL -I ./utilities/include -I /usr/include/boost

#LINKFLAGS  = -lfreeimage -lGLEW -L /usr/lib/x86_64-linux-gnu/ -lboost_filesystem -lboost_system -lassimp
LINKFLAGS  = -lassimp -lfreeimage -lGLEW -L /usr/lib/x86_64-linux-gnu/ \
	     -lboost_filesystem -lboost_system -lboost_program_options 


CFLAGS    = -g -std=c++17 -pthread #-fopenmp -Wall -Wextra 
CC        = g++
SRC_DIR   = ./src
SRCS      = $(SRC_DIR)/main.cpp \
            $(SRC_DIR)/General.cpp \
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
            $(SRC_DIR)/Model.cpp \
            $(SRC_DIR)/Cylinder.cpp \
            $(SRC_DIR)/Box.cpp \
            $(SRC_DIR)/Cone.cpp \
            $(SRC_DIR)/Plain.cpp \
            $(SRC_DIR)/AABB.cpp \
            $(SRC_DIR)/BVH.cpp \


	    
                        
	    

OBJS      = $(SRCS:.cpp=.o)
PROG      = ray_tracer

all: $(SRCS) $(PROG)

$(PROG): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@ $(INCFLAGS) $(LINKFLAGS)

%.o: %.cpp %.h
#	$(CC) $(CFLAGS) $^ -c -o $@ $(INCFLAGS)
	$(CC) $(CFLAGS) $< -c -o $@ $(INCFLAGS)

depend:
	makedepend $(INCFLAGS) -Y $(SRCS)

clean:
	rm -rf $(OBJS) $(PROG)

