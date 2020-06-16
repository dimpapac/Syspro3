CFLAGS	= -g3 -Wall
EXEC	= master
CEXEC   = Worker

OBJS	= master.o functions1.o heap.o rbt.o list.o functions.o
COBJS	= Worker.o functions1.o heap.o rbt.o list.o functions.o

.PHONY : all clean run
all: $(EXEC) $(CEXEC) 

$(EXEC): $(OBJS) 
$(CEXEC): $(COBJS) 

clean:
	rm -f $(OBJS) $(EXEC) $(COBJS) $(CEXEC)

run: $(EXEC) $(CEXEC)
	./master -w 4 -b 256 -s localhost -p 2048 -i input_dir
	
val: $(EXEC) 
	valgrind -v --leak-check=full --show-leak-kinds=all ./master \
	-w 4 -b 256 -i input_dir

