SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)
DEPS := $(OBJS:.o=.d)

INCLUDES := -I../Share/SHLibApp/ -I../Share/SHRemoteShare/ -I../Share/SHLibCommon/ -I../Share/SHDbMySql/ -I../Share/ShareLib/LinuxPublic/Public/ACE_wrappers/ -I../Share/SHProactor
LIBPATH := -L../../lib/
LIBS := -l SHLibCommon -l SHRemoteShare -l SHProactor 

TARGET := libSHLibApp.so
VERSION:= 
#CFLAG := -g $(INCLUDES) -DHAS_REAL_TIMER -D _LINUX
CFLAG := -o3 -D _LINUX $(INCLUDES) -fPIC
LFLAG := -o3 -shared $(INCLUDES) $(LIBPATH) $(LIBS) -fPIC


CC = g++
LD = g++

all:util

util:$(OBJS)
	$(LD) $(LFLAG) -o $(TARGET) $(OBJS) $(LIBPATH) $(LIBS)
	mv $(TARGET) ../../lib/$(TARGET)$(VERSION)
	@echo LibCommon has benn built successfully!

include $(DEPS)

%.o:%.cpp
	$(CC) $(CFLAG) -c $< -o $@
	
%.d:%.cpp
	@set -e; rm -f $@; \
	$(CC) -MM $(INCLUDES) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
	
clean:
	rm -f *.d
	rm -f *.o
	rm -f $(TARGET)

