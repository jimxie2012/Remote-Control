SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)
DEPS := $(OBJS:.o=.d)

INCLUDES :=-I../Share/ShareLib/LinuxPublic/Public/ACE_wrappers/ -I../Share/SHRemoteShare/ -I../Share/SHProactor/ -I../Share/SHLibCommon/ -I./
LIBPATH := -L../../lib/
LIBS := -lSHProactor -lSHLibCommon
TARGET := libSHRemoteShare.so 
CFLAG := -o3 $(INCLUDES) -D _LINUX -fPIC
LFLAG := -o3 $(INCLUDES) $(LIBPATH) $(LIBS) -shared

CC = g++
LD = g++

all:clean SHRemoteShare 

SHRemoteShare:$(OBJS)
	$(LD) $(LFLAG) -o $(TARGET) $(OBJS) $(LIBPATH) $(LIBS)
	rm -f $(OBJS)
	mv ./${TARGET} ../../lib/
	@echo sh remote agent has been built successfully!

include $(DEPS)

%.o:%.cpp
	$(CC) $(CFLAG) -c $< -o $@
	
%.d:%.cpp
	@set -e; rm -f $@; \
	$(CC) -MM $(INCLUDES) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
	
clean:
	rm -f $(OBJS)
	rm -f $(DEPS)
	rm -f $(TARGET)
	rm -f *.d.* *~
