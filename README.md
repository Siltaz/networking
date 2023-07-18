# TCP Chat Server

Starting chat server:

```sh
make
cd build
# ./chat_server <PORT>
./chat_server 65001
```

Starting chat client (New Terminal):

```sh
make
cd build
# ./chat_client <IP_ADDRESS> <PORT>
./chat_client 127.0.0.1 65001
```
