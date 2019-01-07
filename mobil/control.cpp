#include "definitions.h"
#include "tobilib/stringplus/stringplus.h"
#include <iostream>

GPO LEFT_BACKWARD (17,GPIO_value::LOW);
GPO LEFT_FORWARD (22,GPIO_value::LOW);
GPO RIGHT_BACKWARD (23,GPIO_value::LOW);
GPO RIGHT_FORWARD (24,GPIO_value::LOW);
GPO LED (25,GPIO_value::LOW);

control::Client control::client;
control::Status control::status = control::Status::None;
Timer control::timeout (10);

void control::steer(const h2ep::Event& ev)
{
    if (status == Status::Cancel)
        return;
    h2ep::Event res ("steer");
    try
    {
        for (auto& item: ev.data.get_child("pins"))
        {
            int pin = item.second.get<int>("pin");
            GPIO_value value = static_cast<GPIO_value>(item.second.get<int>("value"));
            if (PINS.count(pin)==0)
            {
                res.data.put("result","error");
                res.data.put("msg",string("ungültiger Pin: ")+to_string(pin));
                client.send(res);
                return;
            }
            PINS.at(pin)->set(value);
        }
    }
    catch (boost::property_tree::ptree_error& err)
    {
        res.data.put("result","error");
        res.data.put("msg","falsch formatierte Anfrage");
        client.send(res);
        return;
    }
    catch (Exception& err)
    {
        res.data.put("result","error");
        cout << err.what() << endl;
        res.data.put("msg","Fehler bei der Steuerung. Siehe Server-Log");
        client.send(res);
    }
    res.data.put("result","ok");
    client.send(res);
}

void control::halt()
{
    for (auto& it: PINS)
    {
        try {
            it.second->set(GPIO_value::LOW);
        } catch (Exception& err) {
            cout << "Fehler beim Anhalten: " << err.what() << endl;
        }
    }
}

void control::read()
{
    if (client.status()==Client::Status::Closed)
    {
        cancel();
        return;
    }
    if (!client.read_available())
        return;
    h2ep::Event ev = client.read();

    if (ev.name == "session")
    {
        mutual::id_set(ev.data.get("id",""));
        login();
    }
    else if (ev.name == "mobil")
    {
        if (ev.data.get("result","")=="ok")
        {
            status=Status::Active;
        }
        else
        {
            cout << "Fehler beim Anmelden als Fahrzeug: " << ev.data.get("msg","-") << endl;
            cancel();
        }
    }
    else if (ev.name == "video")
    {
        mutual::video_active_set(ev.data.get("state",false));
    }
    else if (ev.name == "steer")
    {
        steer(ev);
    }
    else if (ev.name == "noop")
    {
        client.send(h2ep::Event("noop"));
    }
}

void control::connect()
{
    client.connect(HOST,CTRL_PORT);
    status = Status::Connecting;
}

void control::login()
{
    timeout.set();
    try
    {
        StringPlus pass = StringPlus::fromFile(PASSFILE);
        h2ep::Event ev ("mobil");
        ev.data.put("pass",pass.toString());
        client.send(ev);
    }
    catch (Exception& err)
    {
        cout << err.what() << endl;
        cancel();
    }
}

void control::cancel()
{
    client.shutdown();
    halt();
    mutual::id_set("");
    mutual::video_active_set(false);
    status = Status::Cancel;
}

void control::restart()
{
    timeout.set();
    status = Status::Timeout;
}

void control::tick()
{
    client.tick();
    if (client.warnings)
    {
        cout << client.warnings.toString();
        cout.flush();
        client.warnings.clear();
    }

    switch(status)
    {
    case Status::Timeout:
        if (timeout.due())
            connect();
        break;

    case Status::Connecting:
        read();
        break;

    case Status::Active:
        read();
        break;

    case Status::Cancel:
        if (client.status() == Client::Status::Closed)
            restart();
        break;
    }
}