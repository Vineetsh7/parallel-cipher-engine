CXX = g++
# -pthread: required for std::thread, std::mutex, std::condition_variable on Linux/WSL2
CXXFLAGS = -std=c++17 -g -Wall -pthread \
           -I. -Isrc/core/cipher -Isrc/core/io -Isrc/core/scheduler

MAIN_TARGET = pce
CIPHER_TARGET = cipher

MAIN_SRC = engine.cpp \
           src/core/scheduler/task_scheduler.cpp \
           src/core/io/file_stream.cpp \
           src/core/io/env_config.cpp \
           src/core/cipher/cipher_engine.cpp

CIPHER_SRC = src/core/cipher/cipher_runner.cpp \
             src/core/cipher/cipher_engine.cpp \
             src/core/io/file_stream.cpp \
             src/core/io/env_config.cpp

MAIN_OBJ = $(MAIN_SRC:.cpp=.o)
CIPHER_OBJ = $(CIPHER_SRC:.cpp=.o)

all: $(MAIN_TARGET) $(CIPHER_TARGET)

$(MAIN_TARGET): $(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(CIPHER_TARGET): $(CIPHER_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(MAIN_OBJ) $(CIPHER_OBJ) $(MAIN_TARGET) $(CIPHER_TARGET)

.PHONY: clean all