
CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

# Homebrew prefix (Apple Silicon). If you're on Intel, change to /usr/local
BREW_PREFIX := /opt/homebrew

INCLUDES := -I$(BREW_PREFIX)/include
LIBDIRS  := -L$(BREW_PREFIX)/lib

# SFML libs you need for graphics/window/system
SFML_LIBS := -lsfml-graphics -lsfml-window -lsfml-system

# macOS frameworks SFML relies on
FRAMEWORKS := -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

TARGET := app
SRCS   := main.cpp Moves.cpp simulateMoves.cpp minimax.cpp
OBJS   := $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LIBDIRS) $(SFML_LIBS) $(FRAMEWORKS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all run clean
