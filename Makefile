# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra

# Source files
SRCS = ipkcpc.cpp modules/protocols.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Executable
EXEC = ipkcpc

# Build rules
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(EXEC)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(EXEC)
