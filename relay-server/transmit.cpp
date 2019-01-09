#include "definitions.h"
#include <iostream>

transmit::Server transmit::server (1601);

void transmit::tick()
{
    server.tick();
    if (server.warnings)
    {
        cout << server.warnings.toString();
        cout.flush();
        server.warnings.clear();
    }
    auto clients = server.clients();
    for (auto& c: clients)
    {
        if (c->endpoint->status()==EPStatus::Closed)
            server.remove(c);
        identify(c);
        rank(c);
    }
    Client vhcl = get_vehicle();
    if (vhcl == NULL)
        return;
    if (vhcl->endpoint->read_size()>0)
    {
        distribute(vhcl->endpoint->read());
        vhcl->endpoint->write("xxx");
    }
}

void transmit::start()
{
    server.start();
}

void transmit::identify(Client c)
{
    if (c->data.id.size()<10)
    {
        if (c->endpoint->read_size()>=10)
        {
            c->data.id = c->endpoint->read().substr(0,10);
        }
    }
    else if (c->data.viewer)
    {
        c->endpoint->read();
    }
}

void transmit::rank(Client c)
{
    c->data.vehicle = mutual::is_vehicle(c->data.id);
    c->data.viewer = mutual::is_viewer(c->data.id);
}

transmit::Client transmit::get_vehicle()
{
    for (auto& c: server.clients())
    {
        if (c->data.vehicle)
            return c;
    }
    return NULL;
}

void transmit::distribute(const string& imgdata)
{
    for (auto& c: server.clients())
    {
        if (c->data.viewer && !c->endpoint->write_busy())
        {
            c->endpoint->write(imgdata);
        }
    }
}