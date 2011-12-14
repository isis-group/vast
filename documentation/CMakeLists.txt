message(STATUS ${CMAKE_CURRENT_SOURCE_DIR})

option(VAST_DOC_HTML "Build html documentation" ON)
option(VAST_DOC_LATEX "Build vast documentation" ON)

if(VAST_DOC_HTML)
	set(SPHINX_OPTIONS_HTML -b html -d doctrees )
	add_custom_target(html COMMAND sphinx-build ${SPHINX_OPTIONS_HTML} "${CMAKE_CURRENT_SOURCE_DIR}/source" html )
endif(VAST_DOC_HTML)

if(VAST_DOC_LATEX)
	set(SPHINX_OPTIONS_LATEX -b latex -d doctrees )
	add_custom_target(latex COMMAND sphinx-build ${SPHINX_OPTIONS_LATEX} "${CMAKE_CURRENT_SOURCE_DIR}/source" latex )
endif(VAST_DOC_LATEX)
