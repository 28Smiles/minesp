#include <Arduino.h>

#define TAG(NAME, TYPE, TYPEID, WRITER) \
inline TagWriter NAME(TYPE value) {\
    return TagWriter {                  \
        [](Stream &stream) {\
            stream.write((byte) TYPEID);\
        },\
        [value](Stream &stream) {\
            WRITER\
        }\
    };\
}\
\
inline NamedTagWriter NAME(const class String name, TYPE value) { \
    return NamedTagWriter {\
        [](Stream &stream) {\
            stream.write((byte) TYPEID);\
        },\
        [name](Stream &stream) {\
            unsigned short length = name.length();\
            stream.write(((byte *) &length)[1]);\
            stream.write(((byte *) &length)[0]);\
            stream.write(name.begin(), length);\
        },\
        [value](Stream &stream) {\
            WRITER\
        }\
    };\
}

#define BASIC_TAG(NAME, TYPE, TYPEID) TAG(NAME, TYPE, TYPEID, stream.write(value);)
#define TAG_TYPE(NAME, TYPEID) constexpr inline byte NAME() { return TYPEID; }

namespace NBT {
    struct TagWriter {
        const std::function<void(Stream &)> type;
        const std::function<void(Stream &)> value;

        inline void operator()(Stream &stream) {
            type(stream);
            value(stream);
        }
    };

    struct NamedTagWriter {
        const std::function<void(Stream &)> type;
        const std::function<void(Stream &)> name;
        const std::function<void(Stream &)> value;

        inline void operator()(Stream &stream) {
            type(stream);
            name(stream);
            value(stream);
        }
    };

    TAG_TYPE(Byte, 1)

    TAG_TYPE(Short, 2)

    TAG_TYPE(Int, 3)

    TAG_TYPE(Long, 4)

    TAG_TYPE(Float, 5)

    TAG_TYPE(Double, 6)

    TAG_TYPE(ByteArray, 7)

    TAG_TYPE(String, 8)

    TAG_TYPE(List, 9)

    TAG_TYPE(Compound, 10)

    BASIC_TAG(Byte, byte, 1)

    TAG(Short, short, 2, stream.write(((byte *) &value)[1]); stream.write(((byte *) &value)[0]);)

    TAG(Int, int, 3, stream.write(((byte *) &value)[3]); stream.write(((byte *) &value)[2]); stream.write(((byte *) &value)[1]); stream.write(((byte *) &value)[0]);)

    TAG(Long, std::int64_t, 4,
        stream.write(((byte *) &value)[7]);
        stream.write(((byte *) &value)[6]);
        stream.write(((byte *) &value)[5]);
        stream.write(((byte *) &value)[4]);
        stream.write(((byte *) &value)[3]);
        stream.write(((byte *) &value)[2]);
        stream.write(((byte *) &value)[1]);
        stream.write(((byte *) &value)[0]);)

    TAG(Float, float, 5, stream.write((byte *) &value, sizeof(float));)

    TAG(Double, double, 6, stream.write((byte *) &value, sizeof(double));)

    inline TagWriter ByteArray(byte *value, int length) {
        return {
                [](Stream &stream) {
                    stream.write((byte) 7);
                },
                [length, value](Stream &stream) {
                    stream.write(((byte *) &length)[1]);
                    stream.write(((byte *) &length)[0]);
                    stream.write(value, length);
                }
        };
    }

    inline NamedTagWriter ByteArray(const class String name, byte *value, int length) {
        return {
                [](Stream &stream) {
                    stream.write((byte) 7);
                },
                [name](Stream &stream) {
                    unsigned short length = name.length();
                    stream.write(((byte *) &length)[1]);
                    stream.write(((byte *) &length)[0]);
                    stream.write(name.begin(), length);
                },
                [length, value](Stream &stream) {
                    stream.write(((byte *) &length)[1]);
                    stream.write(((byte *) &length)[0]);
                    stream.write(value, length);
                }
        };
    }

    inline NamedTagWriter String(class String value) {
        return {
                [](Stream &stream) {
                    stream.write((byte) 8);
                },
                [value](Stream &stream) {
                    unsigned int length = value.length();
                    stream.write(((byte *) &length)[1]);
                    stream.write(((byte *) &length)[0]);
                    stream.write(value.begin(), length);
                }
        };
    }

    inline NamedTagWriter String(const class String name, class String value) {
        return {
                [](Stream &stream) {
                    stream.write((byte) 8);
                },
                [name](Stream &stream) {
                    unsigned short length = name.length();
                    stream.write(((byte *) &length)[1]);
                    stream.write(((byte *) &length)[0]);
                    stream.write(name.begin(), length);
                },
                [value](Stream &stream) {
                    unsigned short length = value.length();
                    stream.write(((byte *) &length)[1]);
                    stream.write(((byte *) &length)[0]);
                    stream.write(value.begin(), length);
                }
        };
    }

    inline TagWriter List(std::initializer_list<TagWriter> content) {
        return {
                [=](Stream &stream) {
                    stream.write((byte) 9);
                },
                [=](Stream &stream) {
                    if (content.size() > 0) {
                        content.begin()[0].type(stream);
                    } else {
                        stream.write((byte) 0);
                    }
                    int length = (int) content.size();
                    stream.write(((byte *) &length)[3]);
                    stream.write(((byte *) &length)[2]);
                    stream.write(((byte *) &length)[1]);
                    stream.write(((byte *) &length)[0]);
                    for (unsigned int i = 0; i < content.size(); ++i) {
                        content.begin()[i].value(stream);
                    }
                }
        };
    }

    inline NamedTagWriter List(const class String name, std::initializer_list<TagWriter> content) {
        return {
                [=](Stream &stream) {
                    stream.write((byte) 9);
                },
                [name](Stream &stream) {
                    unsigned short length = name.length();
                    stream.write(((byte *) &length)[1]);
                    stream.write(((byte *) &length)[0]);
                    stream.write(name.begin(), length);
                },
                [=](Stream &stream) {
                    if (content.size() > 0) {
                        content.begin()[0].type(stream);
                    } else {
                        stream.write((byte) 0);
                    }
                    int length = (int) content.size();
                    stream.write(((byte *) &length)[3]);
                    stream.write(((byte *) &length)[2]);
                    stream.write(((byte *) &length)[1]);
                    stream.write(((byte *) &length)[0]);
                    for (unsigned int i = 0; i < content.size(); ++i) {
                        content.begin()[i].value(stream);
                    }
                }
        };
    }

    inline TagWriter Compound(std::initializer_list<NamedTagWriter> content) {
        return {
                [=](Stream &stream) {
                    stream.write((byte) 10);
                },
                [=](Stream &stream) {
                    for (unsigned int i = 0; i < content.size(); ++i) {
                        content.begin()[i].type(stream);
                        content.begin()[i].name(stream);
                        content.begin()[i].value(stream);
                    }
                    stream.write((byte) 0); // END OF COMPOUND
                }
        };
    }

    inline NamedTagWriter Compound(const class String name, std::initializer_list<NamedTagWriter> content) {
        return {
                [=](Stream &stream) {
                    stream.write((byte) 10);
                },
                [name](Stream &stream) {
                    unsigned short length = name.length();
                    stream.write(((byte *) &length)[1]);
                    stream.write(((byte *) &length)[0]);
                    stream.write(name.begin(), length);
                },
                [=](Stream &stream) {
                    for (unsigned int i = 0; i < content.size(); ++i) {
                        content.begin()[i].type(stream);
                        content.begin()[i].name(stream);
                        content.begin()[i].value(stream);
                    }
                    stream.write((byte) 0); // END OF COMPOUND
                }
        };
    }
}
