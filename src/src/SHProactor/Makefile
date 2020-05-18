SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)
DEPS := $(OBJS:.o=.d)

INCLUDES :=-I../Share/ShareLib/LinuxPublic/ACE_wrappers/ -I./ -I../Share/SHProactor/
LIBPATH := -L../../lib/
LIBS := -lACE
TARGET := libSHProactor.so
CFLAG := -O0 $(INCLUDES) -D _LINUX -fPIC
LFLAG := -O0 $(INCLUDES) $(LIBPATH) $(LIBS) -shared 

CC = g++
LD = g++

all:clean SHProactor

SHProactor:$(OBJS)
	$(LD) $(LFLAG) -o $(TARGET) $(OBJS) $(LIBPATH) $(LIBS)
	rm -f $(OBJS)
	rm -rf ../../lib/$(TARGET)
	mv ./${TARGET} ../../lib/
	@echo SHProactor has been built successfully!

include $(DEPS)

%.o:%.cpp
	$(CC) $(CFLAG) -c $< -o $@
	
%.d:%.cpp
	@set -e; rm -f $@; \
	$(CC) -MM $(INCLUDES) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
	
clean:
	rm -rf ../../lib/$(TARGET)
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -f $(TARGET)
	rm -f *.d.* *~
