
TEMPLATE = subdirs
SUBDIRS = widgets viewer main view_widgets

#if main must be last:
CONFIG += ordered
main.depends = widgets viewer
view_widgets.depends = widgets viewer


#message($$SOURCES)


