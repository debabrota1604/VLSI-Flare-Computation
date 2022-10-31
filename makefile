# Makefile to compile the code

#

INCLUDEDIR = include
SRCDIR = src

CC = g++
CCOPTS = -I${INCLUDEDIR} -c -Wno-deprecated

LD = g++
LDOPTS = -lm

ifdef DEBUG
  CCOPTS += -g 
  BINDIR = bin_dbg
  OBJDIR = lnx86_dbg

  ifdef DEBUG_DIJKSTRA
    CCOPTS += -DDEBUG_DIJKSTRA
  endif

  ifdef DEBUG_UPDATE_SINK
    CCOPTS += -DDEBUG_UPDATE_SINK
  endif
else
  CCOPTS += -O2 -DNDEBUG
  BINDIR = bin
  OBJDIR = lnx86
endif

# define the source files
ROUTE_CPPS = GenTrees.cpp \
            Gradient2.cpp \
            ReadData.cpp \
            WriteRoutSol.cpp \
            GlobalResource.cpp  \
            prioQueue.cpp  \
            Routing.cpp \
            Debug.cpp

ifdef NEWCODE
  ROUTE_CPPS += mad_new.cpp

  ifndef USE_NEW_IMAD
    ROUTE_CPPS += IMAD_q.cpp
  else
    CCOPTS += -DUSE_NEW_IMAD
  endif

  ROUTER_TARG = new_router
else
  ROUTE_CPPS +=  IMAD_q.cpp \
                 mad.cpp 

  ROUTER_TARG = router
endif

GEN_ROUTE_CPPS = InputGenlib.cpp \
            InputGri.cpp \
            InputSch.cpp \
            Timing.cpp \
            RankCalc.cpp \
            GenRoute.cpp \
            WriteDat.cpp

ROUTE_OBJS = ${ROUTE_CPPS:%.cpp=${OBJDIR}/%.o}
GEN_ROUTE_OBJS = ${GEN_ROUTE_CPPS:%.cpp=${OBJDIR}/%.o}

# all: reader router timer
all: reader router

router: ${BINDIR}/${ROUTER_TARG}

reader: ${BINDIR}/reader

timer: ${BINDIR}/timer

# small testcase
run_small: ${BINDIR}/${ROUTER_TARG}
	${BINDIR}/${ROUTER_TARG} -i data/desmal.dat

# larger test case
run: ${BINDIR}/${ROUTER_TARG}
	${BINDIR}/${ROUTER_TARG} -i data/des/desm.dat

${BINDIR}/${ROUTER_TARG}: ${BINDIR} ${ROUTE_OBJS}
	${LD} ${LDOPTS} -o $@ ${ROUTE_OBJS}


${BINDIR}/reader: ${BINDIR} ${GEN_ROUTE_OBJS}
	${LD} ${LDOPTS} -o $@ ${GEN_ROUTE_OBJS}

${BINDIR}: 
	if [ ! -d ${BINDIR} ]; then mkdir ${BINDIR} ; fi

${OBJDIR}: 
	if [ ! -d ${OBJDIR} ]; then mkdir ${OBJDIR} ; fi

${OBJDIR}/%.o: ${SRCDIR}/%.cpp ${OBJDIR}
	${CC} ${CCOPTS} -o $@ $<

clean:
	rm -f ${BINDIR}/${ROUTER_TARG} ${ROUTE_OBJS}
