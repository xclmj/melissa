/******************************************************************
*                            Melissa                              *
*-----------------------------------------------------------------*
*   COPYRIGHT (C) 2017  by INRIA and EDF. ALL RIGHTS RESERVED.    *
*                                                                 *
* This source is covered by the BSD 3-Clause License.             *
* Refer to the  LICENCE file for further information.             *
*                                                                 *
*-----------------------------------------------------------------*
*  Original Contributors:                                         *
*    Theophile Terraz,                                            *
*    Bruno Raffin,                                                *
*    Alejandro Ribes,                                             *
*    Bertrand Iooss,                                              *
******************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <zmq.h>
#include "melissa_utils.h"

#define PORT_NUMBER 5555

struct cmessage_s
{
    void *context;
    void *message_puller;
    void *message_pusher;
    void *message_resp;
};

typedef struct cmessage_s cmessage_t;

void get_node_name (char *node_name);
void init_context();
void bind_message_rcv(char* port_number);
void bind_message_resp(char* port_number);
void bind_message_snd(char* port_number);
void connect_message_rcv(char* node_name, char* port_number);
void connect_message_snd(char* node_name, char* port_number);
void wait_message(char* msg);
void get_resp_message(char* msg);
void send_message(char* msg);
void send_resp_message(char* msg);
void close_message();
cmessage_t message;


void get_node_name (char *node_name)
{
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;
    char   *addr;
    char ok = 0;

    getifaddrs (&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next)
    {
        if (ifa->ifa_addr->sa_family==AF_INET)
        {
            sa = (struct sockaddr_in *) ifa->ifa_addr;
            addr = inet_ntoa(sa->sin_addr);
            if (strcmp (ifa->ifa_name, "ib0") == 0)
            {
                sprintf(node_name, "%s", addr);
                ok = 1;
                break;
            }
        }
    }
    if (ok == 0)
    {
//#ifdef BUILD_WITH_MPI
//        int resultlen;
//        MPI_Get_processor_name(node_name, &resultlen);
//#else
        gethostname(node_name, MPI_MAX_PROCESSOR_NAME);
//#endif // BUILD_WITH_MPI
    }
}

void init_context ()
{
    message.context = zmq_ctx_new ();
}

void bind_message_rcv(char* port_number)
{
    int rcv_timeout = 1000; // miliseconds
    char name[255];
    message.message_puller = zmq_socket (message.context, ZMQ_PULL);
    zmq_setsockopt (message.message_puller, ZMQ_RCVTIMEO, &rcv_timeout, sizeof(int));
    sprintf (name, "tcp://*:%s", port_number);
    melissa_bind (message.message_puller, name);
}

void connect_message_rcv(char* node_name, char* port_number)
{
    int rcv_timeout = 1000; // miliseconds
    char name[255];
    message.message_puller = zmq_socket (message.context, ZMQ_PULL);
    zmq_setsockopt (message.message_puller, ZMQ_RCVTIMEO, &rcv_timeout, sizeof(int));
    sprintf (name, "tcp://%s:%s", node_name, port_number);
    melissa_connect (message.message_puller, name);
}

void bind_message_snd(char* port_number)
{
    int snd_timeout = 1000; // miliseconds
    char name[255];
    message.message_pusher = zmq_socket (message.context, ZMQ_PUB);
    zmq_setsockopt (message.message_pusher, ZMQ_SNDTIMEO, &snd_timeout, sizeof(int));
    zmq_setsockopt (message.message_pusher, ZMQ_LINGER, &snd_timeout, sizeof(int));
    sprintf (name, "tcp://*:%s", port_number);
    melissa_bind (message.message_pusher, name);
}

void connect_message_snd(char* node_name, char* port_number)
{
    int snd_timeout = 1000; // miliseconds
    char name[255];
    message.message_pusher = zmq_socket (message.context, ZMQ_PUSH);
    zmq_setsockopt (message.message_pusher, ZMQ_SNDTIMEO, &snd_timeout, sizeof(int));
    zmq_setsockopt (message.message_pusher, ZMQ_LINGER, &snd_timeout, sizeof(int));
    sprintf (name, "tcp://%s:%s", node_name, port_number);
    fprintf (stdout, "connecting launcher to %s\n", name);
    melissa_connect (message.message_pusher, name);
}

void bind_message_resp(char* port_number)
{
    int rcv_timeout = 1000; // miliseconds
    char name[255];
    message.message_resp = zmq_socket (message.context, ZMQ_REP);
    zmq_setsockopt (message.message_resp, ZMQ_RCVTIMEO, &rcv_timeout, sizeof(int));
    zmq_setsockopt (message.message_resp, ZMQ_LINGER, &rcv_timeout, sizeof(int));
    sprintf (name, "tcp://*:%s", port_number);
    melissa_bind (message.message_resp, name);
}

void wait_message(char* msg)
{
    char text[256];
    int size = zmq_recv (message.message_puller, text, 255, 0);
    if (size < 1)
    {
        sprintf (msg, "%s", "nothing");
    }
    else
    {
        text[size] = 0;
        sprintf (msg, "%s", text);
    }
}

void get_resp_message(char* msg)
{
    char text[256];
    int size = zmq_recv (message.message_resp, text, 255, 0);
    if (size < 1)
    {
        sprintf (msg, "%s", "nothing");
    }
    else
    {
        text[size] = 0;
        sprintf (msg, "%s", text);
    }
}

void send_message(char* msg)
{
    char text[256];
    sprintf (text, "%s", msg);
    zmq_send (message.message_pusher, text, 255, 0);
}

void send_resp_message(char* msg)
{
    char text[256];
    sprintf (text, "%s", msg);
    zmq_send (message.message_resp, text, 255, 0);
}

void close_message()
{
    zmq_close (message.message_puller);
    zmq_close (message.message_pusher);
    zmq_close (message.message_resp);
    zmq_ctx_term (message.context);
}
