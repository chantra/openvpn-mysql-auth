#
# Build the OpenVPN auth-mysql plugin module.
#

INCSQL=`mysql_config --cflags`
LIBSQL=`mysql_config --libs`
INCLUDE=-I../..

CC_FLAGS=-O2 -Wall

openvpn-auth-mysql.so : auth-mysql.o sqlstuff.o rconf.o
	gcc ${LIBSQL} ${CC_FLAGS} -fPIC -shared -Wl,-soname,openvpn-auth-mysql.so -o openvpn-auth-mysql.so auth-mysql.o sqlstuff.o rconf.o -lc -lmysqlclient 

auth-mysql.o : auth-mysql.c sqlstuff.h rconf.h
	gcc ${CC_FLAGS} -fPIC -c ${INCSQL} ${INCLUDE} auth-mysql.c

sqlstuff.o : sqlstuff.c sqlstuff.h
	gcc ${CC_FLAGS} -fPIC -c ${INCSQL} ${INCLUDE} sqlstuff.c

rconf.o : rconf.h rconf.c sqlstuff.h
	gcc ${CC_FLAGS} -fPIC -c ${INCLUDE} rconf.c


clean :
	rm -f *.o *.so
