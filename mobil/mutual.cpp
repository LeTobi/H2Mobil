#include "definitions.h"

string mutual::id = "";
bool mutual::video_active = false;
bool mutual::restart = false;
recursive_mutex mutual::lock;

void mutual::id_set(const string& val)
{
    Guard g (lock);
    id = val;
}

string mutual::id_get()
{
    Guard g (lock);
    return id;
}

void mutual::video_active_set(bool val)
{
    Guard g (lock);
    video_active = val;
}

bool mutual::video_active_get()
{
    Guard g (lock);
    return video_active;
}

void mutual::set_restart()
{
    Guard g (lock);
    restart = true;
}

bool mutual::check_restart()
{
    Guard g (lock);
    bool out = restart;
    restart = false;
    return out;
}