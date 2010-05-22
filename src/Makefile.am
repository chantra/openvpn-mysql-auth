lib_LTLIBRARIES = libopenvpn-auth-mysql.la

auth_mysql_SOURCES = rconf.c  rconf.h \
							utils.c utils.h \
							list.c list.h \
							kvp.c kvp.h \
							debug.c debug.h \
							sqlstuff.c  sqlstuff.h

libopenvpn_auth_mysql_la_SOURCES = auth-mysql.c \
				$(auth_mysql_SOURCES)



