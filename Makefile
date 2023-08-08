CC = g++
INCFLAGS = -I./glm-0.9.7.1 -I./include/

RM = /bin/rm -f 
all:
	$(CC) $(CFLAGS) -o raytrace main.cpp transform.cpp transform.h geometry.h geometry.cpp scene.h scene.cpp $(INCFLAGS) -lfreeimage
clean: 
	$(RM) *.o raytrace *.png


 
