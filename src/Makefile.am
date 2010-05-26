lib_LTLIBRARIES = libopenvpn-mysql-auth.la

mysql_auth_SOURCES = plugin_conf.c plugin_conf.h \
							utils.c utils.h \
							client_context.c client_context.h \
							list.c list.h \
							kvp.c kvp.h \
							debug.c debug.h \
							am_mysql.c am_mysql.h

libopenvpn_mysql_auth_la_SOURCES = mysql-auth.c \
				$(mysql_auth_SOURCES)



