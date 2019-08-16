# WPT

WPT(windows port tunnel) is a windows port remote mapping programs.
* Mapping your local port to a public server.
* Mapping a public server port to your local host.

![Mechanism](https://github.com/xitongsys/wpt/blob/master/mechanism.jpg?raw=true)

## Quick start

### Configuration file
Config file is a json format file
```json
{
    "role": "client",
    "server": "0.0.0.0:22222",
	"direction": 0,
	"client_tun_ports": [1111, 1112],
	"server_tun_ports": [11111, 11122]
}
```