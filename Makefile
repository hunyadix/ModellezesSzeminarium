SrcSuf = cc
ObjSuf = o
ExeSuf = 

CXX       = g++
CXXFLAGS  = -Wall
LIBS      =
LD        = g++
LDFLAGS   = -Wall
OutPutOpt = -o # +whitespace

CONSOLECOLORS_S = ../../src/ConsoleColors.$(SrcSuf)
CONSOLECOLORS_O = ./obj/ConsoleColors.$(ObjSuf)
OBJS     += $(CONSOLECOLORS_O)

TESTPHYSICS_S = ./src/testPhysics.$(SrcSuf)
TESTPHYSICS_O = ./obj/testPhysics.$(ObjSuf)
TESTPHYSICS_A = ./bin/startSimulation$(ExeSuf)
OBJS     += $(TESTPHYSICS_O)
PROGRAMS += $(TESTPHYSICS_A)

all: $(PROGRAMS)

$(TESTPHYSICS_A): $(TESTPHYSICS_O)
	@printf "Compiling done, linking...\n"
	$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@
	@echo "Succesful make..."
	@echo "...$@ is ready to use."

$(TESTPHYSICS_O): $(TESTPHYSICS_S)  
	@printf "Compiling test: \"testPhysics\"...\n"
	$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

clean:
	@rm -f $(OBJS) core


distclean: clean
	@rm -f $(PROGRAMS) *Dict.* *.def *.exp \
	*.root *.ps *.so *.lib *.dll *.d *.log .def so_locations
	@rm -rf cxx_repository

.SUFFIXES: .$(SrcSuf)