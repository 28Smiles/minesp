#include <Arduino.h>
#include <vector>
#include <game.h>


namespace packet {
    namespace util {
        typedef byte uuid[16];

        int readVarInt(Stream &stream) {
            int numRead = 0;
            int result = 0;
            byte b;
            do {
                b = stream.read();
                int value = (b & 0b01111111);
                result |= (value << (7 * numRead));

                numRead++;
                if (numRead > 5) {
                    return result;
                }
            } while ((b & 0b10000000) != 0);

            return result;
        }

        int varIntSize(unsigned int n) {
            if (n <= 0x7F) {
                return 1;
            } else if (n <= 0x3FFF) {
                return 2;
            } else if (n <= 0x1FFFF) {
                return 3;
            } else if (n <= 0xFFFFFFF) {
                return 4;
            } else {
                return 5;
            }
        }

        void writeVarInt(Stream &stream, const int &value) {
            unsigned int uvalue = (unsigned int) value;
            do {
                byte temp = (byte) (uvalue & 0b01111111);
                uvalue = uvalue >> 7;
                if (uvalue != 0) {
                    temp |= 0b10000000;
                }
                stream.write(temp);
            } while (uvalue != 0);
        }

        void writeBoolean(Stream &stream, const bool &value) {
            stream.write(value ? 0x01 : 0x00);
        }

        void writeShort(Stream &stream, const short &value) {
            stream.write(((byte *) &value)[1]);
            stream.write(((byte *) &value)[0]);
        }

        void writeInt(Stream &stream, const int &value) {
            stream.write(((byte *) &value)[3]);
            stream.write(((byte *) &value)[2]);
            stream.write(((byte *) &value)[1]);
            stream.write(((byte *) &value)[0]);
        }

        void writeFloat(Stream &stream, const float &value) {
            stream.write(((byte *) &value)[3]);
            stream.write(((byte *) &value)[2]);
            stream.write(((byte *) &value)[1]);
            stream.write(((byte *) &value)[0]);
        }

        void writeDouble(Stream &stream, const double &value) {
            stream.write(((byte *) &value)[7]);
            stream.write(((byte *) &value)[6]);
            stream.write(((byte *) &value)[5]);
            stream.write(((byte *) &value)[4]);
            stream.write(((byte *) &value)[3]);
            stream.write(((byte *) &value)[2]);
            stream.write(((byte *) &value)[1]);
            stream.write(((byte *) &value)[0]);
        }

        void writeLong(Stream &stream, const long long &value) {
            stream.write(((byte *) &value)[7]);
            stream.write(((byte *) &value)[6]);
            stream.write(((byte *) &value)[5]);
            stream.write(((byte *) &value)[4]);
            stream.write(((byte *) &value)[3]);
            stream.write(((byte *) &value)[2]);
            stream.write(((byte *) &value)[1]);
            stream.write(((byte *) &value)[0]);
        }

        void writeUUID(Stream &stream, const uuid &value) {
            stream.write(value, 16);
        }

        void writeString(Stream &stream, const String &value) {
            stream.write(value.length());
            stream.write(value.begin(), value.length());
        }

        template<int bufferSize> String readString(Stream &stream) {
            int size = stream.read();
            char buffer[bufferSize];
            stream.readBytes(buffer, size);
            buffer[size] = '\0';

            return String(buffer);
        }

        float readFloat(Stream &stream) {
            union {
                float value;
                byte bytes[4];
            } v;

            v.bytes[3] = stream.read();
            v.bytes[2] = stream.read();
            v.bytes[1] = stream.read();
            v.bytes[0] = stream.read();

            return v.value;
        }

        double readDouble(Stream &stream) {
            union {
                double value;
                byte bytes[8];
            } v;

            v.bytes[7] = stream.read();
            v.bytes[6] = stream.read();
            v.bytes[5] = stream.read();
            v.bytes[4] = stream.read();
            v.bytes[3] = stream.read();
            v.bytes[2] = stream.read();
            v.bytes[1] = stream.read();
            v.bytes[0] = stream.read();

            return v.value;
        }
    }

    const byte defaultDimensionNBT[300] = {
            0x0A, 0x00, 0x0F, 0x44, 0x69, 0x6D, 0x65, 0x6E, // 1
            0x73, 0x69, 0x6F, 0x6E, 0x20, 0x43, 0x6F, 0x64,
            0x65, 0x63, 0x09, 0x00, 0x09, 0x64, 0x69, 0x6D,
            0x65, 0x6E, 0x73, 0x69, 0x6F, 0x6E, 0x0A, 0x00,
            0x00, 0x00, 0x01, 0x08, 0x00, 0x04, 0x6E, 0x61, // 5
            0x6D, 0x65, 0x00, 0x13, 0x6D, 0x69, 0x6E, 0x65,
            0x63, 0x72, 0x61, 0x66, 0x74, 0x3A, 0x6F, 0x76,
            0x65, 0x72, 0x77, 0x6F, 0x72, 0x6C, 0x64, 0x01,
            0x00, 0x07, 0x6E, 0x61, 0x74, 0x75, 0x72, 0x61,
            0x6C, 0x01, 0x05, 0x00, 0x0D, 0x61, 0x6D, 0x62, // 10
            0x69, 0x65, 0x6E, 0x74, 0x5F, 0x6C, 0x69, 0x67,
            0x68, 0x74, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00,
            0x0B, 0x68, 0x61, 0x73, 0x5F, 0x63, 0x65, 0x69,
            0x6C, 0x69, 0x6E, 0x67, 0x01, 0x01, 0x00, 0x0C,
            0x68, 0x61, 0x73, 0x5F, 0x73, 0x6B, 0x79, 0x6C, // 15
            0x69, 0x67, 0x68, 0x74, 0x01, 0x04, 0x00, 0x0A,
            0x66, 0x69, 0x78, 0x65, 0x64, 0x5F, 0x74, 0x69,
            0x6D, 0x65, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x17, 0x70, 0x01, 0x00, 0x06, 0x73, 0x68, 0x72,
            0x75, 0x6E, 0x6B, 0x00, 0x01, 0x00, 0x09, 0x75, // 20
            0x6C, 0x74, 0x72, 0x61, 0x77, 0x61, 0x72, 0x6D,
            0x00, 0x01, 0x00, 0x09, 0x68, 0x61, 0x73, 0x5F,
            0x72, 0x61, 0x69, 0x64, 0x73, 0x00, 0x01, 0x00,
            0x14, 0x72, 0x65, 0x73, 0x70, 0x61, 0x77, 0x6E,
            0x5F, 0x61, 0x6E, 0x63, 0x68, 0x6F, 0x72, 0x5F, // 25
            0x77, 0x6F, 0x72, 0x6B, 0x73, 0x00, 0x01, 0x00,
            0x09, 0x62, 0x65, 0x64, 0x5F, 0x77, 0x6F, 0x72,
            0x6B, 0x73, 0x00, 0x01, 0x00, 0x0B, 0x70, 0x69,
            0x67, 0x6C, 0x69, 0x6E, 0x5F, 0x73, 0x61, 0x66,
            0x65, 0x00, 0x03, 0x00, 0x0E, 0x6C, 0x6F, 0x67, // 30
            0x69, 0x63, 0x61, 0x6C, 0x5F, 0x68, 0x65, 0x69,
            0x67, 0x68, 0x74, 0x00, 0x00, 0x01, 0x00, 0x08,
            0x00, 0x0A, 0x69, 0x6E, 0x66, 0x69, 0x6E, 0x69,
            0x62, 0x75, 0x72, 0x6E, 0x00, 0x1C, 0x6D, 0x69,
            0x6E, 0x65, 0x63, 0x72, 0x61, 0x66, 0x74, 0x3A, // 35
            0x69, 0x6E, 0x66, 0x69, 0x62, 0x75, 0x72, 0x6E,
            0x2D, 0x6F, 0x76, 0x65, 0x72, 0x77, 0x6F, 0x72,
            0x6C, 0x64, 0x00, 0x00
    };
    const util::uuid playerId = {
            0x45, 0x66, 0xE6, 0x9F, 0xC9, 0x07, 0x48, 0xEE, 0x8D, 0x71, 0xD7, 0xBA, 0x5A, 0xA0, 0x0D, 0x20
    };

    class Handshake {
    public:
        static constexpr byte packetId = 0x00;
        int protocolVersion;
        int serverAddressLength;
        char serverAddress[255];
        unsigned short port;
        int nextState;
        Handshake(Stream &stream) {
            this->protocolVersion = util::readVarInt(stream);
            this->serverAddressLength = util::readVarInt(stream);
            stream.readBytes(this->serverAddress, serverAddressLength);
            this->port = (((short) stream.read()) << 8) | stream.read();
            this->nextState = util::readVarInt(stream);
        }
    };

    String rLoginStart(Stream &stream) {
        return util::readString<17>(stream);
    }

    void wLoginSuccess(Stream &stream, String &username, util::uuid &uuid) {
        const byte size = username.length() + sizeof(util::uuid) + 2;
        stream.write(size);
        stream.write(0x02);
        util::writeUUID(stream, uuid);
        util::writeString(stream, username);
    }

    void wJoinGame(
            Stream &stream,
            int entityId,
            String worldName,
            Stream &nbtData,
            String dimensionIdentifier,
            String worldIdentifier,
            byte maxPlayers,
            byte viewDistance = 2,
            bool reducedDebug = false,
            bool isDebug = false,
            bool enableRespawnScreen = false,
            bool isFlat = false,
            long long seed = 0,
            byte gamemode = 0x00,
            byte prevGamemode = 0x00) {
        int size = 22;
        size += worldName.length() + 1;
        size += nbtData.available();
        size += dimensionIdentifier.length() + 1;
        size += worldIdentifier.length() + 1;

        util::writeVarInt(stream, size);
        stream.write(0x25);
        util::writeInt(stream, entityId);
        stream.write(gamemode);
        stream.write(prevGamemode);
        stream.write(1);
        util::writeString(stream, worldName);

        const int available = stream.available();
        for (int i = 0; i < available; ++i) {
            stream.write(nbtData.read());
        }

        util::writeString(stream, dimensionIdentifier);
        util::writeString(stream, worldIdentifier);
        stream.write((byte*) &seed, 8);
        stream.write(maxPlayers);
        stream.write(viewDistance);
        util::writeBoolean(stream, reducedDebug);
        util::writeBoolean(stream, enableRespawnScreen);
        util::writeBoolean(stream, isDebug);
        util::writeBoolean(stream, isFlat);
    }

    void wJoinGame(
            Stream &stream,
            const int entityId,
            const String worldName,
            const String dimensionIdentifier,
            const String worldIdentifier,
            const byte maxPlayers,
            const byte *nbtData = defaultDimensionNBT,
            const short nbtDataLength = 300,
            const byte viewDistance = 8,
            const bool reducedDebug = false,
            const bool isDebug = false,
            const bool enableRespawnScreen = false,
            const bool isFlat = false,
            const long long seed = 0,
            const byte gamemode = 0x00,
            const byte prevGamemode = 0x00) {
        int size = 22;
        size += worldName.length() + 1;
        size += nbtDataLength;
        size += dimensionIdentifier.length() + 1;
        size += worldIdentifier.length() + 1;

        util::writeVarInt(stream, size);
        stream.write(0x25);
        util::writeInt(stream, entityId);
        stream.write(gamemode);
        stream.write(prevGamemode);
        stream.write(1);
        util::writeString(stream, worldName);
        stream.write(nbtData, nbtDataLength);
        util::writeString(stream, dimensionIdentifier);
        util::writeString(stream, worldIdentifier);
        stream.write((byte*) &seed, 8);
        stream.write(maxPlayers);
        stream.write(viewDistance);
        util::writeBoolean(stream, reducedDebug);
        util::writeBoolean(stream, enableRespawnScreen);
        util::writeBoolean(stream, isDebug);
        util::writeBoolean(stream, isFlat);
    }

    void wPosition(Stream &stream,
                   const double x,
                   const double y,
                   const double z,
                   const float yaw,
                   const float pitch,
                   const byte tpId = 0x42,
                   const byte flags = 0x00) {
        stream.write(35);
        stream.write(0x35);
        util::writeDouble(stream, x);
        util::writeDouble(stream, y);
        util::writeDouble(stream, z);
        util::writeFloat(stream, yaw);
        util::writeFloat(stream, pitch);
        stream.write(flags);
        stream.write(tpId);
    }

    void wChunk(Stream &stream,
                int x, int y,
                byte sections = 0b00001000,
                bool fullChunk = true,
                bool ignoreOld = true) {
        int size = 6493;
        util::writeVarInt(stream, size);
        stream.write(0x21);
        util::writeInt(stream, x);
        util::writeInt(stream, y);
        util::writeBoolean(stream, fullChunk);
        util::writeBoolean(stream, ignoreOld);
        stream.write(sections);
        // HEIGHTMAP (FLAT)
        byte nbtHead[36] = {
                0x0A, 0x00, 0x0A, 0x48, 0x65, 0x69, 0x67, 0x68, 0x74, 0x6D, 0x61, 0x70, 0x73, 0x09, 0x00, 0x0F,
                0x4D, 0x4F, 0x54, 0x49, 0x4F, 0x4E, 0x5F, 0x42, 0x4C, 0x4F, 0x43, 0x4B, 0x49, 0x4E, 0x47, 0x04,
                0x00, 0x00, 0x00, 0x24
        };
        stream.write(nbtHead, 36);
        byte nbtBodyPart[9] = {
                0x1F, 0x8F, 0xC7, 0xE3, 0xF1, 0xF8, 0xFC, 0x7E, 0x3F
        };
        for (int i = 0; i < 32; ++i) {
            stream.write(nbtBodyPart, 9);
        }
        stream.write(0x00); // NBT END
        // BIOMES
        for (int i = 0; i < 1024; ++i) {
            stream.write(0x00);
            stream.write(0x00);
            stream.write(0x00);
            stream.write(0x01);
        }

        int chunkSize = 2057;
        util::writeVarInt(stream, chunkSize);
        short blockCount = 4096;
        util::writeShort(stream, blockCount);
        stream.write(4); // Bits per Block
        // PALETTE
        stream.write(3);
        stream.write(0x01); // STONE
        stream.write(0x09); // Grass
        stream.write(0x0A); // Dirt
        // CHUNK
        // LONG ARRAY SIZE
        stream.write(0b10000000);
        stream.write(0b00000010);

        for (int i = 0; i < 12 * 128; ++i) {
            stream.write(0x00);
        }
        for (int i = 0; i < 3 * 128; ++i) {
            stream.write(0x22);
        }
        for (int i = 0; i < 1 * 128; ++i) {
            stream.write(0x11);
        }

        stream.write(0x00); // EMPTY ENTITY LIST
    }

    void wEmptyChunk(Stream &stream,
                     const int x,
                     const int y,
                     const bool fullChunk = true,
                     const bool ignoreOld = true) {
        int size = 4435;
        util::writeVarInt(stream, size);
        stream.write(0x21);
        util::writeInt(stream, x);
        util::writeInt(stream, y);
        util::writeBoolean(stream, fullChunk);
        util::writeBoolean(stream, ignoreOld);
        stream.write(0x00); // No section
        // HEIGHTMAP (FLAT)
        byte nbtHead[36] = {
                0x0A, 0x00, 0x0A, 0x48, 0x65, 0x69, 0x67, 0x68, 0x74, 0x6D, 0x61, 0x70, 0x73, 0x09, 0x00, 0x0F,
                0x4D, 0x4F, 0x54, 0x49, 0x4F, 0x4E, 0x5F, 0x42, 0x4C, 0x4F, 0x43, 0x4B, 0x49, 0x4E, 0x47, 0x04,
                0x00, 0x00, 0x00, 0x24
        };
        stream.write(nbtHead, 36);
        byte nbtBodyPart[9] = {
                0x1F, 0x8F, 0xC7, 0xE3, 0xF1, 0xF8, 0xFC, 0x7E, 0x3F
        };
        for (int i = 0; i < 32; ++i) {
            stream.write(nbtBodyPart, 9);
        }
        stream.write(0x00); // NBT END
        // BIOMES
        for (int i = 0; i < 1024; ++i) {
            stream.write(0x00);
            stream.write(0x00);
            stream.write(0x00);
            stream.write(0x01);
        }

        int chunkSize = 0;
        util::writeVarInt(stream, chunkSize);
        stream.write(0x00); // EMPTY ENTITY LIST
    }

    void rPlayerPosition(Stream &stream, game::Player &player) {
        player.posX = util::readDouble(stream);
        player.posY = util::readDouble(stream);
        player.posZ = util::readDouble(stream);
        stream.read(); // ON GROUND
    }

    void rPlayerPositionAndRotation(Stream &stream, game::Player &player) {
        player.posX = util::readDouble(stream);
        player.posY = util::readDouble(stream);
        player.posZ = util::readDouble(stream);

        player.yaw = util::readFloat(stream);
        player.pitch = util::readFloat(stream);

        stream.read(); // ON GROUND
    }

    void rPlayerRotation(Stream &stream, game::Player &player) {
        player.yaw = util::readFloat(stream);
        player.pitch = util::readFloat(stream);

        stream.read(); // ON GROUND
    }

    void rPlayerMovement(Stream &stream) {
        stream.read(); // ON GROUND
    }

    void wKeepAlive(Stream &stream) {
        stream.write(9);
        stream.write(0x20);
        long long i = 0x42;
        util::writeLong(stream, i);
    }

    void wSpawnPlayer(Stream &stream, const game::Player &player) {
        stream.write(44);
        stream.write(0x04);
        stream.write(player.eid);
        util::writeUUID(stream, playerId);
        util::writeDouble(stream, player.posX);
        util::writeDouble(stream, player.posY);
        util::writeDouble(stream, player.posZ);
        stream.write(((int) abs(player.yaw / 360 / (1./256.))) % 256);
        stream.write(((int) abs(player.pitch / 360 / (1./256.))) % 256);
    }

    void wEntityPosition(Stream &stream, const game::Player &playerOld, const game::Player &playerNew) {
        stream.write(9);
        stream.write(0x28);
        stream.write(playerOld.eid);
        short dX = (playerNew.posX * 32 - playerOld.posX * 32) * 128;
        util::writeShort(stream, dX);
        short dY = (playerNew.posY * 32 - playerOld.posY * 32) * 128;
        util::writeShort(stream, dY);
        short dZ = (playerNew.posZ * 32 - playerOld.posZ * 32) * 128;
        util::writeShort(stream, dZ);
        stream.write(0x00);
    }

    void wEntityPositionRotation(Stream &stream, const game::Player &playerOld, const game::Player &playerNew) {
        stream.write(11);
        stream.write(0x29);
        stream.write(playerOld.eid);
        short dX = (playerNew.posX * 32 - playerOld.posX * 32) * 128;
        util::writeShort(stream, dX);
        short dY = (playerNew.posY * 32 - playerOld.posY * 32) * 128;
        util::writeShort(stream, dY);
        short dZ = (playerNew.posZ * 32 - playerOld.posZ * 32) * 128;
        util::writeShort(stream, dZ);
        stream.write(((int) abs(playerNew.yaw / 360 / (1./256.))) % 256);
        stream.write(((int) abs(playerNew.pitch / 360 / (1./256.))) % 256);
        stream.write(0x00);
    }

    void wEntityRotation(Stream &stream, const game::Player &player) {
        stream.write(5);
        stream.write(0x2A);
        stream.write(player.eid);
        stream.write(((int) abs(player.yaw / 360 / (1./256.))) % 256);
        stream.write(((int) abs(player.pitch / 360 / (1./256.))) % 256);
        stream.write(0x00);
    }

    void wEntityMovement(Stream &stream, const game::Player &player) {
        stream.write(2);
        stream.write(0x2B);
        stream.write(player.eid);
    }

    const String welcomeMessage = "{ \"translate\": \"chat.type.announcement\", \"with\": [ \"Hello World!\" ] }";
    void wChatMessage(Stream &stream, const String &message = welcomeMessage, const byte pos = 0x01) {
        int size = 1 + 1 + message.length() + 1 + 16;
        util::writeVarInt(stream, size);
        stream.write(0x0E);
        util::writeString(stream, message);
        stream.write(pos);
        for (int i = 0; i < 16; ++i) {
            stream.write(0);
        }
    }

    const String b64Texture = "ewogICJ0aW1lc3RhbXAiIDogMTU5Njk4NDU0MjI2MywKICAicHJvZmlsZUlkIiA6ICI0NTY2ZTY5ZmM5MDc0OGVlOGQ3MWQ3YmE1YWEwMGQyMCIsCiAgInByb2ZpbGVOYW1lIiA6ICJUaGlua29mZGVhdGgiLAogICJ0ZXh0dXJlcyIgOiB7CiAgICAiU0tJTiIgOiB7CiAgICAgICJ1cmwiIDogImh0dHA6Ly90ZXh0dXJlcy5taW5lY3JhZnQubmV0L3RleHR1cmUvNzRkMWUwOGIwYmI3ZTlmNTkwYWYyNzc1ODEyNWJiZWQxNzc4YWM2Y2VmNzI5YWVkZmNiOTYxM2U5OTExYWU3NSIKICAgIH0sCiAgICAiQ0FQRSIgOiB7CiAgICAgICJ1cmwiIDogImh0dHA6Ly90ZXh0dXJlcy5taW5lY3JhZnQubmV0L3RleHR1cmUvYjBjYzA4ODQwNzAwNDQ3MzIyZDk1M2EwMmI5NjVmMWQ2NWExM2E2MDNiZjY0YjE3YzgwM2MyMTQ0NmZlMTYzNSIKICAgIH0KICB9Cn0=";
    void wPlayerInfoAddPlayer(Stream &stream, std::initializer_list<game::Player> players) {
        int size = 3;
        for (unsigned int i = 0; i < players.size(); ++i) {
            size += 16 + 1 + players.begin()[i].name.length() + 1 + 1 + 8 + 2 + b64Texture.length() + 4;
        }

        util::writeVarInt(stream, size);
        stream.write(0x33);
        stream.write(0);
        stream.write(players.size());
        for (unsigned int i = 0; i < players.size(); ++i) {
            util::writeUUID(stream, playerId);
            util::writeString(stream, players.begin()[i].name);
            stream.write(1);
            util::writeString(stream, "textures");
            util::writeVarInt(stream, b64Texture.length());
            stream.write(b64Texture.begin(), b64Texture.length());
            stream.write(0);
            stream.write(0);
            stream.write(20);
            stream.write(0x00);
        }
    }

    void wPlayerInfoAddPlayer(Stream &stream, game::Player *players, int length) {
        int size = 3;
        for (int i = 0; i < length; ++i) {
            size += 16 + 1 + players[i].name.length() + 1 + 1 + 8 + 2 + b64Texture.length() + 4;
        }

        util::writeVarInt(stream, size);
        stream.write(0x33);
        stream.write(0);
        stream.write(length);
        for (int i = 0; i < length; ++i) {
            util::writeUUID(stream, playerId);
            util::writeString(stream, players[i].name);
            stream.write(1);
            util::writeString(stream, "textures");
            util::writeVarInt(stream, b64Texture.length());
            stream.write(b64Texture.begin(), b64Texture.length());
            stream.write(0);
            stream.write(0);
            stream.write(20);
            stream.write(0x00);
        }
    }
}
