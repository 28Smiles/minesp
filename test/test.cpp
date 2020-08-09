#include <unity.h>
#include <nbt.h>
#include <packet.h>
#include <MemStream.h>
#include <Arduino.h>

void testNBTHelloWorld() {
    uint8_t memory[256];
    MemStream stream(memory, 256);

    NBT::Compound("Hello World", {
            NBT::List("shorts", {
                    NBT::Short(42),
                    NBT::Short(42),
                    NBT::Short(42),
            }),
    })(stream);

    uint8_t reference[35] = {
            0x0A, 0x00, 0x0B, 0x48, 0x65, 0x6C, 0x6C, 0x6F,
            0x20, 0x57, 0x6F, 0x72, 0x6C, 0x64, 0x09, 0x00,
            0x06, 0x73, 0x68, 0x6F, 0x72, 0x74, 0x73, 0x02,
            0x00, 0x00, 0x00, 0x03, 0x00, 0x2A, 0x00, 0x2A,
            0x00, 0x2A, 0x00
    };

    for (unsigned int i = 0; i < 35; ++i) {
        TEST_ASSERT_EQUAL(reference[i], memory[i]);
    }
}

void testNBTDimension() {
    uint8_t memory[512];
    MemStream stream(memory, 512);

    NBT::Compound("Dimension Codec", {
            NBT::List("dimension", {
                    NBT::Compound({
                                          NBT::String("name", "minecraft:overworld"),
                                          NBT::Byte("natural", 1),
                                          NBT::Float("ambient_light", 0),
                                          NBT::Byte("has_ceiling", 1),
                                          NBT::Byte("has_skylight", 1),
                                          NBT::Long("fixed_time", 6000),
                                          NBT::Byte("shrunk", 0),
                                          NBT::Byte("ultrawarm", 0),
                                          NBT::Byte("has_raids", 0),
                                          NBT::Byte("respawn_anchor_works", 0),
                                          NBT::Byte("bed_works", 0),
                                          NBT::Byte("piglin_safe", 0),
                                          NBT::Int("logical_height", 256),
                                          NBT::String("infiniburn", "minecraft:infiburn-overworld"),
                                  })
            })
    })(stream);

    uint8_t reference[300] = {
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

    for (unsigned int i = 0; i < 300; ++i) {
        TEST_ASSERT_EQUAL(reference[i], memory[i]);
    }
}

void testPacketPosition() {
    uint8_t memory[64];
    MemStream stream(memory, 64);

    packet::wPosition(stream, 0, 64, 0, 0, 0, 0x42, 0x00);

    byte positionPacket[36] = {
            35, // Length
            0x35, // TYPE
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // X Double
            0x40, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Y Double
            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Z Double
            0x00, 0x00, 0x00, 0x00, // Yaw Float
            0x00, 0x00, 0x00, 0x00, // Pitch Float
            0x00, // FLAGS
            0x42 // TP_ID
    };

    for (unsigned int i = 0; i < 36; ++i) {
        TEST_ASSERT_EQUAL(positionPacket[i], memory[i]);
    }
}

void testPacketChunk() {
    uint8_t memory[7000];
    MemStream stream(memory, 7000);

    packet::wChunk(stream, 0, 0);

    int length = packet::util::readVarInt(stream);
    TEST_ASSERT_EQUAL(length, stream.available());
    byte type = stream.read();
    TEST_ASSERT_EQUAL(0x21, type);
}

void testPacketEmptyChunk() {
    uint8_t memory[5000];
    MemStream stream(memory, 5000);

    packet::wEmptyChunk(stream, 0, 0);

    int length = packet::util::readVarInt(stream);
    TEST_ASSERT_EQUAL(length, stream.available());
    byte type = stream.read();
    TEST_ASSERT_EQUAL(0x21, type);
}

void testPacketSpawnPlayer() {
    game::Player p;
    uint8_t memory[128];
    MemStream stream(memory, 128);

    packet::wSpawnPlayer(stream, p);

    int length = packet::util::readVarInt(stream);
    TEST_ASSERT_EQUAL(length, stream.available());
    byte type = stream.read();
    TEST_ASSERT_EQUAL(0x04, type);
}

void testPacketEntityPosition() {
    game::Player p;
    game::Player po;
    uint8_t memory[300];
    MemStream stream(memory, 300);

    packet::wEntityPosition(stream, p, po);

    int length = packet::util::readVarInt(stream);
    TEST_ASSERT_EQUAL(length, stream.available());
    byte type = stream.read();
    TEST_ASSERT_EQUAL(0x28, type);
}

void testPacketEntityPositionRotation() {
    game::Player p;
    game::Player po;
    uint8_t memory[300];
    MemStream stream(memory, 300);

    packet::wEntityPositionRotation(stream, p, po);

    int length = packet::util::readVarInt(stream);
    TEST_ASSERT_EQUAL(length, stream.available());
    byte type = stream.read();
    TEST_ASSERT_EQUAL(0x29, type);
}

void testPacketEntityRotation() {
    game::Player p;
    uint8_t memory[300];
    MemStream stream(memory, 300);

    packet::wEntityRotation(stream, p);

    int length = packet::util::readVarInt(stream);
    TEST_ASSERT_EQUAL(length, stream.available());
    byte type = stream.read();
    TEST_ASSERT_EQUAL(0x2A, type);
}

void testPacketEntityMovement() {
    game::Player p;
    uint8_t memory[300];
    MemStream stream(memory, 300);

    packet::wEntityMovement(stream, p);

    int length = packet::util::readVarInt(stream);
    TEST_ASSERT_EQUAL(length, stream.available());
    byte type = stream.read();
    TEST_ASSERT_EQUAL(0x2B, type);
}

void testPacketChatMessage() {
    uint8_t memory[300];
    MemStream stream(memory, 300);

    packet::wChatMessage(stream);

    int length = packet::util::readVarInt(stream);
    TEST_ASSERT_EQUAL(length, stream.available());
    byte type = stream.read();
    TEST_ASSERT_EQUAL(0x0E, type);
}

void testPacketKeepAlive() {
    uint8_t memory[100];
    MemStream stream(memory, 100);

    packet::wKeepAlive(stream);

    int length = packet::util::readVarInt(stream);
    TEST_ASSERT_EQUAL(length, stream.available());
    byte type = stream.read();
    TEST_ASSERT_EQUAL(0x20, type);
}

void testPacketPlayerInfo() {
    game::Player p;
    p.name = "hello_world";

    uint8_t memory[2000];
    MemStream stream(memory, 2000);

    packet::wPlayerInfoAddPlayer(stream, { p, p });

    int length = packet::util::readVarInt(stream);
    TEST_ASSERT_EQUAL(length, stream.available());
    byte type = stream.read();
    TEST_ASSERT_EQUAL(0x33, type);
}

void testPacketPlayerInfoArray() {
    game::Player p[2];
    p[0].name = "hello_world";
    p[1].name = "hello";

    uint8_t memory[2000];
    MemStream stream(memory, 2000);

    packet::wPlayerInfoAddPlayer(stream, p, 2);

    int length = packet::util::readVarInt(stream);
    TEST_ASSERT_EQUAL(length, stream.available());
    byte type = stream.read();
    TEST_ASSERT_EQUAL(0x33, type);
}

void setup() {
    UNITY_BEGIN();
    RUN_TEST(testNBTHelloWorld);
    RUN_TEST(testNBTDimension);
    RUN_TEST(testPacketPosition);
    RUN_TEST(testPacketChunk);
    RUN_TEST(testPacketEmptyChunk);
    RUN_TEST(testPacketSpawnPlayer);
    RUN_TEST(testPacketEntityPosition);
    RUN_TEST(testPacketEntityPositionRotation);
    RUN_TEST(testPacketEntityRotation);
    RUN_TEST(testPacketEntityMovement);
    RUN_TEST(testPacketChatMessage);
    RUN_TEST(testPacketKeepAlive);
    RUN_TEST(testPacketPlayerInfo);
    RUN_TEST(testPacketPlayerInfoArray);
    UNITY_END();
}

void loop() {

}
