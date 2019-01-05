#include "definitions.h"

string control::captain::password::value = "";

void control::captain::password::set(const string& pass)
{
    value = pass;
    core::statusinfo::send=true;
    if (value.size()>0)
    {
        Client cptn = seat::get();
        if (cptn!=NULL)
        {
            h2ep::Event ev ("captainlogout");
            ev.data.put("result","ok");
            ev.data.put("msg","Das Passwort wurde angepasst");
            cptn->endpoint->send(ev);
            seat::remove();
        }
    }
}

control::Client control::captain::seat::get()
{
    for (auto& conn: core::server.clients())
    {
        if (conn->data.captain)
            return conn;
    }
    return NULL;
}

void control::captain::seat::set(Client c)
{
    remove();
    c->data.captain=true;
    core::statusinfo::send=true;
}

void control::captain::seat::remove()
{
    Client cptn = get();
    if (cptn==NULL)
        return;
    cptn->data.captain=false;
    vehicle::halt();
    core::statusinfo::send=true;
}

void control::captain::steer(Client c, const h2ep::Event& ev)
{
    if (c->data.vehicle)
    {
        Client cptn = seat::get();
        if (cptn!=NULL)
            cptn->endpoint->send(ev);
        return;
    }
    if (!c->data.captain)
    {
        h2ep::Event err ("steer");
        err.data.put("result","error");
        err.data.put("msg","Du bist nicht am Steuer");
        c->endpoint->send(err);
        return;
    }
    Client vhcl = vehicle::seat::get();
    if (vhcl==NULL)
    {
        h2ep::Event err ("steer");
        err.data.put("result","error");
        err.data.put("msg","Es ist kein Fahrzeug angeschlossen");
        c->endpoint->send(err);
        return;
    }
    vhcl->endpoint->send(ev);
}

void control::captain::login(Client c, const h2ep::Event& ev)
{
    h2ep::Event res ("captainlogin");
    if (seat::get()!=NULL)
    {
        res.data.put("result","error");
        res.data.put("msg","Es ist bereits jemand am Steuer");
        c->endpoint->send(res);
        return;
    }
    if (password::value.size()>0)
    {
        if (ev.data.get("pass","")!=password::value)
        {
            res.data.put("result","error");
            res.data.put("msg","falsches Passwort");
            c->endpoint->send(res);
            return;
        }
    }
    else if (viewers::password::value.size()>0)
    {
        if (ev.data.get("pass","")!=viewers::password::value)
        {
            res.data.put("result","error");
            res.data.put("msg","falsches Passwort");
            c->endpoint->send(res);
            return;
        }
    }
    seat::set(c);
    res.data.put("result","ok");
    c->endpoint->send(res);
}

void control::captain::logout(Client c)
{
    h2ep::Event res ("captainlogout");
    if (!c->data.captain)
    {
        res.data.put("result","error");
        res.data.put("msg","Du bist nicht am Steuer");
        c->endpoint->send(res);
        return;
    }
    seat::remove();
    res.data.put("result","ok");
    c->endpoint->send(res);
}