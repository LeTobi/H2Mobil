#include "definitions.h"
#include "tobilib/encoding/all.h"

video::Status video::status = video::Status::Closed;
Capture video::camera;
stream::WS_Client video::client;

void video::tick()
{
    switch (status)
    {
    case Status::Closed:
        if (mutual::id_get().size()>0)
            status = Status::Connecting;
        mutual::check_restart();
        break;

    case Status::Connecting:
        if (client.status() == Client::Status::Active)
        {
            client.write_setmode(WS_Endpoint::WriteMode::Binary);
            client.write(mutual::id_get());
            status = Status::Enabled;
        }
        break;

    case Status::Enabled:
        if (!mutual::video_active_get())
            status = Status::Disabled;
        if (mutual::check_restart())
            status = Status::Cancel;
        break;

    case Status::Disabled:
        if (mutual::video_active_get())
            status = Status::Enabled;
        if (mutual::check_restart())
            status = Status::Cancel;
        break;

    case Status::Cancel:
        if (client.status()==Client::Status::Closed && camera.status()==Capture::State::closed)
            status = Status::Closed;
        mutual::check_restart();
        break;
    }
    video_tick();
    client_tick();
}

void video::video_tick()
{
    switch (camera.status())
    {
    case Capture::State::closed:
        if (status == Status::Enabled || status==Status::Disabled)
            camera.open(CAMERA);
        break;

    case Capture::State::open:
        if (status == Status::Cancel)
            camera.close();
        if (status == Status::Enabled)
            camera.streamon();
        break;

    case Capture::State::streaming:
        if (status == Status::Cancel)
            camera.close();
        else if (status == Status::Disabled)
            camera.streamoff();
        else
            snapshot();
        break;
    }
}

void video::client_tick()
{
    client.tick();
    client.read();
    switch (client.status())
    {
    case Client::Status::Closed:
        if (status == Status::Connecting)
            client.connect(HOST,VIDEO_PORT);
        else if (status == Status::Enabled || status==Status::Disabled)
            status = Status::Cancel;
        break;
    
    }
}

void video::snapshot()
{
    if (client.status()!=Client::Status::Active && client.status()!=Client::Status::Idle)
        return;
    if (client.write_busy())
        return;
    try
    {
        camera.read();
        client.write(camera.data().toString());
    }
    catch (Exception& err)
    {
        cout << err.what() << endl;
    }
}
