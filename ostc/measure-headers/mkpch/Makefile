
all:
	g++ -Wall -o mkpch main.cpp

#PREFIXES += /root/mesos/3rdparty/boost-1.53.0/boost
PREFIXES += /root/mesos/3rdparty

tests:
	./mkpch input $(PREFIXES)

install:
	cp mkpch /root/bin
