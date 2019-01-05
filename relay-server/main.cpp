#include "definitions.h"

int main()
{
    control::start();
    while (true)
    {
        control::tick();
    }
    return 0;
}