#ifndef RELAY_DEFS
#define RELAY_DEFS

#include "tobilib/h2ep/server.hpp"
#include "tobilib/network/server.hpp"
#include <string>
#include <set>
#include <mutex>
#include <vector>

using namespace tobilib;
using namespace std;
using namespace boost::property_tree;

const string PASSFILE = "passwort.txt";
const vector<int> PINS = {17,22,23,24,25};

namespace control
{
    void tick();
    void start();

    struct ConnectionData
    {
        string id;
        bool viewer = false;
        bool admin = false;
        bool vehicle = false;
        bool captain = false;
    };

    typedef h2ep::WS_Server<ConnectionData> Server;
    typedef Server::Connection* Client;
    typedef vector<Client> Client_list;
    typedef Server::EndpointType::Status EPStatus;

    namespace core
    {
        extern Server server;

        namespace statusinfo
        {
            extern bool send;
            h2ep::Event objectify();
            h2ep::Event objectify_admin();
        }

        void tick();
        void client_tick(Client);
        void command(Client, const h2ep::Event&);
        void send_to_all(const h2ep::Event&);
        void send_to_all(const string&);
        void welcome(Client);
        Client get_client(const string&);
    }

    namespace viewers
    {
        namespace password
        {
            extern string value;
            void set(const string&);
        }

        namespace list
        {
            Client_list get();
            int count();
            void add (Client);
            void remove (Client);
        }

        void login(Client, const h2ep::Event&);
        void logout(Client);
    }

    namespace admins
    {
        void tick();

        namespace list
        {
            Client_list get();
            void add(Client);
            void remove(Client);
        }
        
        void login(Client, const h2ep::Event&);
        void video_pass(Client, const h2ep::Event&);
        void captain_pass(Client, const h2ep::Event&);
        void kick(Client, const h2ep::Event&);
    }

    namespace captain
    {
        namespace password
        {
            extern string value;
            void set(const string&);
        }

        namespace seat
        {
            Client get();
            void set(Client);
            void remove();
        }

        void steer(Client, const h2ep::Event&);
        void login(Client, const h2ep::Event&);
        void logout(Client);
    }

    namespace vehicle
    {
        namespace video
        {
            extern bool enabled;
            void set(bool);
            void sync();
        }

        namespace seat
        {
            Client get();
            void set(Client);
            void remove();
        }
        
        void login(Client, const h2ep::Event&);
        void logout();
        void halt();
    }
}

namespace mutual
{
    extern set<string> viewers;
    extern string vehicle;
    extern recursive_mutex lock;

    typedef lock_guard<recursive_mutex> Guard;

    void add_viewer(const string&);
    void remove_viewer(const string&);
    bool is_viewer(const string&);
    void set_vehicle(const string&);
    bool is_vehicle(const string&);
}

namespace transmit
{
    void tick();
    void start();

    struct ConnectionData
    {
        string id;
        bool viewer = false;
        bool vehicle = false;
    };

    typedef stream::WS_Server<ConnectionData> Server;
    typedef Server::Connection* Client;
    typedef Server::EndpointType::Status EPStatus;

    extern Server server;

    void identify(Client);
    void rank(Client);
    Client get_vehicle();
    void distribute(const string&);

}

#endif