#=====================================================================
#
# Makefile                                              Date: 2022/10/29
#
# Author : Mustafa abdelhamid
#=====================================================================
SRC=issue_stage.c GPRs.c scoreboard.c dispatch.c assign_structs.c ibuffer.c common.c 

all: issue_test

#raiv_core: raiv_core.o

issue_test: Isuue_test.c $(SRC)
	gcc  $^  -o $@ -I $(PWD) 

clean:
	rm -rf issue_test
