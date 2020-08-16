# Minesp
An ESP8266 (unstable) minimal minecraft 1.16.1 Server.

## Features

Only flat world with multiple users is supported right now.

## Installation

In the `main.cpp` replace:
```
#define SSID "Voyager"
#define PASSWORD "7neHHvcuz3eMSYUE"
```
with your access point data.
This Project uses [platformio](https://docs.platformio.org/en/latest/core/quickstart.html#process-project) for compilation, upload and test. Compile and upload with running 
`plattformio run --target upload` in the project directory.
After Uploading check the serial console for the IP-Address of the esp in your local network. You can then connect to the "server" with you minecraft 1.16.1 Client. The Server will not be listed online, but you can still connect. I sometimes takes a few tries to get the chunkload to start, just disconnect and reconnect if you do not get chunk data.
And there you have it, your embedded minecraft server.
