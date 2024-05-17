/* 
    <<< ELABORATION AND SIMULATION CALLBACKS >>>

    Four callback functions are called by the kernel at various stages during elaboration and simulation. They have the following declarations: 

        - virtual void before_end_of_elaboration() ;
        - virtual void end_of_elaboration() ;
        - virtual void start_of_simulation() ;
        - virtual void end_of_simulation() ; 

    The implementation shall define each of these 4 callbacks functions as member functions of the classes sc_module, sc_port, sc_export, 
        and sc_prim_channel, and each of these definitions shall have empty bodies.

*/

/*
    <<< before_end_of_elaboration >>>

    The implementation shall make callbacks to member function before_end_of_elaboration after the construction of the module hierarchy is
        is complete.
    
    Function before_end_of_elaboration may extend the construction of the model hierarchy by instantiating futher modules ( and other objects ) 
        within the module hierarchy.

    THE PURPOSE OF MEMBER FUNCTION before_end_of_elaboration IS TO ALLOW AN APPLICATION TO PERFORM ACTIONS DURING ELABORATION THAT DEPEND ON THE 
        GLOBAL PROPERTIES OF THE MODULE HIERARCHU AND THAT ALSO NEED TO MODIFY THE MODULE HIERARCHY.
 

*/



#include "systemc.h"
using namespace sc_core;

SC_MODULE(STAGE) 
{
  SC_CTOR(STAGE) 
  { // elaboration
    std::cout << sc_time_stamp() << ": Elaboration: constructor" << std::endl;
    SC_THREAD(thread); // initialization + simulation
  }

  ~STAGE()
  { // cleanup
    std::cout << sc_time_stamp() << ": Cleanup: desctructor" << std::endl;
  }

  void thread()
   {
    std::cout << sc_time_stamp() << ": Execution.initialization" << std::endl;
    int i = 0;
    while(true) 
    {
      wait(1, SC_SEC); // advance-time
      std::cout << sc_time_stamp() << ": Execution.simulation" << std::endl; // evaluation
      std::cout << "i = " << i << std::endl ;
      if (++i >= 2)
      {
        std::cout << "i = " << i << std::endl ;
        sc_stop(); // stop simulation after 5 iterations
      }
    }
  }
  void before_end_of_elaboration() {
    std::cout << "before end of elaboration" << std::endl;
  }
  void end_of_elaboration() {
    std::cout << "end of elaboration" << std::endl;
  }
  void start_of_simulation() {
    std::cout << "start of simulation" << std::endl;
  }
  void end_of_simulation() {
    std::cout << "end of simulation" << std::endl;
  }
};

int sc_main(int, char*[]) {
  STAGE stage("stage"); // Elaboration
  sc_start(); // Execution till sc_stop
  return 0; // Cleanup
}