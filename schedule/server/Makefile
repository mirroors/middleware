CFLAGES = -I./ -O2
OBJS = common.o SchM_Cfg.o SchM_TskSch.o SchM_MainFunc.o SchM_Queue.o SchM_Timer.o 
TARGET = schedule
RM = rm -rf

$(TARGET):$(OBJS)
	$(CC) -o $@ $^ $(CFLAGES) -lrt -lpthread
$(OBJS):%.o:%.c
	$(CC) -c $< $(CFLAGES) 

clean:
	-$(RM) $(TARGET) $(OBJS)
