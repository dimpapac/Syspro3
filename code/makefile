CFLAGS	= -g3 -Wall
EXEC	= diseaseAggregator
CEXEC   = Worker

OBJS	= diseaseAggregator.o functions1.o heap.o rbt.o list.o functions.o
COBJS	= Worker.o functions1.o heap.o rbt.o list.o functions.o

.PHONY : all clean run
all: $(EXEC) $(CEXEC) 

$(EXEC): $(OBJS) 
$(CEXEC): $(COBJS) 

clean:
	rm -f $(OBJS) $(EXEC) $(COBJS) $(CEXEC)

run: $(EXEC) $(CEXEC)
	./diseaseAggregator -w 3 -b 256 -i input_dir
	
val: $(EXEC) 
	valgrind -v --leak-check=full --show-leak-kinds=all ./diseaseAggregator \
	-w 4 -b 256 -i input_dir

