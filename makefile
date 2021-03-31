CC               = gcc
CXXFLAGS        += -ansi -pedantic -W -Wall -Werror -g


SOURCES          = arg_utils.c file_tree.c find.c main.c str_utils.c syscall_utils.c
OBJECTS          = $(patsubst %.c,%.o,$(SOURCES))
TARGET			 = myFind

all:             $(TARGET)
$(TARGET):       $(OBJECTS)
	$(CC) $(OBJECTS) $(CXXFLAGS) -o $(TARGET)

$(OBJECTS):
	$(CC) $(SOURCES) -c 

clear:
	rm $(OBJECTS) $(TARGET)