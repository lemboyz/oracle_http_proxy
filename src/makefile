CC=g++
FLAG= -g -Wall -Wextra
INC = -I. 

ORACLE_INCLUDES=-I$(ORACLE_HOME)/rdbms/demo \
                -I$(ORACLE_HOME)/rdbms/public \
                -I$(ORACLE_HOME)/plsql/public \
                -I$(ORACLE_HOME)/network/public \
                -I$(ORACLE_HOME)/precomp/public
ORACLE_LIBS=-L$(ORACLE_HOME)/lib -L$(ORACLE_HOME)/rdbms/lib -locci -lclntsh -lpthread -ldl -lm -lrt -lstdc++  

SYS_BITS=$(shell getconf LONG_BIT 2>/dev/null | sed 's/32//') #取得字长位数

SYLIB=-L/usr/lib$(SYS_BITS) -lpthread -liconv -lcurl -llua-5.1 -L/usr/lib64/

ALL: oracle_http_proxy
OBJ=main.o tools.o lua4app.o DBPool.o DBData.o httpparser.o CoutRedirector.o listener.o

.cpp.o:$*.cpp
	$(CC) -c -o $@ $< $(FLAG) $(INC) $(ORACLE_INCLUDES)

oracle_http_proxy: $(OBJ)
	$(CC) $^ -o ../bin/$@ $(INC) $(SYLIB) $(LULIB) $(ORACLE_LIBS)

clean:
	rm -f *.o
