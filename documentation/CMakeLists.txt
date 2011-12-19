message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})

option(VAST_DOC_HTML "Build html documentation" ON)
option(VAST_DOC_LATEX "Build vast documentation" ON)

#setting version and release info
set( VAST_VERSION "${VAST_VERSION_MAJOR}.${VAST_VERSION_MINOR}" )
set( VAST_RELEASE "${VAST_VERSION_MAJOR}.${VAST_VERSION_MINOR}.${VAST_VERSION_PATCH}" )

#setting the sphinx common options
set( COMMON_OPTIONS -d doctrees -D version=${VAST_VERSION} -D release=${VAST_RELEASE} )

if(VAST_DOC_HTML)
	set(SPHINX_OPTIONS_HTML -b html ${COMMON_OPTIONS} )
	add_custom_target(html ALL COMMAND sphinx-build ${SPHINX_OPTIONS_HTML} "${CMAKE_CURRENT_SOURCE_DIR}/source" html )
endif(VAST_DOC_HTML)

if(VAST_DOC_LATEX)
	set(SPHINX_OPTIONS_LATEX -b latex ${COMMON_OPTIONS} )
	add_custom_target(latex ALL COMMAND sphinx-build ${SPHINX_OPTIONS_LATEX} "${CMAKE_CURRENT_SOURCE_DIR}/source" latex )
endif(VAST_DOC_LATEX)

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/ DESTINATION ${CMAKE_INSTALL_PREFIX}/share/vast/doc/html
          FILES_MATCHING PATTERN "*.html")

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/ DESTINATION ${CMAKE_INSTALL_PREFIX}/share/vast/doc/html
          FILES_MATCHING PATTERN "*.js")

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/ DESTINATION ${CMAKE_INSTALL_PREFIX}/share/vast/doc/html
          FILES_MATCHING PATTERN "*.inv")

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/_images/ DESTINATION ${CMAKE_INSTALL_PREFIX}/share/vast/doc/html/_images
          FILES_MATCHING PATTERN "*.png")

install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html/_static/ DESTINATION ${CMAKE_INSTALL_PREFIX}/share/vast/doc/html/_static
          FILES_MATCHING PATTERN "*")
