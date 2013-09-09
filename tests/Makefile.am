INCLUDES = -I$(top_srcdir)/src
AM_LDFLAGS = -L$(top_srcdir)/src

bin_PROGRAMS = testplugin

testplugin_SOURCES = testplugin.c

ldadd = $(top_srcdir)/src/.libs/libopenvpn-mysql-auth.a

testplugin_CFLAGS = $(MYSQL_CFLAGS)
testplugin_LDADD = $(ldadd) $(MYSQL_LIBS)
