default: AUTOMAKEFILE_DEFAULT

-include AutoMakefile

LFLAGS+= -lbfd
CFLAGS+= -Wall -Werror -ggdb3 -O0
TARGETS=test

clean: AUTOMAKEFILE_CLEAN

AutoMakefile: *.c *.h Makefile
	./gen_makefile.pl --makefile_name=Makefile -f AutoMakefile $(TARGETS)

