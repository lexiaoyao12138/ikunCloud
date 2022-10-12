SRCS:=$(wildcard *.c)
OBJECTS:=$(patsubst %.c, %.o, $(SRCS))
CC:=gcc
LIBS:= -lpthread -lcjson
SERVER:=server

$(SERVER):$(OBJECTS)
	$(CC) $^ -o $@ $(LIBS) -g
	mv $(OBJECTS) ./temp

%.o:%.c
	$(CC) -c $^ -o $@ $(LIBS) -g 
print:
	echo $(OBJECTS)
clean:
	rm -rf $(OBJECTS) $(SERVER)
