ndt26

shs162

To be compliled using Server.c linkedlist.h strbuf.h 
    as well as -pthread flag like so

    gcc -pthread Server.c linkedlist.h strbuf.h

We tested multiple cases such as:
    - opening multiple clients at the same time and entering commands simulatneously
    - setting keys in one client and getting/deleting in another
    - many variations of keys and values
    - all error cases such as length, malformed message, server disconnection, key not found
    - if the server disconnects while a client is still connected we report SRV to any clients still connected
    - unknown commands