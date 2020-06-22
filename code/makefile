CFLAGS	= -g3 -Wall
EXEC	= master
CEXEC   = Worker
QEXEC	= whoServer
JEXEC	= whoClient

OBJS	= master.o functions1.o heap.o rbt.o list.o functions.o
COBJS	= Worker.o functions1.o heap.o rbt.o list.o functions.o
QOBJS	= whoServer.o functions1.o heap.o rbt.o list.o functions.o
JOBJS	= whoClient.o functions1.o heap.o rbt.o list.o functions.o
LDFLAGS = -lpthread

.PHONY : all clean run
all: $(EXEC) $(CEXEC) $(QEXEC) $(JEXEC) 

$(EXEC): $(OBJS) 
$(CEXEC): $(COBJS) 
$(QEXEC): $(QOBJS) $(LDFLAGS)
$(JEXEC): $(JOBJS) $(LDFLAGS)

clean:
	rm -f $(OBJS) $(EXEC) $(COBJS) $(CEXEC) $(QOBJS) $(QEXEC) $(JOBJS) $(JEXEC) log_file*

run: $(EXEC) $(CEXEC)
	./master -w 4 -b 256 -s localhost -p 8000 -i input_dir
	
val: $(EXEC) 
	valgrind -v --leak-check=full --show-leak-kinds=all ./master \
	-w 7 -b 256 -i input_dir

