CC=gcc
CXX=g++
RM=rm -f
DEBUG = -g
LDFLAGS = -Wall $(DEBUG)
CPPFLAGS = -Wall -c $(DEBUG)
SRCS=main.cpp inputstream.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: medianfilter

medianfilter: $(OBJS)
	$(CXX) $(LDFLAGS) -o medianfilter $(OBJS) 

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

run:
	./medianfilter

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) *~ .depend

include .depend