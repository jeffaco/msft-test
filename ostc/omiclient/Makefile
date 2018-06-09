OMI_HOME = /home/jeffcof/dev/work/omi/Unix
OMI_LIBS = /home/jeffcof/dev/work/omi/Unix/output/lib

all : omiclient

omiclient : omiclient.cpp
	g++ -g -o omiclient omiclient.cpp -Wl,-rpath=$(OMI_LIBS) -I$(OMI_HOME)/output/include -I$(OMI_HOME)/common -I$(OMI_HOME) -L$(OMI_LIBS) -lomiclient

clean :
	rm -f omiclient *~
