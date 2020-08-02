AUTOMAKE_OPTIONS = foreign subdir-objects
MODNAME=mod_example

mod_LTLIBRARIES = mod_example.la
mod_example_la_SOURCES  = mod_example.c
mod_example_la_CFLAGS   = $(FREESWITCH_CFLAGS)
mod_example_la_LDFLAGS  = -avoid-version -module -no-undefined -shared $(FREESWITCH_LIBS)
