CXX = g++
CXXFLAGS = -g -Wall -pedantic -std=c++17

# Add any additional source files here
SRCS = main.cpp cacheSimFuncs.cpp cacheSimFuncs.h
OBJS = $(SRCS:.cpp=.o)
TARGET = csim

# When submitting to Gradescope, submit all .cpp and .h files,
# the Makefile, as well as README.txt
FILES_TO_SUBMIT = $(shell ls *.cpp *.h Makefile README.txt 2> /dev/null)

# Rule to compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@
 
# Executable target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Target to create a solution.zip file you can upload to Gradescope
.PHONY: solution.zip
solution.zip:
	zip -9r $@ $(FILES_TO_SUBMIT)
	
# Rule to clean the build directory
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: clean