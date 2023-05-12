# Project 2

## Phase 1: queue API

We first started by testing if the library could be imported properly by making
a test function and then using it by including the library from a file in the
apps folder, which all worked as expected. Then as we began implementing a
queue, we decided that a doubly linked list would be the most appropriate
structure to use as it provided us with the O(1) runtime needed for the methods.
A singly linked list for example would not be O(1) runtime as it would need to
traverse the entire list to reach the back, ending up as O(n). We used typedef
structs in order to avoid redundant struct calls throughout the function and to
increase readability, as many lines would refer to the structs. Our queue tester
also runs many tests on the queue structure, testing every function and checking
the outputs to make sure that it is robust for the future steps that make
extensive use of the queue.

## Phase 2: uthread API

The uthread API went through different iterations before we decided on the
current design. At first we tried making five states of
ready/running/blocked/waiting/zombie, but quickly realized that blocked and
waiting were the same thing for the purposes of our methods, and that a zombie
state would not be necessary as we do not need to store or use the exit codes of
terminated threads. We created exit commands in the exit method to make sure the
threads would be terminated by the system, and the rest was all done by putting
threads into the ready queue and then running them with given args. We also made
a second queue called the waiting queue. We utilized having two queues by
sending all the blocked threads into the waiting queue, which we thought would
be an important distinction to make in order to deal with corner cases for the
future phases. We utilized the provided testers to test uthread, as they all
heavily utilized the uthread API in order to function.

## Phase 3: semaphore API

For the semaphores, we defined the semaphore with variables that would keep
track of the available resources and blocked threads. We also made sure to check
for invalid cases to get the proper return from the methods if any errors
occured. The implementation was a semaphore much like the ones in class,
incrementing/decrementing the count, combined with queueing/dequeueing threads
so that it would handle the threads and not just be a counter. We did run into
an issue where sem_destroy() would destroy the semaphore too early, which we
fixed by stopping it from destroying if any threads were still unblocked and
ready to process. As for the corner case and starvation issues, we determined
that our queue-based design would tackle it by putting the blocked threads into
a queue and running them in queued order, instead of not using a queue and
leaving it up to the system to arbitrarily pick what thread would be resumed
when a new resource became available. We used the provided sem tests in order to
test our semaphores, and each time we would run all four of them to be sure that
our implementation was stable.

## Phase 4: preemption

Preemption was quite interesting as it ended up being a very complex task due to
going back to previous phases in order to apply the preemption code. For
designing our preemtion, we made sets of signals that would either be blocked or
unblocked depending on whether disable or enable was called. When enabled they
would use the interval timer set to 10000 microseconds (or 100 ticks a second)
and yield threads in accordance to the timer. We also had tester code inside the
signal_handler() in order to make sure the intervals were properly 10000
microseconds. When we tested with low intervals the numbers were not as
consistent, but when testing with higher numbers such as 100000 microseconds,
the results were very consistent so we knew our design was properly counting the
time with low amount of error. We would then reset the timer when calling stop.
We then applied the preemption to our previous phases, making sure that
preemption would be disabled for tasks like creating new threads, and running
logic or otherwise interacting with shared variables.