#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include <qDebug>
#include <QDir>
#include <QString>

#include "ZeroTierSockets.h"

struct Node
{
    Node() : online(false), joinedAtLeastOneNetwork(false), id(0) {}
    bool online;
    bool joinedAtLeastOneNetwork;
    uint64_t id;
    // etc
} myNode;

void myZeroTierEventCallback(void *msgPtr)
{
    struct zts_callback_msg *msg = (struct zts_callback_msg *)msgPtr;

    // Node events
    if (msg->eventCode == ZTS_EVENT_NODE_ONLINE) {
        qDebug("ZTS_EVENT_NODE_ONLINE --- This node's ID is %llx\n", msg->node->address);
        myNode.id = msg->node->address;
        myNode.online = true;
    }
    if (msg->eventCode == ZTS_EVENT_NODE_OFFLINE) {
        qDebug("ZTS_EVENT_NODE_OFFLINE --- Check your physical Internet connection, router, firewall, etc. What ports are you blocking?\n");
        myNode.online = false;
    }
    if (msg->eventCode == ZTS_EVENT_NODE_NORMAL_TERMINATION) {
        qDebug("ZTS_EVENT_NODE_NORMAL_TERMINATION\n");
        myNode.online = false;
    }

    // Virtual network events
    if (msg->eventCode == ZTS_EVENT_NETWORK_NOT_FOUND) {
        qDebug("ZTS_EVENT_NETWORK_NOT_FOUND --- Are you sure %llx is a valid network?\n",
            msg->network->nwid);
    }
    if (msg->eventCode == ZTS_EVENT_NETWORK_REQ_CONFIG) {
        qDebug("ZTS_EVENT_NETWORK_REQ_CONFIG --- Requesting config for network %llx, please wait a few seconds...\n",
            msg->network->nwid);
    }
    if (msg->eventCode == ZTS_EVENT_NETWORK_ACCESS_DENIED) {
        qDebug("ZTS_EVENT_NETWORK_ACCESS_DENIED --- Access to virtual network %llx has been denied. Did you authorize the node yet?\n",
            msg->network->nwid);
    }
    if (msg->eventCode == ZTS_EVENT_NETWORK_READY_IP4) {
        qDebug("ZTS_EVENT_NETWORK_READY_IP4 --- Network config received. IPv4 traffic can now be sent over network %llx\n",
            msg->network->nwid);
        myNode.joinedAtLeastOneNetwork = true;
    }
    if (msg->eventCode == ZTS_EVENT_NETWORK_READY_IP6) {
        qDebug("ZTS_EVENT_NETWORK_READY_IP6 --- Network config received. IPv6 traffic can now be sent over network %llx\n",
            msg->network->nwid);
        myNode.joinedAtLeastOneNetwork = true;
    }
    if (msg->eventCode == ZTS_EVENT_NETWORK_DOWN) {
        qDebug("ZTS_EVENT_NETWORK_DOWN --- %llx\n", msg->network->nwid);
    }

    // Address events
    if (msg->eventCode == ZTS_EVENT_ADDR_ADDED_IP4) {
        char ipstr[ZTS_INET_ADDRSTRLEN];
        struct zts_sockaddr_in *in4 = (struct zts_sockaddr_in*)&(msg->addr->addr);
        zts_inet_ntop(ZTS_AF_INET, &(in4->sin_addr), ipstr, ZTS_INET_ADDRSTRLEN);
        qDebug("ZTS_EVENT_ADDR_NEW_IP4 --- This node's virtual address on network %llx is %s\n",
            msg->addr->nwid, ipstr);
    }
    if (msg->eventCode == ZTS_EVENT_ADDR_ADDED_IP6) {
        char ipstr[ZTS_INET6_ADDRSTRLEN];
        struct zts_sockaddr_in6 *in6 = (struct zts_sockaddr_in6*)&(msg->addr->addr);
        zts_inet_ntop(ZTS_AF_INET6, &(in6->sin6_addr), ipstr, ZTS_INET6_ADDRSTRLEN);
        qDebug("ZTS_EVENT_ADDR_NEW_IP6 --- This node's virtual address on network %llx is %s\n",
            msg->addr->nwid, ipstr);
    }
    if (msg->eventCode == ZTS_EVENT_ADDR_REMOVED_IP4) {
        char ipstr[ZTS_INET_ADDRSTRLEN];
        struct zts_sockaddr_in *in4 = (struct zts_sockaddr_in*)&(msg->addr->addr);
        zts_inet_ntop(ZTS_AF_INET, &(in4->sin_addr), ipstr, ZTS_INET_ADDRSTRLEN);
        qDebug("ZTS_EVENT_ADDR_REMOVED_IP4 --- The virtual address %s for this node on network %llx has been removed.\n",
            ipstr, msg->addr->nwid);
    }
    if (msg->eventCode == ZTS_EVENT_ADDR_REMOVED_IP6) {
        char ipstr[ZTS_INET6_ADDRSTRLEN];
        struct zts_sockaddr_in6 *in6 = (struct zts_sockaddr_in6*)&(msg->addr->addr);
        zts_inet_ntop(ZTS_AF_INET6, &(in6->sin6_addr), ipstr, ZTS_INET6_ADDRSTRLEN);
        qDebug("ZTS_EVENT_ADDR_REMOVED_IP6 --- The virtual address %s for this node on network %llx has been removed.\n",
            ipstr, msg->addr->nwid);
    }

    // Peer events
    if (msg->peer) {
        if (msg->peer->role == ZTS_PEER_ROLE_PLANET) {
            /* Safe to ignore, these are our roots. They orchestrate the P2P connection.
            You might also see other unknown peers, these are our network controllers. */
            return;
        }
        if (msg->eventCode == ZTS_EVENT_PEER_DIRECT) {
            qDebug("ZTS_EVENT_PEER_DIRECT --- A direct path is known for node=%llx\n",
                msg->peer->address);
        }
        if (msg->eventCode == ZTS_EVENT_PEER_RELAY) {
            qDebug("ZTS_EVENT_PEER_RELAY --- No direct path to node=%llx\n", msg->peer->address);
        }
        if (msg->eventCode == ZTS_EVENT_PEER_PATH_DISCOVERED) {
            qDebug("ZTS_EVENT_PEER_PATH_DISCOVERED --- A new direct path was discovered for node=%llx\n",
                msg->peer->address);
        }
        if (msg->eventCode == ZTS_EVENT_PEER_PATH_DEAD) {
            qDebug("ZTS_EVENT_PEER_PATH_DEAD --- A direct path has died for node=%llx\n",
                msg->peer->address);
        }
    }
}


//28.84.116.100

int start_proxy()
{

    int ztServicePort = atoi("9994"); // Port ZT uses to send encrypted UDP packets to peers (try something like 9994)

    int err = ZTS_ERR_OK;
    zts_allow_network_caching(false);
    const char* home_path = (QDir::homePath() + "/oxfold/bigfiletool/myrouter").toStdString().c_str();

    if((err = zts_start(home_path, &myZeroTierEventCallback, ztServicePort)) != ZTS_ERR_OK) {
        qDebug("Unable to start service, error = %d. Exiting.\n", err);
        exit(1);
    }
    qDebug("Waiting for node to come online...\n");
    while (!myNode.online) { zts_delay_ms(50); }
    qDebug("This node's identity is stored in %s\n", home_path);

    uint64_t nwid = 0x8056c2e21c000001;

    if((err = zts_join(nwid)) != ZTS_ERR_OK) {
        qDebug("Unable to join network, error = %d. Exiting.\n", err);
        exit(1);
    }
    qDebug("Joining network %llx\n", nwid);
    while (!myNode.joinedAtLeastOneNetwork) { zts_delay_ms(50); }

    // Idle and just show callback events, stack statistics, etc

    qDebug("Node will now idle...\n");
    while (true) { zts_delay_ms(1000); }

    // Shut down service and stack threads

    zts_stop();
    return 0;
}
