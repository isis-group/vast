message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})

option(VAST_DOC_HTML "Build html documentation" ON)
option(VAST_DOC_LATEX "Build vast documentation" ON)

set( VAST_VERSION "${VAST_VERSION_MAJOR}.${VAST_VERSION_MINOR}" )
set( VAST_RELEASE "${VAST_VERSION_MAJOR}.${VAST_VERSION_MINOR}.${VAST_VERSION_PATCH}" )

set( COMMON_OPTIONS -d doctrees -D version=${VAST_VERSION} -D release=${VAST_RELEASE} )

if(VAST_DOC_HTML)
	set(SPHINX_OPTIONS_HTML -b html ${COMMON_OPTIONS} )
	add_custom_target(html ALL COMMAND sphinx-build ${SPHINX_OPTIONS_HTML} "${CMAKE_CURRENT_SOURCE_DIR}/source" html )
endif(VAST_DOC_HTML)

if(VAST_DOC_LATEX)
	set(SPHINX_OPTIONS_LATEX -b latex ${COMMON_OPTIONS} )
	add_custom_target(latex ALL COMMAND sphinx-build ${SPHINX_OPTIONS_LATEX} "${CMAKE_CURRENT_SOURCE_DIR}/source" latex )
endif(VAST_DOC_LATEX)
