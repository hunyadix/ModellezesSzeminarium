include Makefile.arch

SrcSuf = cc
# ObjSuf = o
# ExeSuf = 

SDL_LIB = -L/usr/local/lib -lSDL2 -Wl,-rpath=/usr/local/lib
SDL_INCLUDE = -I/usr/local/include

# CXX       = g++-5 -g
# CXXFLAGS  = -Wall -std=c++11
CXXFLAGS  += -std=c++1y $(SDL_INCLUDE)
# CXXFLAGS  += -std=c++1y -fdiagnostics-color=always
# LIBS      = -lGL -lGLU -lglut
LIBS      += -lGL -lGLU -lglut -lGLEW $(SDL_LIB)
# LIBS      += -lGL -lGLU -lglut -lGLEW -lavcodec -lswscale -lavutil
# LD        = g++
# LDFLAGS   = -Wall -Wextra
# OutPutOpt = -o # +whitespace

# CONSOLECOLORS_S = ../../src/ConsoleColors.$(SrcSuf)
# CONSOLECOLORS_O = ./obj/ConsoleColors.$(ObjSuf)
# OBJS     += $(CONSOLECOLORS_O)

TESTPHYSICS_S = ./src/testPhysics.$(SrcSuf)
TESTPHYSICS_O = ./obj/testPhysics.$(ObjSuf)
TESTPHYSICS_A = ./bin/test_1$(ExeSuf)
OBJS     += $(TESTPHYSICS_O)
PROGRAMS += $(TESTPHYSICS_A)

TESTSCREENSHOTS_S = ./src/testScreenshots.$(SrcSuf)
TESTSCREENSHOTS_O = ./obj/testScreenshots.$(ObjSuf)
TESTSCREENSHOTS_A = ./bin/test_2$(ExeSuf)
OBJS     += $(TESTSCREENSHOTS_O)
PROGRAMS += $(TESTSCREENSHOTS_A)

TWOPROTONGRAPHICS_S = ./src/twoProtonGraphics.$(SrcSuf)
TWOPROTONGRAPHICS_O = ./obj/twoProtonGraphics.$(ObjSuf)
TWOPROTONGRAPHICS_A = ./bin/startSimulation$(ExeSuf)
OBJS     += $(TWOPROTONGRAPHICS_O)
PROGRAMS += $(TWOPROTONGRAPHICS_A)

TWOPROTON_S = ./src/twoProton.$(SrcSuf)
TWOPROTON_O = ./obj/twoProton.$(ObjSuf)
TWOPROTON_A = ./bin/startTwoProtonDataCollection$(ExeSuf)
OBJS     += $(TWOPROTON_O)
PROGRAMS += $(TWOPROTON_A)

PROTONSINLINE_S = ./src/protonsInLine.$(SrcSuf)
PROTONSINLINE_O = ./obj/protonsInLine.$(ObjSuf)
PROTONSINLINE_A = ./bin/startDataCollection$(ExeSuf)
OBJS     += $(PROTONSINLINE_O)
PROGRAMS += $(PROTONSINLINE_A)

PROTONSINLINEEXPMATRIX_S = ./src/protonsInLineExperimentMatrix.$(SrcSuf)
PROTONSINLINEEXPMATRIX_O = ./obj/protonsInLineExperimentMatrix.$(ObjSuf)
PROTONSINLINEEXPMATRIX_A = ./bin/startProtonsInLineExperimentMatrix$(ExeSuf)
OBJS     += $(PROTONSINLINEEXPMATRIX_O)
PROGRAMS += $(PROTONSINLINEEXPMATRIX_A)

all: $(PROGRAMS)

$(TESTPHYSICS_A): $(TESTPHYSICS_O)
	@printf "Compiling done, linking...\n"
	$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@
	@echo "Succesful make..."
	@echo "...$@ is ready to use."

$(TESTSCREENSHOTS_A): $(TESTSCREENSHOTS_O)
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

$(PROTONSINLINE_A): $(PROTONSINLINE_O)
	@printf "Compiling done, linking...\n"
	$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@
	@echo "Succesful make..."
	@echo "...$@ is ready to use."

$(PROTONSINLINEEXPMATRIX_A): $(PROTONSINLINEEXPMATRIX_O)
	@printf "Compiling done, linking...\n"
	$(LD) $(LDFLAGS) $^ $(LIBS) $(OutPutOpt)$@
	@echo "Succesful make..."
	@echo "...$@ is ready to use."


# Objs

$(TESTPHYSICS_O): $(TESTPHYSICS_S)  
	@printf "Compiling test: \"test_1\"...\n"
	$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

$(TESTSCREENSHOTS_O): $(TESTSCREENSHOTS_S)  
	@printf "Compiling test: \"test_2\"...\n"
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

$(PROTONSINLINE_O): $(PROTONSINLINE_S)  
	@printf "Compiling test: \"protonsInLine\"...\n"
	$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

$(PROTONSINLINEEXPMATRIX_O): $(PROTONSINLINEEXPMATRIX_S)  
	@printf "Compiling test: \"protonsInLineExpMatrix\"...\n"
	$(CXX) $(CXXFLAGS) $(LIBS) -c $< $(OutPutOpt)$@
	@printf "Done.\n"

clean:
	@rm -f $(OBJS) core

distclean: clean
	@rm -f $(PROGRAMS) *Dict.* *.def *.exp \
	*.root *.ps *.so *.lib *.dll *.d *.log .def so_locations
	@rm -rf cxx_repository

.SUFFIXES: .$(SrcSuf)