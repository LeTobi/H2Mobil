#include "definitions.h"
#include "tobilib/stringplus/stringplus.h"
#include <iostream>

bool control::vehicle::video::enabled = false;

void control::vehicle::video::set(bool en)
{
    if (en!=enabled)
    {
        enabled=en;
        sync();
    }
}

void control::vehicle::video::sync()
{
    Client vhcl = seat::get();
    if (vhcl!=NULL)
    {
        h2ep::Event ev ("video");
        ev.data.put("state",enabled);
        vhcl->endpoint->send(ev);
    }
}

control::Client control::vehicle::seat::get()
{
    for (auto& conn: core::server.clients())
    {
        if (conn->data.vehicle)
            return conn;
    }
    return NULL;
}

void control::vehicle::seat::set(Client c)
{
    remove();
    c->data.vehicle=true;
    core::statusinfo::send=true;
    mutual::set_vehicle(c->data.id);
}

void control::vehicle::seat::remove()
{
    Client vhcl = get();
    if (vhcl==NULL)
        return;
    vhcl->data.vehicle=false;
    core::statusinfo::send=true;
    mutual::set_vehicle("");
}

void control::vehicle::login(Client c, const h2ep::Event& ev)
{
    h2ep::Event res ("mobil");
    StringPlus pass;
    try 
    {
        pass = StringPlus::fromFile(PASSFILE);
    }
    catch (exception& err)
    {
        res.data.put("result","error");
        cout << err.what() << endl;
        cout.flush();
        res.data.put("msg","Es wurde kein Serverpasswort definiert");
        c->endpoint->send(res);
        return;
    }
    if (seat::get()!=NULL)
    {
        res.data.put("result","error");
        res.data.put("msg","Ein Fahrzeug ist bereits verbunden");
        c->endpoint->send(res);
        return;
    }
    if (ev.data.get("pass","")!=pass.toString())
    {
        res.data.put("result","error");
        res.data.put("msg","falsches Passwort");
        c->endpoint->send(res);
        return;
    }
    seat::set(c);
    res.data.put("result","ok");
    c->endpoint->send(res);
    video::sync();
}

void control::vehicle::logout()
{
    Client vhcl = seat::get();
    if (vhcl==NULL)
        return;
    h2ep::Event ev ("mobil");
    ev.data.put("result","error");
    ev.data.put("msg","Fahrzeug abgemeldet");
    vhcl->endpoint->send(ev);
    seat::remove();
}

void control::vehicle::halt()
{
    Client vhcl = seat::get();
    if (vhcl == NULL)
        return;
    h2ep::Event ev ("steer");
    for (auto& i: PINS)
    {
        ptree pin;
        pin.put("pin",i);
        pin.put("value",0);
        ev.data.push_back(make_pair("",pin));
    }
    vhcl->endpoint->send(ev);
}