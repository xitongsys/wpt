# WPT

WPT(windows port tunnel) is a windows port remote mapping program.
* Mapping your local host ports to a server.
* Mapping a server ports to your local host.

![Mechanism](https://github.com/xitongsys/wpt/blob/master/mechanism.jpg?raw=true)

## Quick start
The package gives two executable files: ```wpt-ui.exe``` and ```wpt.exe```. ```wpt-ui.exe``` gives a gui and needs .NET4. ```wpt.exe``` is a console version.

* Download the release binary file.
* Configure the config.txt or use ```wpt-ui.exe``` which gives a gui.
* Run the wpt on server and client hosts separately.
* Enjoy ~

## Configuration file
Config file is a json format file (config.txt). To understand the items, your can see the picture before.
```json
{
    "role": "client",
    "server": "0.0.0.0:22222",
	"direction": 0,
	"client_tun_ports": [1111, 1112],
	"server_tun_ports": [11111, 11122]
}
```
