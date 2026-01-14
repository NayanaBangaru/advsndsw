set(INCLUDE_DIRECTORIES
${SYSTEM_INCLUDE_DIRECTORIES}
${VMC_INCLUDE_DIRS}
${CMAKE_SOURCE_DIR}/shipdata
${CMAKE_SOURCE_DIR}/genfit/core/include
${CMAKE_SOURCE_DIR}/shipLHC

${CMAKE_SOURCE_DIR}/analysis/cuts
${CMAKE_SOURCE_DIR}/analysis/core
${FMT_INCLUDE_DIR}
)

include_directories( ${INCLUDE_DIRECTORIES} )

set(SRCS
sndNtrackCut.cxx
)

set(LINK_DIRECTORIES
${ROOT_LIBRARY_DIR}
${FAIRROOT_LIBRARY_DIR}
)

link_directories( ${LINK_DIRECTORIES})

Set(HEADERS )
Set(LINKDEF AnalysisCutsLinkDef.h)
Set(LIBRARY_NAME snd_analysis_cuts)
Set(DEPENDENCIES shipLHC veto -lROOTTPython genfit)

GENERATE_LIBRARY()