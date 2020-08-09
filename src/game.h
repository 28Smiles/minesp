#include <Arduino.h>


namespace game {
    struct Coordinate {
        byte x;
        byte y;
    };

    struct Player {
        int eid;
        String name;
        double posX = 0;
        double posY = 0;
        double posZ = 0;
        float yaw = 0;
        float pitch = 0;
        byte chunksLoaded[8];
        byte state = 0;
        long long lastKeepalive;

        void unloadAllChunks() {
            for (byte x = 0; x < 8; ++x) {
                chunksLoaded[x] = 0;
            }
        }
        bool isChunkLoaded(byte &x, byte &y) {
            return 0x1 & (chunksLoaded[x] >> y);
        }
        void setChunkLoaded(byte &x, byte &y) {
            chunksLoaded[x] |= 0x1 << y;
        }
        Coordinate nextChunk() {
            for (byte x = 0; x < 8; ++x) {
                byte chunkInfo = chunksLoaded[x];
                if (chunkInfo < 0xFF) {
                    for (byte y = 0; y < 8; ++y) {
                        if (!isChunkLoaded(x, y)) {
                            return { x, y };
                        }
                    }
                }
            }
            return { 255, 255 };
        }
    };
}
