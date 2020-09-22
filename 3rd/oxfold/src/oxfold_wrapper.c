#include "../../../oxfold/include/oxfold_wrapper.h"
#include "../../../oxfold/include/ZeroTierSockets.h"

bool isServer = false;

struct Node
{
    bool online;
    bool joinedAtLeastOneNetwork;
    bool connection_ready;
    uint64_t id;
    // etc
    uint64_t nwid;
    char ipV4[50];
    char ipV6[100];
} myNode;

char* add_char_array(char* str1, char*str2)
{
    char * str3 = (char *) malloc(1 + strlen(str1)+ strlen(str2) );
    strcpy(str3, str1);
    strcat(str3, str2);
    return str3;
}

char* getHomePath()
{
#if defined(_WIN32)
    char *home_drive;
    char *home_path;
    int ret;
    home_drive = getenv("HOMEDRIVE");
    home_path  = getenv("HOMEPATH");
    char *home = add_char_array(home_drive, home_path);
    return add_char_array(home, "\\oxfold\\bigfiletool\\myrouter");
#else
    char * value;
    value = getenv ("HOME");
    return add_char_array(value, "/oxfold/bigfiletool/myrouter");
#endif
}

void assign_ipv4(char* ipv4){
    char *home, *ipfile;
    memset(myNode.ipV4, 0x00, 50);
    strncpy(myNode.ipV4, ipv4, strlen(ipv4));
    if (isServer) { // save IP to file
        home = getHomePath();
#if defined(_WIN32)
        ipfile = add_char_array(home, "\\webserver.txt");
#else
        ipfile = add_char_array(home, "/webserver.txt");
#endif
        FILE *f = fopen(ipfile, "w");
        fwrite(ipv4, 1, strlen(ipv4), f);
        fclose(f);
    }
}

void assign_ipv6(char* ipv6){
    memset(myNode.ipV6, 0x00, 100);
    strncpy(myNode.ipV6, ipv6, strlen(ipv6));
}

void myZeroTierEventCallback(void *msgPtr)
{
    struct zts_callback_msg *msg = (struct zts_callback_msg *)msgPtr;

    // Node events
    if (msg->eventCode == ZTS_EVENT_NODE_ONLINE) {
        printf("ZTS_EVENT_NODE_ONLINE --- This node's ID is %llx\n", msg->node->address);
        myNode.id = msg->node->address;
        myNode.online = true;
    }
    if (msg->eventCode == ZTS_EVENT_NODE_OFFLINE) {
        printf("ZTS_EVENT_NODE_OFFLINE --- Check your physical Internet connection, router, firewall, etc. What ports are you blocking?\n");
        myNode.online = false;
    }
    if (msg->eventCode == ZTS_EVENT_NODE_NORMAL_TERMINATION) {
        printf("ZTS_EVENT_NODE_NORMAL_TERMINATION\n");
        myNode.online = false;
    }

    // Virtual network events
    if (msg->eventCode == ZTS_EVENT_NETWORK_NOT_FOUND) {
        printf("ZTS_EVENT_NETWORK_NOT_FOUND --- Are you sure %llx is a valid network?\n",
            msg->network->nwid);
    }
    if (msg->eventCode == ZTS_EVENT_NETWORK_REQ_CONFIG) {
        printf("ZTS_EVENT_NETWORK_REQ_CONFIG --- Requesting config for network %llx, please wait a few seconds...\n",
            msg->network->nwid);
    }
    if (msg->eventCode == ZTS_EVENT_NETWORK_ACCESS_DENIED) {
        printf("ZTS_EVENT_NETWORK_ACCESS_DENIED --- Access to virtual network %llx has been denied. Did you authorize the node yet?\n",
            msg->network->nwid);
    }
    if (msg->eventCode == ZTS_EVENT_NETWORK_READY_IP4) {
        printf("ZTS_EVENT_NETWORK_READY_IP4 --- Network config received. IPv4 traffic can now be sent over network %llx\n",
            msg->network->nwid);
        myNode.joinedAtLeastOneNetwork = true;
        myNode.nwid = msg->network->nwid;
    }
    if (msg->eventCode == ZTS_EVENT_NETWORK_READY_IP6) {
        printf("ZTS_EVENT_NETWORK_READY_IP6 --- Network config received. IPv6 traffic can now be sent over network %llx\n",
            msg->network->nwid);
        myNode.joinedAtLeastOneNetwork = true;
        myNode.nwid = msg->network->nwid;
    }
    if (msg->eventCode == ZTS_EVENT_NETWORK_DOWN) {
        printf("ZTS_EVENT_NETWORK_DOWN --- %llx\n", msg->network->nwid);
    }

    // Address events
    if (msg->eventCode == ZTS_EVENT_ADDR_ADDED_IP4) {
        char ipstr[ZTS_INET_ADDRSTRLEN];
        struct zts_sockaddr_in *in4 = (struct zts_sockaddr_in*)&(msg->addr->addr);
        zts_inet_ntop(ZTS_AF_INET, &(in4->sin_addr), ipstr, ZTS_INET_ADDRSTRLEN);
        printf("ZTS_EVENT_ADDR_NEW_IP4 --- This node's virtual address on network %llx is %s\n",
            msg->addr->nwid, ipstr);
        assign_ipv4(ipstr);
    }
    if (msg->eventCode == ZTS_EVENT_ADDR_ADDED_IP6) {
        char ipstr[ZTS_INET6_ADDRSTRLEN];
        struct zts_sockaddr_in6 *in6 = (struct zts_sockaddr_in6*)&(msg->addr->addr);
        zts_inet_ntop(ZTS_AF_INET6, &(in6->sin6_addr), ipstr, ZTS_INET6_ADDRSTRLEN);
        printf("ZTS_EVENT_ADDR_NEW_IP6 --- This node's virtual address on network %llx is %s\n",
            msg->addr->nwid, ipstr);
        assign_ipv6(ipstr);
    }
    if (msg->eventCode == ZTS_EVENT_ADDR_REMOVED_IP4) {
        char ipstr[ZTS_INET_ADDRSTRLEN];
        struct zts_sockaddr_in *in4 = (struct zts_sockaddr_in*)&(msg->addr->addr);
        zts_inet_ntop(ZTS_AF_INET, &(in4->sin_addr), ipstr, ZTS_INET_ADDRSTRLEN);
        printf("ZTS_EVENT_ADDR_REMOVED_IP4 --- The virtual address %s for this node on network %llx has been removed.\n",
            ipstr, msg->addr->nwid);
        assign_ipv4("");
    }
    if (msg->eventCode == ZTS_EVENT_ADDR_REMOVED_IP6) {
        char ipstr[ZTS_INET6_ADDRSTRLEN];
        struct zts_sockaddr_in6 *in6 = (struct zts_sockaddr_in6*)&(msg->addr->addr);
        zts_inet_ntop(ZTS_AF_INET6, &(in6->sin6_addr), ipstr, ZTS_INET6_ADDRSTRLEN);
        printf("ZTS_EVENT_ADDR_REMOVED_IP6 --- The virtual address %s for this node on network %llx has been removed.\n",
            ipstr, msg->addr->nwid);
        assign_ipv6("");
    }

    // Peer events
    if (msg->peer) {
        if (msg->peer->role == ZTS_PEER_ROLE_PLANET) {
            /* Safe to ignore, these are our roots. They orchestrate the P2P connection.
            You might also see other unknown peers, these are our network controllers. */
            return;
        }
        if (msg->eventCode == ZTS_EVENT_PEER_DIRECT) {
            printf("ZTS_EVENT_PEER_DIRECT --- A direct path is known for node=%llx\n", msg->peer->address);
            if (msg->peer->address == myNode.id) {
                myNode.connection_ready = true;
                printf("myNoe is ready for communication.\n");
            }
        }
        if (msg->eventCode == ZTS_EVENT_PEER_RELAY) {
            printf("ZTS_EVENT_PEER_RELAY --- No direct path to node=%llx\n", msg->peer->address);
        }
        if (msg->eventCode == ZTS_EVENT_PEER_PATH_DISCOVERED) {
            printf("ZTS_EVENT_PEER_PATH_DISCOVERED --- A new direct path was discovered for node=%llx\n", msg->peer->address);
        }
        if (msg->eventCode == ZTS_EVENT_PEER_PATH_DEAD) {
            printf("ZTS_EVENT_PEER_PATH_DEAD --- A direct path has died for node=%llx\n", msg->peer->address);
        }
    }
}

int start_oxfold(char* home_path)
{
    memset(&myNode, 0, sizeof(myNode));

    int ztServicePort = atoi("9994"); // Port ZT uses to send encrypted UDP packets to peers (try something like 9994)

    int err = ZTS_ERR_OK;
    zts_allow_network_caching(false);
    char* home;

    if (home_path == NULL) {  // run for server
        home = getHomePath();
        isServer = true;
    } else {                  // run for client
        home = home_path;
        isServer = false;
    }

    if((err = zts_start(home, &myZeroTierEventCallback, ztServicePort)) != ZTS_ERR_OK) {
        printf("Unable to start service, error = %d. Exiting.\n", err);
        exit(1);
    }
    printf("Waiting for node to come online...\n");
    while (!myNode.online) { zts_delay_ms(50); }
    printf("This node's identity is stored in %s\n", home);

    uint64_t nwid = OXFOLD_NWID; //0x8056c2e21c000001;

    if((err = zts_join(nwid)) != ZTS_ERR_OK) {
        printf("Unable to join network, error = %d. Exiting.\n", err);
        exit(1);
    }
    printf("Joining network %llx\n", nwid);
    while (!myNode.joinedAtLeastOneNetwork) { zts_delay_ms(50); }

    // Shut down service and stack threads
    return 0;
}

int stop_oxfold()
{
    zts_stop();
    zts_free();
    return 0;
}

char* getHostIPV4()
{
    return "0.0.0.0";
}

char* getNodeIPV4() {
    return myNode.ipV4;
}

char* getNodeIPV6() {
    return myNode.ipV6;
}

uint64_t getNodeNWID()
{
    return myNode.nwid;
}

bool is_connect_ready()
{
    return myNode.connection_ready;
}
