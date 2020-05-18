SRCS := $(wildcard *.cpp) 
OBJS := $(SRCS:.cpp=.o)
DEPS := $(OBJS:.o=.d)

INCLUDES :=-I../Share/SHProactor -I../Share/SHRemoteShare/ -I../Share/SHLibCommon/ -I../Share/SHLibApp

LIBPATH := -L../../lib/
LIBS := -lSHRemoteShare -lSHLibApp -lSHLibCommon
TARGET := SHAgentService
CFLAG := -o3 $(INCLUDES) -D _LINUX -fPIC
LFLAG := -o3 $(INCLUDES) $(LIBPATH) $(LIBS) -lrt

CC = g++
LD = g++

all:clean SHAgentService

SHAgentService:$(OBJS)
	$(LD) $(LFLAG) -o $(TARGET) $(OBJS) $(LIBPATH) $(LIBS)
	rm -f $(OBJS)
	mv ./${TARGET} ../../Release/
	@echo sh SHAgentService has been built successfully!

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
