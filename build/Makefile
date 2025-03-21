CC = g++
# CFLAGS = -Werror -Wall -O2
CFLAGS = -O2
LFLAGS = -lmysqlclient

TARGET = ./main
SRCS = main.cpp guanxin.cpp yixuan.cpp ran.cpp tianci.cpp tianci2.cpp
OBJS = $(SRCS:.cpp=.o)

.PHONY = all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET) $(LFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

