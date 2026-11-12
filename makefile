CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
  # Homebrew prefix: /opt/homebrew on Apple Silicon, /usr/local on Intel
  BREW_PREFIX := $(shell brew --prefix 2>/dev/null || echo /opt/homebrew)
  INCLUDES    := -I$(BREW_PREFIX)/include
  LIBS        := -L$(BREW_PREFIX)/lib -lsfml-graphics -lsfml-window -lsfml-system \
                 -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
else
  # Linux / other Unix: let pkg-config find SFML, fall back to plain -l flags
  PKG_OK   := $(shell pkg-config --exists sfml-graphics && echo yes)
  ifeq ($(PKG_OK),yes)
    INCLUDES := $(shell pkg-config --cflags sfml-graphics)
    LIBS     := $(shell pkg-config --libs sfml-graphics)
  else
    INCLUDES :=
    LIBS     := -lsfml-graphics -lsfml-window -lsfml-system
  endif
  LIBS += -pthread
endif

TARGET := app
SRCS   := main.cpp Moves.cpp simulateMoves.cpp minimax.cpp gui.cpp UCI/UciOut.cpp
OBJS   := $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LIBS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all run clean
