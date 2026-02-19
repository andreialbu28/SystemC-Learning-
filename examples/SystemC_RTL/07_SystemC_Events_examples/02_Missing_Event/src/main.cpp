#include "missing_event.h"

int sc_main(int, char*[]) {
    missing_event miss_event("miss_event");
    sc_start(20, SC_SEC);
    return 0;
}