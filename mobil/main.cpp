#include "definitions.h"
#include <iostream>
#include <thread>

void main_video()
{
    while (true)
    {
        video::tick();
    }
}

int main()
{
    thread videothread (main_video);
    control::connect();
    while (true)
    {
        control::tick();
    }
    return 0;
}