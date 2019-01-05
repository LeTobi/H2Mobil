#include "definitions.h"
#include <thread>

void video_main()
{
    transmit::start();
    while (true)
    {
        transmit::tick();
    }
}

int main()
{
    thread videothread (video_main);
    control::start();
    while (true)
    {
        control::tick();
    }
    return 0;
}