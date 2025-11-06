CXX = g++
CXXFLAGS = -Iinclude -Wall -std=c++17
LDFLAGS = -lglfw -ldl -lGL -lX11 -lpthread -lXrandr -lXi -lglut

SRC = src/glad.c \
      src/globals.cpp \
      src/input.cpp \
      src/mass.cpp \
      src/main.cpp \
      src/rendering.cpp \
      src/utils.cpp \
      src/window.cpp
OUT = build/SolarSim

$(OUT): $(SRC)
	$(CXX) $(SRC) $(CXXFLAGS) $(LDFLAGS) -o $(OUT)

clean:
	rm -f $(OUT)
