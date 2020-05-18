SRCS := $(wildcard *.cpp)
OBJS := $(SRCS:.cpp=.o)
DEPS := $(OBJS:.o=.d)

INCLUDES := -I../Share/SHLibApp/ -I../Share/SHRemoteShare/ -I../Share/SHLibCommon/ -I../Share/SHDbMySql/ -I../Share/ShareLib/LinuxPublic/Public/ACE_wrappers/ -I../Share/SHProactor
LIBPATH := -L../../lib/
LIBS := -l SHLibCommon -l SHRemoteShare -l SHDbMySql -l SHLibApp -l SHProactor

TARGET := SHCenterCtrl
VERSION:= 
#CFLAG := -g -m32 $(INCLUDES) -DHAS_REAL_TIMER -D _LINUX
CFLAG := -o3 -D _LINUX $(INCLUDES)
LFLAG := -o3 $(INCLUDES) $(LIBPATH) $(LIBS)


CC = g++
LD = g++

all:util

util:$(OBJS)
	$(LD) $(LFLAG) -o $(TARGET) $(OBJS) $(LIBPATH) $(LIBS)
	mv $(TARGET) ../../Release/$(TARGET)$(VERSION)
	@echo SHCenterCtrl has benn built successfully!

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

