Claire Jung
yjung18
worked alone.

Eventually your report about how you implemented thread synchronization
in the server should go here

The critical section refers to the section of code that shares resource and must be executed without another
thread affecting at a given time. Mutex is a synchronization object that helps accomplish mutual exclusion with semaphore.
Semaphore is a synchronization object that keeps the thread count to limit the number of threads that is accessing
the code at a given time and prevents deadlock.

The code implements thread synchronization in two files, room.cpp and message_queue.cpp.

    In room.cpp, critical sections were added to the add_member() and remove_member() functions to make sure that any
changes made to the set of User pointers in the Room class, which keeps track of all receivers in the room, are atomic.
This was done by using a scoped Guard object, which is a mutex.

    In message_queue.cpp, critical sections were added to the enqueue() and dequeue() functions to make sure that interactions
with the deque of messages, which is used as a queue, are atomic. This was done by using a scoped Guard object and semaphores
in both functions.
    - In enqueue(), the Guard object was used to make the push_back() deque call atomic, and
then sem_post() was called on the semaphore to let the receiver know that there is a message to be sent.
    - In dequeue(), the semaphore was used to check for changes in its value every second, and on success, the message
would be dequeued from the queue. The Guard object was used only in the scope where a dequeue was valid, to prevent
unnecessary locking in the rest of the function.


