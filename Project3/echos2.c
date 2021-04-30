#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <signal.h>
#include <pthread.h>
#include "strbuf.h"
#include "linkedlist.h"

#define BACKLOG 5

int running = 1;

// the argument we will pass to the connection-handler threads
struct connection
{
    struct sockaddr_storage addr;
    socklen_t addr_len;
    int fd;
};

node_t *key_list;
int server(char *port);
void *echo(void *arg);

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage: %s [port]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    (void)server(argv[1]);
    return EXIT_SUCCESS;
}

void handler(int signal)
{
    running = 0;
}

int server(char *port)
{
    struct addrinfo hint, *info_list, *info;
    struct connection *con;
    int error, sfd;
    pthread_t tid;

    // initialize hints
    memset(&hint, 0, sizeof(struct addrinfo));
    hint.ai_family = AF_UNSPEC;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_flags = AI_PASSIVE;
    // setting AI_PASSIVE means that we want to create a listening socket

    // get socket and address info for listening port
    // - for a listening socket, give NULL as the host name (because the socket is on
    //   the local host)
    error = getaddrinfo(NULL, port, &hint, &info_list);
    if (error != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
        return -1;
    }

    // attempt to create socket
    for (info = info_list; info != NULL; info = info->ai_next)
    {
        sfd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

        // if we couldn't create the socket, try the next method
        if (sfd == -1)
        {
            continue;
        }

        // if we were able to create the socket, try to set it up for
        // incoming connections;
        //
        // note that this requires two steps:
        // - bind associates the socket with the specified port on the local host
        // - listen sets up a queue for incoming connections and allows us to use accept
        if ((bind(sfd, info->ai_addr, info->ai_addrlen) == 0) &&
            (listen(sfd, BACKLOG) == 0))
        {
            break;
        }

        // unable to set it up, so try the next method
        close(sfd);
    }

    if (info == NULL)
    {
        // we reached the end of result without successfuly binding a socket
        fprintf(stderr, "Could not bind\n");
        return -1;
    }

    freeaddrinfo(info_list);

    struct sigaction act;
    act.sa_handler = handler;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGINT, &act, NULL);

    sigset_t mask;

    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);

    // at this point sfd is bound and listening
    printf("Waiting for connection\n");
    while (running)
    {
        // create argument struct for child thread
        con = malloc(sizeof(struct connection));
        con->addr_len = sizeof(struct sockaddr_storage);
        // addr_len is a read/write parameter to accept
        // we set the initial value, saying how much space is available
        // after the call to accept, this field will contain the actual address length

        // wait for an incoming connection
        con->fd = accept(sfd, (struct sockaddr *)&con->addr, &con->addr_len);
        // we provide
        // sfd - the listening socket
        // &con->addr - a location to write the address of the remote host
        // &con->addr_len - a location to write the length of the address
        //
        // accept will block until a remote host tries to connect
        // it returns a new socket that can be used to communicate with the remote
        // host, and writes the address of the remote hist into the provided location

        // if we got back -1, it means something went wrong
        if (con->fd == -1)
        {
            perror("accept");
            continue;
        }

        // temporarily block SIGINT (child will inherit mask)
        error = pthread_sigmask(SIG_BLOCK, &mask, NULL);
        if (error != 0)
        {
            fprintf(stderr, "sigmask: %s\n", strerror(error));
            abort();
        }

        // spin off a worker thread to handle the remote connection
        error = pthread_create(&tid, NULL, echo, con);

        // if we couldn't spin off the thread, clean up and wait for another connection
        if (error != 0)
        {
            fprintf(stderr, "Unable to create thread: %d\n", error);
            close(con->fd);
            free(con);
            continue;
        }

        // otherwise, detach the thread and wait for the next connection request
        pthread_detach(tid);

        // unblock SIGINT
        error = pthread_sigmask(SIG_UNBLOCK, &mask, NULL);
        if (error != 0)
        {
            fprintf(stderr, "sigmask: %s\n", strerror(error));
            abort();
        }
    }

    puts("No longer listening.");
    pthread_detach(pthread_self());
    pthread_exit(NULL);

    // never reach here
    return 0;
}

#define BUFSIZE 8

void *echo(void *arg)
{
    strbuf_t strbuf;
    int count, bytes = 0;
    int com, kv, kf = 0;
    char *message, *command, *key, *value;
    char host[100], port[10], buf[2];
    struct connection *c = (struct connection *)arg;
    int error, nread, compare;

    // find out the name and port of the remote host
    error = getnameinfo((struct sockaddr *)&c->addr, c->addr_len, host, 100, port, 10, NI_NUMERICSERV);
    // we provide:
    // the address and its length
    // a buffer to write the host name, and its length
    // a buffer to write the port (as a string), and its length
    // flags, in this case saying that we want the port as a number, not a service name
    if (error != 0)
    {
        fprintf(stderr, "getnameinfo: %s", gai_strerror(error));
        close(c->fd);
        return NULL;
    }

    printf("[%s:%s] connection\n", host, port);
    sb_init(&strbuf, 10);

    while ((nread = read(c->fd, buf, 1)) > 0)
    {

        printf("[%s:%s] read %d bytes |%s|\n", host, port, nread, buf);
        if (buf[0] == '\n') // reached the end of a message so lets read the sb buffer
        {
            message = malloc(sizeof(char) * (count + 1));
            sb_word(&strbuf, count + 1, message);
            if (com == 0)
            { //When no command is set
                //set command to..
                if (strcmp(message, "GET") == 0 || strcmp(message, "SET") == 0 || strcmp(message, "DEL") == 0)
                {
                    command = malloc(sizeof(char) * (count + 1));
                    strcpy(command, message);
                    if (strcmp(command, "SET") == 0)
                        kv = 1; // boolean if command is "SET"
                    printf("Sets command to %s\n", command);
                    com = 1; // boolean if a command has been set
                }
                else
                {
                    write(c->fd, "ERR\nBAD\n", 8);
                    break;
                }
                count = 0; // number of characters since last \n
            }

            else if (bytes > 0) //When number of bytes is given
            {
                // we have key and then value coming
                //Set Key and/or value
                if (kv == 1)
                {
                    printf("Count: %d Bytes Remaining: %d\n", count + 1, bytes);
                    if (count + 1 <= bytes)
                    {
                        printf("count + 1 <= bytes\n");
                        if (kf == 1)
                        {
                            if (count + 1 != bytes)
                            {
                                write(c->fd, "ERR\nLEN\n", 8);
                                break;
                            }
            
                            printf("f = 1\n");
                            value = malloc(sizeof(char) * (count + 1));
                            strcpy(value, message);
                            printf("SET key: %s value: %s\n", key, value);
                            key_list = add(key_list, key, value);
                            write(c->fd, "OKS\n", 4);
                            count = 0;
                            com = 0;
                            bytes = 0;
                            kv = 0;
                            kf = 0;
                        }
                        else
                        {
                            key = malloc(sizeof(char) * (count + 1));
                            strcpy(key, message);
                            printf("SET key: %s\n", key);
                            bytes -= (count + 1);
                            if (bytes <= 0)
                            {
                                write(c->fd, "ERR\nLEN\n", 8);
                                break;
                            }
                            count = 0;
                            kf = 1; // boolean if key has been set yet
                        }
                    }
                    else
                    {
                        write(c->fd, "ERR\nLEN\n", 8);
                        break;
                    }
                }

                else
                {
                    // we just have key
                    if (count + 1 == bytes)
                    {
                        key = malloc(sizeof(char) * bytes);
                        strcpy(key, message);
                        printf("GET/DEL key: %s\n", key);
                        count = 0;
                        com = 0;
                        bytes = 0;
                        kv = 0;
                        kf = 0;
                    }
                    else
                    {
                        write(c->fd, "ERR\nLEN\n", 8);
                        break;
                    }
                    if (strcmp(command, "GET") == 0)
                    {
                        value = find(key_list, key);
                        if (value == NULL)
                        {
                            //key does not exist
                            write(c->fd, "KNF\n", 4);
                        }
                        else
                        {
                            write(c->fd, "OKG\n", 4);
                            int value_size = strlen(value) + 1;
                            int length = snprintf(NULL, 0, "%d", value_size);
                            char *str = malloc(length + 1);
                            snprintf(str, length + 1, "%d", value_size);
                            write(c->fd, str, length + 1);
                            write(c->fd, "\n", 1);
                            //write(c->fd, value_size, value_size + 1);
                            int err = write(c->fd, value, (value_size + 1));
                            if (err == 0)
                                perror("error writing exists: ");
                            
                            free(str);
                        }
                    }
                    if (strcmp(command, "DEL") == 0)
                    {
                        char *exists = find(key_list, key);
                        if (exists == NULL)
                        {
                            write(c->fd, "KNF\n", 4);
                        }
                        else
                        {
                            key_list = del(key_list, key);
                            write(c->fd, "OKD\n", 4);
                            int value_size = strlen(exists) + 1;
                            int length = snprintf(NULL, 0, "%d", value_size);
                            char *str = malloc(length + 1);
                            snprintf(str, length + 1, "%d", value_size);
                            write(c->fd, str, length + 1);
                            write(c->fd, "\n", 1);
                            //write(c->fd, value_size, value_size + 1);
                            int err = write(c->fd, exists, (value_size + 1));
                            if (err == 0)
                                perror("error writing exists: ");

                            write(c->fd, "\n", 1);
                            
                            free(str);
                        }
                    }
                }
            }
            else
            {
                //Set byte length
                bytes = atoi(message);
                if(bytes == 0)
                {
                    write(c->fd, "ERR\nBAD\n", 8);
                    break;
                }
                printf("byte length: %d\n", bytes);
                count = 0;
            }
            free(message);
            sb_destroy(&strbuf);
            sb_init(&strbuf, 10);
        }
        else
        {
            count++;
            sb_append(&strbuf, buf[0]);
        }
    }
    sb_destroy(&strbuf);

    printf("[%s:%s] got EOF\n", host, port);

    close(c->fd);
    free(c);
    return NULL;
}

/*while (written < buflen)
{
    bytes = write(fd, buf + written, buflen - written);
    if (bytes < 1)
    {
        //error
    }
    written += bytes;
}*/