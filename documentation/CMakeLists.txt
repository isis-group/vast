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



file (GLOB VAST_DOC_FILES_HTML  ${CMAKE_CURRENT_BINARY_DIR}/html/*.html ${CMAKE_CURRENT_BINARY_DIR}/html/*.js ${CMAKE_CURRENT_BINARY_DIR}/html/*.inv  )
file (GLOB VAST_DOC_STATIC_FILES ${CMAKE_CURRENT_BINARY_DIR}/html/_static/* )

install(FILES ${VAST_DOC_FILES_HTML} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/vast/doc/html )
install(FILES ${VAST_DOC_STATIC_FILES} DESTINATION ${CMAKE_INSTALL_PREFIX}/share/vast/doc/html/_static )
