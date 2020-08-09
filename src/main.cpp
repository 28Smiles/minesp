#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <packet.h>

#define SSID "Voyager"
#define PASSWORD "7neHHvcuz3eMSYUE"
#define PORT 25565

WiFiServer tcpServer(PORT);

// Objects for connections
#define MAX_TCP_CONNECTIONS 4
WiFiClient clients[MAX_TCP_CONNECTIONS];

// Buffer for incoming text
#define BUFFER_SIZE 256
char tcp_buffer[MAX_TCP_CONNECTIONS][BUFFER_SIZE];

game::Player players[MAX_TCP_CONNECTIONS];


/** Optional: Notify about AP connection status changes */
void check_ap_connection()
{
    static wl_status_t preStatus = WL_DISCONNECTED;

    wl_status_t newStatus = WiFi.status();
    if (newStatus != preStatus)
    {
        if (newStatus == WL_CONNECTED)
        {
            digitalWrite(LED_BUILTIN, LOW);

            // Display the own IP address and port
            Serial.print(F("AP connection established, listening on "));
            Serial.print(WiFi.localIP());
            Serial.print(":");
            Serial.println(PORT);
        }
        else
        {
            digitalWrite(LED_BUILTIN, HIGH);
            Serial.println(F("AP conection lost"));
        }
        preStatus = newStatus;
    }
}


/**
 * Put new connections into the array and
 * send a welcome message.
 */
void handle_new_connections()
{
    WiFiClient client = tcpServer.available();
    if (client)
    {
        Serial.print(F("New connection from "));
        Serial.println(client.remoteIP().toString());

        // Find a free space in the array
        for (int i = 0; i < MAX_TCP_CONNECTIONS; i++)
        {
            if (!clients[i].connected())
            {
                // Found free space
                clients[i] = client;
                players[i].state = 0;
                players[i].eid = i;
                players[i].lastKeepalive = millis();
                players[i].unloadAllChunks();
                tcp_buffer[i][0]='\0';
                Serial.print(F("Kanal="));
                Serial.println(i);

                return;
            }
        }
        Serial.println(F("To many connections"));
        client.stop();
    }
}

/** Receive TCP messages and send echo back */
void process_incoming_tcp()
{
    static int i=0;

    // Only one connection is checked in each call
    if (clients[i].available())
    {
        auto client = clients[i];
        int packetLength = packet::util::readVarInt(client);
        int packetType = packet::util::readVarInt(client);
        if (packetType == 0 && players[i].state == 0) { // Handshake
            packet::Handshake handshake(client);

            Serial.print("Protocol Version: ");
            Serial.println(handshake.protocolVersion);
            Serial.print("Port: ");
            Serial.println(handshake.port);
            Serial.print("Next State: ");
            Serial.println(handshake.nextState);

            players[i].state = handshake.nextState;
        } else if (packetType == 0 && players[i].state == 2) { // Login
            String playerName = packet::rLoginStart(client);

            Serial.print("Login Player: ");
            Serial.write(playerName.begin(), playerName.length());
            Serial.println();

            players[i].name = playerName;
            packet::util::uuid uuid { 42, 42, 42, 42, 42, 42, 42, 42 };
            String worldIdentifier = "minecraft:overworld";

            packet::wLoginSuccess(client, playerName, uuid);
            client.flush();
            packet::wJoinGame(client, 42, worldIdentifier, worldIdentifier, worldIdentifier, 2);
            client.flush();
            packet::wPosition(client,3 * 16, 64, 3 * 16, 0, 0, 0x42, 0x00);
            client.flush();

            players[i].state = 3;
        } else if (packetType == 0 && players[i].state == 3) { // Player Spawned
            Serial.print("Client Spawned at tpid ");
            Serial.println(packet::util::readVarInt(client), HEX);
            players[i].state = 4;

            // UPDATE INFO
            // Add all players to this player
            packet::wPlayerInfoAddPlayer(clients[i], players, MAX_TCP_CONNECTIONS);
            for (byte pid = 0; pid < MAX_TCP_CONNECTIONS; ++pid) {
                if (pid != i && clients[pid].connected()) {
                    // Add player to others
                    packet::wPlayerInfoAddPlayer(clients[pid], { players[i] });
                }
                clients[pid].flush();
            }

            // ANNOUNCE
            for (byte pid = 0; pid < MAX_TCP_CONNECTIONS; ++pid) {
                if (pid != i && clients[pid].connected()) {
                    packet::wSpawnPlayer(clients[i], players[pid]);
                }
            }
            for (byte pid = 0; pid < MAX_TCP_CONNECTIONS; ++pid) {
                if (pid != i && clients[pid].connected()) {
                    packet::wSpawnPlayer(clients[pid], players[i]);
                }
            }

            packet::wChatMessage(client);
        } else if (packetType == 0x12 && players[i].state == 4) { // Player Position
            game::Player p;
            packet::rPlayerPosition(client, p);
            for (byte pid = 0; pid < MAX_TCP_CONNECTIONS; ++pid) {
                if (pid != i && clients[pid].connected()) {
                    packet::wEntityPosition(clients[pid], players[i], p);
                }
            }
            players[i].posX = p.posX;
            players[i].posY = p.posY;
            players[i].posZ = p.posZ;
        } else if (packetType == 0x13 && players[i].state == 4) { // Player Position And Rotation
            game::Player p;
            packet::rPlayerPositionAndRotation(client, p);
            for (byte pid = 0; pid < MAX_TCP_CONNECTIONS; ++pid) {
                if (pid != i && clients[pid].connected()) {
                    packet::wEntityPositionRotation(clients[pid], players[i], p);
                }
            }
            players[i].posX = p.posX;
            players[i].posY = p.posY;
            players[i].posZ = p.posZ;
            players[i].yaw = p.yaw;
            players[i].pitch = p.pitch;
        } else if (packetType == 0x14 && players[i].state == 4) { // Player Rotation
            packet::rPlayerRotation(client, players[i]);
            for (byte pid = 0; pid < MAX_TCP_CONNECTIONS; ++pid) {
                if (pid != i && clients[pid].connected()) {
                    packet::wEntityRotation(clients[pid], players[i]);
                }
            }
        } else if (packetType == 0x15 && players[i].state == 4) { // Player Rotation
            packet::rPlayerMovement(client);
            for (byte pid = 0; pid < MAX_TCP_CONNECTIONS; ++pid) {
                if (pid != i && clients[pid].connected()) {
                    packet::wEntityMovement(clients[pid], players[i]);
                }
            }
        } else {
            Serial.print("Received Packet of type ");
            Serial.print(packetType, HEX);
            if (packetLength < 255) {
                Serial.println(":");
                while (packetLength > 0) {
                    --packetLength;
                    Serial.print(client.read(), HEX);
                    Serial.print(" ");
                }
            }
            Serial.println();
        }
    }

    if (clients[i].connected() && players[i].state == 4) {
        if (millis() - players[i].lastKeepalive > 10000) { // Every 10 Sek
            Serial.println("Sending keepalive...");
            players[i].lastKeepalive = millis();
            packet::wKeepAlive(clients[i]);
            clients[i].flush();
        }
        game::Coordinate coordinate = players[i].nextChunk();
        if (coordinate.x != 255) {
            Serial.print("Sending Chunk Data for Chunk (");
            Serial.print(coordinate.x, DEC);
            Serial.print(", ");
            Serial.print(coordinate.y, DEC);
            Serial.println(")...");
            players[i].setChunkLoaded(coordinate.x, coordinate.y);
            if (coordinate.x > 2 && coordinate.x <= 6 && coordinate.y > 2 && coordinate.y <= 6) {
                packet::wChunk(clients[i], coordinate.x, coordinate.y);
            } else {
                packet::wEmptyChunk(clients[i], coordinate.x, coordinate.y);
            }
            clients[i].flush();
        }
    }

    // Switch to the next connection for the next call
    if (++i >= MAX_TCP_CONNECTIONS)
    {
        i=0;
    }
}


/** Executes once during start*/
void setup()
{
    // LED off
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    // Initialize the serial port
    Serial.begin(115200);

    // Give the serial monitor of the Arduino IDE time to start
    delay(500);

    // Use an external AP
    WiFi.mode(WIFI_STA);
    WiFi.begin(SSID, PASSWORD);

    // Start the TCP server
    tcpServer.begin();
}


/** Main loop, executed repeatedly */
void loop()
{
    handle_new_connections();
    process_incoming_tcp();
    check_ap_connection();
}
