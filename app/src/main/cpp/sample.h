#ifndef __sample_h__
#define __sample_h__

#include <bits/basic_string.h>
#include "auto_test_interface.h"

class Sample {
public :
    Sample();
    ~Sample();
    void initServer();

    void initClient();

    void sendClientMsg(std::string string);

    void sendServermsg(std::string string);

private:

    AutoTestInterface* autotest_server;

    AutoTestInterface* autotest_client;

};


#endif// __sample_h__