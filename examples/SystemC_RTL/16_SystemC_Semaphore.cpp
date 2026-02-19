/*
    -SEMAPHORE-

    For some resources, you may want to model more than one copy or owner. A good example of this would be parking spaces in a
         parking lot. 

    To manage this type of resource, SystemC provides the sc_semaphore class. 

    - the sc_semaphore class inherits from and implements the sc_semaphore_if class.

    When creating a sc_semaphore object, it is necessary to specify how many are available. In a sense, a MUTEX is merely
         a semaphore with a count of one. 
        
    An sc_semaphore access consists of waiting for an available resource and then posting notice when finished with the resource. 

                                    sc_semaphore NAME(COUNT);

                                    NAME.wait();                    // Lock one semaphore

                                                            // Wait until available if in use

                                    int NAME.trywait()              // Non-blocking, return success
                                    int NAME.get_value()            // Returns available semaphores
                                    NAME.post();                    // Free one previously locked semaphore

*/


/*
    A semaphore:
        1. Is a predefined channel intended to model the behavior of a software semaphore used to provide limited concurrent 
            access to a shared resource.
        2. Has an integer value, the semaphore value, which is set to the permitted number of concurrent accesses when the 
             is constructed.

           a) if the initial value is one, the semaphore is equivelent to a mutex.

Member functions:

  1. int wait():

    a) If the semaphore value is greater than 0, wait() shall decrement the semaphore value and return.
    b) If the semaphore value is equal to 0, wait() shall suspend until the semaphore value is incremented (by another process).
    c) Shall unconditionally return the value 0.

  2. int trywait():
    a) If the semaphore value is greater than 0, trywait() shall decrement the semaphore value and shall return the value 0.
    b) If the semaphore value is equal to 0, trywait() shall immediately return the value –1 without modifying the semaphore value.

  3. int post():
    a) shall increment the semaphore value.
    b) shall use immediate notification to signal the act of incrementing the semaphore value to any waiting processes.
    c) shall unconditionally return the value 0.
    
  4. int get_value(): shall return the semaphore value.

*/

#include <systemc>
using namespace sc_core;

SC_MODULE(SEMAPHORE) 
{
  sc_semaphore s; // declares semaphore

  SC_CTOR(SEMAPHORE) : s(2) // init semaphore with 2 resources
  { 
    SC_THREAD(thread_1); // register 3 threads competing for resources
    SC_THREAD(thread_2);
    SC_THREAD(thread_3);
  }
  
  void thread_1() 
  {
    while (true) 
    {
      if (s.trywait() == -1) // try to obtain a resource
      { 
        std::cout<< sc_time_stamp() << ": THREAD 1 : resource not available, value of semaphore is " << s.get_value() << std::endl;
        s.wait(); // if not successful, wait till resource is available
      }

      std::cout<< sc_time_stamp() << ": locked by thread_1, value is " << s.get_value() << std::endl;
      wait(2, SC_SEC); // occupy resource for 1 s
      s.post(); // release resource
      std::cout<< sc_time_stamp() << ": unlocked by thread_1, value is " << s.get_value() << std::endl;
      wait(SC_ZERO_TIME); // give time for the other process to lock
    }
  }

  void thread_2() 
  {
    while (true) 
    {
      if (s.trywait() == -1) // try to obtain a resource
      { 
        std::cout<< sc_time_stamp() << ": THREAD 2 : resource not available, value of semaphore is " << s.get_value() << std::endl;
        s.wait(); // if not successful, wait till resource is available
      }

      std::cout<< sc_time_stamp() << ": locked by thread_2, value is " << s.get_value() << std::endl;
      wait(1, SC_SEC); // occupy resource for 1 s
      s.post(); // release resource
      std::cout<< sc_time_stamp() << ": unlocked by thread_2, value is " << s.get_value() << std::endl;
      wait(SC_ZERO_TIME); // give time for the other process to lock
    }
  }
  void thread_3() 
  {
    while (true) 
    {
      if (s.trywait() == -1) // try to obtain a resource
      { 
        std::cout<< sc_time_stamp() << ": THREAD 3 : resource not available, value of semaphore is " << s.get_value() << std::endl;
        s.wait(); // if not successful, wait till resource is available
      }
      std::cout<< sc_time_stamp() << ": locked by thread_3, value is " << s.get_value() << std::endl;
      wait(1, SC_SEC); // occupy resource for 1 s
      s.post(); // release resource
      std::cout<< sc_time_stamp() << ": unlocked by thread_3, value is " << s.get_value() << std::endl;
      wait(SC_ZERO_TIME); // give time for the other process to lock
    }
  }
};

int sc_main(int, char*[]) 
{
  SEMAPHORE semaphore("semaphore");
  sc_start(20, SC_SEC);
  return 0;
}