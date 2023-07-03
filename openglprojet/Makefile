# Variables
CC = g++
INCLUDE_DIR = ./include
BULLET_DIR = /usr/local/include/bullet
LIBS = -lBulletDynamics -lBulletCollision -lLinearMath -lglfw
CPP_SOURCES = $(wildcard src/*.cpp)
C_SOURCES = src/glad.c
OBJECTS = $(CPP_SOURCES:.cpp=.o) $(C_SOURCES:.c=.o)
TARGET = main

# Default Target
all: $(TARGET)

# Rule to build the target
$(TARGET): $(OBJECTS)
	$(CC) -I$(INCLUDE_DIR) $^ $(LIBS) -o $@

# Rule to build object files from C++ sources
%.o: %.cpp
	$(CC) -I$(INCLUDE_DIR) -I$(BULLET_DIR) -c $< -o $@

# Rule to build object files from C sources
%.o: %.c
	$(CC) -I$(INCLUDE_DIR) -c $< -o $@

# Rule to clean intermediate files
clean:
	rm -f $(OBJECTS)

# Rule to clean everything
clean-all: clean
	rm -f $(TARGET)

