file(GLOB REPORTING_SOURCE 
	sca_information.cpp)

add_library(reporting STATIC ${REPORTING_SOURCE})