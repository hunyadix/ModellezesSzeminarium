SrcSuf = cc
ObjSuf = o
ExeSuf = 

CXX       = g++ -g
CXXFLAGS  = -Wall -std=c++11
LIBS      = -lGL -lGLU -lglut
LD        = g++
LDFLAGS   = -Wall -Wextra
OutPutOpt = -o # +whitespace

CONSOLECOLORS_S = ../../src/ConsoleColors.$(SrcSuf)
CONSOLECOLORS_O = ./obj/ConsoleColors.$(ObjSuf)
OBJS     += $(CONSOLECOLORS_O)

TESTPHYSICS_S = ./src/testPhysics.$(SrcSuf)
TESTPHYSICS_O = ./obj/testPhysics.$(ObjSuf)
TESTPHYSICS_A = ./bin/test_1$(ExeSuf)
OBJS     += $(TESTPHYSICS_O)
# PROGRAMS += $(TESTPHYSICS_A)

TWOPROTONGRAPHICS_S = ./src/twoProtonGraphics.$(SrcSuf)
TWOPROTONGRAPHICS_O = ./obj/twoProtonGraphics.$(ObjSuf)
TWOPROTONGRAPHICS_A = ./bin/startSimulation$(ExeSuf)
OBJS     += $(TWOPROTONGRAPHICS_O)
PROGRAMS += $(TWOPROTONGRAPHICS_A)

TWOPROTON_S = ./src/twoProton.$(SrcSuf)
TWOPROTON_O = ./obj/twoProton.$(ObjSuf)
TWOPROTON_A = ./bin/startDataCollection$(ExeSuf)
OBJS     += $(TWOPROTON_O)
PROGRAMS += $(TWOPROTON_A)

all: $(PROGRAMS)

$(TESTPHYSICS_A): $(TESTPHYSICS_O)
	@printf "Compiling done, linking...\n"
	$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@
	@echo "Succesful make..."
	@echo "...$@ is ready to use."

$(TWOPROTONGRAPHICS_A): $(TWOPROTONGRAPHICS_O)
	@printf "Compiling done, linking...\n"
	$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@
	@echo "Succesful make..."
	@echo "...$@ is ready to use."

$(TWOPROTON_A): $(TWOPROTON_O)
	@printf "Compiling done, linking...\n"
	$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@
	@echo "Succesful make..."
	@echo "...$@ is ready to use."

$(TESTPHYSICS_O): $(TESTPHYSICS_S)  
	@printf "Compiling test: \"test_1\"...\n"
	$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

$(TWOPROTONGRAPHICS_O): $(TWOPROTONGRAPHICS_S)  
	@printf "Compiling test: \"twoProtonGraphics\"...\n"
	$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

$(TWOPROTON_O): $(TWOPROTON_S)  
	@printf "Compiling test: \"twoProton\"...\n"
	$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

clean:
	@rm -f $(OBJS) core

distclean: clean
	@rm -f $(PROGRAMS) *Dict.* *.def *.exp \
	*.root *.ps *.so *.lib *.dll *.d *.log .def so_locations
	@rm -rf cxx_repository

.SUFFIXES: .$(SrcSuf)