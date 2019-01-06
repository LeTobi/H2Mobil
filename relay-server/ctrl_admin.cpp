#include "definitions.h"
#include "tobilib/stringplus/stringplus.h"
#include <iostream>

control::Client_list control::admins::list::get()
{
    Client_list out;
    for (auto& conn: core::server.clients())
    {
        if (conn->data.admin)
            out.push_back(conn);
    }
    return out;
}

void control::admins::list::add(Client c)
{
    c->data.admin=true;
}

void control::admins::list::remove(Client c)
{
    c->data.admin=false;
}

void control::admins::login(Client c, const h2ep::Event& ev)
{
    h2ep::Event res ("adminlogin");
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
    if (ev.data.get("pass","")!=pass.toString())
    {
        res.data.put("result","error");
        res.data.put("msg","falsches Passwort");
        c->endpoint->send(res);
        return;
    }
    list::add(c);
    res.data.put("result","ok");
    c->endpoint->send(res);
    c->endpoint->send(core::statusinfo::objectify_admin());
}

void control::admins::video_pass(Client c, const h2ep::Event& ev)
{
    h2ep::Event res ("video_pass");
    if (!c->data.admin)
    {
        res.data.put("result","error");
        res.data.put("msg","Du bist kein Admin");
        c->endpoint->send(res);
        return;
    }
    viewers::password::set(ev.data.get("pass",""));
    res.data.put("result","ok");
    c->endpoint->send(res);
}

void control::admins::captain_pass(Client c, const h2ep::Event& ev)
{
    h2ep::Event res ("video_pass");
    if (!c->data.admin)
    {
        res.data.put("result","error");
        res.data.put("msg","Du bist kein Admin");
        c->endpoint->send(res);
        return;
    }
    captain::password::set(ev.data.get("pass",""));
    res.data.put("result","ok");
    c->endpoint->send(res);
}

void control::admins::kick(Client c, const h2ep::Event& ev)
{
    h2ep::Event res ("kick");
    if (!c->data.admin)
    {
        res.data.put("result","error");
        res.data.put("msg","Du bist kein Admin");
        c->endpoint->send(res);
        return;
    }
    Client target = core::get_client(ev.data.get("id","-"));
    if (target==NULL)
    {
        res.data.put("result","error");
        res.data.put("msg","Der Client wurde nicht gefunden");
        c->endpoint->send(res);
        return;
    }
    if (target->data.viewer)
    {
        h2ep::Event info ("videologout");
        info.data.put("result","ok");
        info.data.put("msg","Du wurdest vom Admin als Zuschauer entfernt");
        target->endpoint->send(ev);
        viewers::list::remove(target);
    }
    if (target->data.captain)
    {
        h2ep::Event info ("captainlogout");
        info.data.put("result","ok");
        info.data.put("msg","Du wurdest vom Admin vom Steuer entfernt");
        target->endpoint->send(info);
        captain::seat::remove();
    }
    if (target->data.vehicle)
    {
        vehicle::logout();
    }
    res.data.put("result","ok");
    c->endpoint->send(res);
}