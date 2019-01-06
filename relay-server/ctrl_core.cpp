#include "definitions.h"
#include <iostream>
#include "tobilib/stringplus/stringplus.h"

control::Server control::core::server (1600);
bool control::core::statusinfo::send = false;

void control::tick()
{
    core::tick();
}

void control::start()
{
    core::server.start();
}

h2ep::Event control::core::statusinfo::objectify()
{
    h2ep::Event ev ("status");
    ev.data.put("videocount",viewers::list::count());
    ev.data.put("captain",captain::seat::get()!=NULL);
    ev.data.put("mobile",vehicle::seat::get()!=NULL);
    ev.data.put("video_pass",viewers::password::value.size()>0);
    ev.data.put("captain_pass",captain::password::value.size()>0);
    return ev;
}

h2ep::Event control::core::statusinfo::objectify_admin()
{
    h2ep::Event ev ("status_admin");
    Client vhcl = vehicle::seat::get();
    ev.data.put("mobile.occupied",vhcl!=NULL);
    if (vhcl!=NULL)
    {
        ev.data.put("mobile.ip",vhcl->endpoint->remote_ip().to_string());
        ev.data.put("mobile.id",vhcl->data.id);
    }
    Client cptn = captain::seat::get();
    ev.data.put("captain.occupied",cptn!=NULL);
    if (cptn!=NULL)
    {
        ev.data.put("captain.ip",cptn->endpoint->remote_ip().to_string());
        ev.data.put("captain.id",cptn->data.id);
    }
    ptree viewers_node;
    Client_list vwrs = viewers::list::get();
    for (auto& v: vwrs)
    {
        ptree viewer_node;
        viewer_node.put("ip",v->endpoint->remote_ip().to_string());
        viewer_node.put("id",v->data.id);
        viewers_node.push_back(make_pair("",viewer_node));
    }
    ev.data.add_child("viewers",viewers_node);
    return ev;
}

void control::core::tick()
{
    server.tick();
    if (server.warnings)
    {
        cout << server.warnings.toString();
        cout.flush();
        server.warnings.clear();
    }

    auto list = server.clients();
    for (auto& c: list)
        client_tick(c);

    if (statusinfo::send)
    {
        statusinfo::send = false;
        send_to_all(statusinfo::objectify());
        for (auto& conn: admins::list::get())
        {
            conn->endpoint->send(statusinfo::objectify_admin());
        }
    }
}

void control::core::client_tick(Client c)
{
    if (c->endpoint->status() == EPStatus::Shutdown || c->endpoint->status() == EPStatus::Closed)
    {
        if (c->data.viewer)
            viewers::list::remove(c);
        if (c->data.vehicle)
            vehicle::seat::remove();
        if (c->data.captain)
            captain::seat::remove();
        if (c->data.admin)
            admins::list::remove(c);
    }

    if (c->endpoint->status() == EPStatus::Closed)
        if (!c->data.viewer && !c->data.vehicle && !c->data.captain && !c->data.admin)
            server.remove(c);

    if (c->data.id.size()==0)
        welcome(c);

    if (c->endpoint->status() == EPStatus::Idle)
        c->endpoint->reactivate(h2ep::Event("noop"));

    if (c->endpoint->read_available())
        command(c,c->endpoint->read());
}

void control::core::command(Client c, const h2ep::Event& ev)
{
    if (ev.name=="videologin")
        viewers::login(c,ev);
    else if (ev.name=="videologout")
        viewers::logout(c);
    else if (ev.name=="captainlogin")
        captain::login(c,ev);
    else if (ev.name=="captainlogout")
        captain::logout(c);
    else if (ev.name=="steer")
        captain::steer(c,ev);
    else if (ev.name=="adminlogin")
        admins::login(c,ev);
    else if (ev.name=="video_pass")
        admins::video_pass(c,ev);
    else if (ev.name=="captain_pass")
        admins::captain_pass(c,ev);
    else if (ev.name=="kick")
        admins::kick(c,ev);
    else if (ev.name=="mobil")
        vehicle::login(c,ev);
    else if (ev.name!="noop")
    {
        h2ep::Event res ("info");
        res.data.put("msg","unbekannte anfrage");
        c->endpoint->send(res);
    }
}

void control::core::send_to_all(const h2ep::Event& ev)
{
    for (auto& conn: server.clients())
    {
        if (!conn->data.vehicle)
            conn->endpoint->send(ev);
    }
}

void control::core::send_to_all(const string& txt)
{
    h2ep::Event ev ("info");
    ev.data.put("msg",txt);
    send_to_all(ev);
}

void control::core::welcome(Server::Connection* conn)
{
    conn->data.id = StringPlus::random().toString();
    h2ep::Event ev ("session");
    ev.data.put("id",conn->data.id);
    conn->endpoint->send(ev);
    conn->endpoint->send(statusinfo::objectify());
}

control::Client control::core::get_client(const string& id)
{
    for (auto& c: server.clients())
    {
        if (c->data.id==id)
            return c;
    }
    return NULL;
}