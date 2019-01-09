#include "definitions.h"

set<string> mutual::viewers;
string mutual::vehicle;
recursive_mutex mutual::lock;

void mutual::add_viewer(const string& id)
{
    Guard g (lock);
    viewers.insert(id);
}

void mutual::remove_viewer(const string& id)
{
    Guard g (lock);
    viewers.erase(id);
}

bool mutual::is_viewer(const string& id)
{
    Guard g (lock);
    return viewers.count(id)>0;
}

void mutual::set_vehicle(const string& id)
{
    Guard g (lock);
    vehicle = id;
}

bool mutual::is_vehicle(const string& id)
{
    Guard g (lock);
    if (vehicle.size()==0)
        return false;
    return id==vehicle;
}