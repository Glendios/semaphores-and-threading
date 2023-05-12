# Target library
lib := libuthread.a

#object list variable, listed in phase order
objs := queue.o
objs += context.o
objs += uthread.o
objs += sem.o
objs += preempt.o

#flags and variables
CC      := gcc
CFLAGS  := -Wall -Wextra -Werror

#Don't print long commands unless specified with "make V=1"
ifneq ($(V),1)
Q=@
endif

all: $(lib)

# Automatic dependency tracking:
deps := $(patsubst %.o,%.d,$(objs))
-include $(deps)

libuthread.a: $(objs)
	@echo "CC	$@"
	$(Q) ar rcs $@ $^

# Generic rule for compiling objects
%.o: %.c
	@echo "CC	$@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "clean"
	$(Q)rm -f $(lib) $(objs) $(deps)