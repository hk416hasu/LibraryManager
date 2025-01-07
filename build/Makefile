# compiler
cc = g++

# compiler flags
CFLAGS =

# Target executable
TARGET = ./main

# source files
SRCS = main.c guanxin.cpp yixuan.cpp ran.cpp tianci.cpp\
       tianci2.cpp

$(TARGET): $(SRCS) # Dependency
	$(cc) $(CFLAGS) $(SRCS) -o $(TARGET) -lmysqlclient

clean:
	rm -f $(TARGET)

