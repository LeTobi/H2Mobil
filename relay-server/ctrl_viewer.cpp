#include "definitions.h"

string control::viewers::password::value = "";

void control::viewers::password::set(const string& pass)
{
    value = pass;
    core::statusinfo::send=true;
    if (value.size()>0)
    {
        Client_list vwrs = list::get();
        h2ep::Event ev ("videologout");
        ev.data.put("result","ok");
        ev.data.put("msg","Das Passwort wurde angepasst");
        for (auto& v: vwrs)
        {
            if (!v->data.captain)
            {
                list::remove(v);
                v->endpoint->send(ev);
            }
        }
    }
}

vector<control::Client> control::viewers::list::get()
{
    Client_list out;
    for (auto& conn: core::server.clients())
    {
        if (conn->data.viewer)
            out.push_back(conn);
    }
    return out;
}

int control::viewers::list::count()
{
    int count = 0;
    for (auto& conn: core::server.clients())
    {
        if (conn->data.viewer)
            count++;
    }
    return count;
}

void control::viewers::list::add(Client c)
{
    if (count()==0)
        vehicle::video::set(true);
    c->data.viewer=true;
    core::statusinfo::send=true;
    mutual::add_viewer(c->data.id);
}

void control::viewers::list::remove(Client c)
{
    if (!c->data.viewer)
        return;
    c->data.viewer = false;
    core::statusinfo::send=true;
    mutual::remove_viewer(c->data.id);
    if (count()==0)
        vehicle::video::set(false);
}

void control::viewers::login(Client c, const h2ep::Event& ev)
{
    h2ep::Event res ("videologin");
    if (password::value.size()>0)
    {
        string pass = ev.data.get("pass","");
        if (pass=="" || (pass!=password::value && pass!=captain::password::value))
        {
            res.data.put("result","error");
            res.data.put("msg","falsches Passwort");
            c->endpoint->send(res);
            return;
        }
    }
    list::add(c);
    res.data.put("result","ok");
    c->endpoint->send(res);
}

void control::viewers::logout(Client c)
{
    if (!c->data.viewer)
        return;
    h2ep::Event ev ("videologout");
    ev.data.put("result","ok");
    c->endpoint->send(ev);
    list::remove(c);
}