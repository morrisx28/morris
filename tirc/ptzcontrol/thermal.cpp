
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "inc/ThermalDemo.h"

using namespace std;


int main() {

    ThermalDemo *demo = new ThermalDemo();

    demo->open();
    demo->runLoop();
    demo->close();
    exit(0);
}
