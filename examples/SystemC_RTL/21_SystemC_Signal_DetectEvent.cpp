#include <systemc>
using namespace sc_core;

SC_MODULE(SIGNAL_EVENT) 
{
    sc_signal<int> s1, s2; // defines two signal channels
    SC_CTOR(SIGNAL_EVENT) 
    {
        SC_THREAD(producer1);
        SC_THREAD(producer2);
        SC_THREAD(consumer); // consumer sensitive to (s1 OR s2)
        sensitive << s1 << s2; // same as: sensitive << s1.default_event() << s2.value_changed_event();
        dont_initialize();
    }

  void producer1()
    {
    int v = 1;
    while (true) {
      s1.write(v++); // write to s1
      std::cout << "s1 = " << s1 << "; " << "s1.read() = " << s1.read() << std::endl;
      wait(1, SC_SEC);
    }
  }
  void producer2() 
  {
    int v = 10;
    while (true) {
      s2 = v++; // write to s2
      std::cout << "s2 = " << s2 << "; " << "s2.read() = " << s2.read() << std::endl;
      wait(2, SC_SEC);
    }
  }
  void consumer() 
  {
    while (true) 
    {
        if ( s1.event() == true && s2.event() == true) // both triggered
        { 
            std::cout << "@ time = " << sc_time_stamp() << ": s1 & s2 triggered" << std::endl; 
        } 
        else if (s1.event() == true) // only s1 triggered
        { 
            std::cout <<  "@ time = " << sc_time_stamp() << ": s1 triggered" << std::endl; 
        }
         else  // only s2 triggered
        {
            std::cout <<  "@ time = " << sc_time_stamp() << ": s2 triggered" << std::endl; 
        }
        wait();
    }
  }
};

int sc_main(int, char*[]) 
{
  SIGNAL_EVENT signal_event("signal_event");
  sc_start(8, SC_SEC);
  return 0;
}

