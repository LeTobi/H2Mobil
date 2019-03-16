#include "tobilib/h2ep/client.h"
#include "tobilib/network/client.h"
#include "tobilib/general/timer.hpp"
#include "tobilib/general/gpio.h"
#include "tobilib/webcap/capture.h"
#include <mutex>
#include <map>

using namespace tobilib;
using namespace std;

const string HOST = "wetterfrosch.internet-box.ch";
const int CTRL_PORT = 1600;
const int VIDEO_PORT = 1601;
const string PASSFILE = "passwort.txt";
const string CAMERA = "/dev/video0";

extern GPO LEFT_BACKWARD;
extern GPO LEFT_FORWARD;
extern GPO RIGHT_BACKWARD;
extern GPO RIGHT_FORWARD;
extern GPO LED;

const map<int,GPO*> PINS = {
    make_pair(17,&LEFT_BACKWARD),
    make_pair(22,&LEFT_FORWARD),
    make_pair(23,&RIGHT_BACKWARD),
    make_pair(24,&RIGHT_FORWARD),
    make_pair(25,&LED)
};

namespace control
{
    typedef h2ep::WS_Client Client;
    enum class Status {Timeout, Connecting, Active, Cancel, None};

    extern Client client;
    extern Status status;
    extern Timer timeout;
    extern Timer safety;
    
    void steer(const h2ep::Event& ev);
    void halt();
    void read();
    void connect();
    void login();
    void cancel();
    void restart();
    void tick();
}

namespace mutual
{
    extern string id;
    extern bool video_active;
    extern bool restart;
    extern recursive_mutex lock;

    typedef lock_guard<recursive_mutex> Guard;

    void id_set(const string&);
    string id_get();
    void video_active_set(bool);
    bool video_active_get();
    void set_restart();
    bool check_restart();
}

namespace video
{
    typedef stream::WS_Client Client;
    enum class Status {Closed,Connecting,Enabled,Disabled,Cancel};

    extern Status status;
    extern Capture camera;
    extern jpeg_encoder encoder;
    extern Client client;

    void tick();
    void video_tick();
    void client_tick();
    void snapshot();
}