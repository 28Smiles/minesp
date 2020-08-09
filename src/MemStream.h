#include <Arduino.h>

class MemStream : public Stream {
private:
    byte * _buffer;
    const unsigned short _len;
    bool _buffer_overflow;
    unsigned short _pos_read;
    unsigned short _pos_write;
    bool _allowWrite;
public:
    // public methods
    MemStream(byte *buffer, const unsigned short len, unsigned short content_len = 0, bool allowWrite=true)
            :_buffer(buffer),_len(len), _buffer_overflow(false), _pos_read(0), _pos_write(content_len), _allowWrite(allowWrite)
    {
        if (content_len==0) memset(_buffer, 0, _len);
        if (content_len>len) {
            content_len=len;
            _pos_write=len;
        }
    }

    ~MemStream() {}

    operator const byte *() const { return _buffer; }
    operator const char *() const { return (const char*)_buffer; }

    const unsigned short current_length() const { return _pos_write; }

    bool listen() { return true; }
    void end() {}
    bool isListening() { return true; }
    bool overflow() { bool ret = _buffer_overflow; _buffer_overflow = false; return ret; }
    int peek() {
        if (_pos_read >= _len) {
            _buffer_overflow = true;
            return -1;
        }
        if (_pos_read >= _pos_write) {
            return -1;
        }
        return _buffer[_pos_read+1];
    }

    virtual size_t write(byte b) {
        if (! _allowWrite) return -1;
        if (_pos_write >= _len) {
            _buffer_overflow = true;
            return 0;
        }
        _buffer[_pos_write] = b;
        ++_pos_write;
        return 1;
    }
    virtual int read() {
        if (_pos_read >= _len) {
            _buffer_overflow = true;
            return -1;
        }
        if (_pos_read >= _pos_write) {
            return -1;
        }
        return _buffer[_pos_read++];
    }
    virtual int available() {
        int ret=_pos_write-_pos_read;
        if (ret<0) ret=0;
        return ret;
    }
    virtual void flush() {
        memset(_buffer, 0, _len);
        _pos_write = 0;
        _pos_read = 0;
    }

    void setBufferContent(byte *buffer, unsigned short content_len) {
        memset(_buffer, 0, _len);
        memcpy(_buffer, buffer, content_len);
        _buffer_overflow=false;
        _pos_write=content_len;
        _pos_read=0;
    }
    void setBufferContentFromProgmem(byte *buffer, unsigned short content_len) {
        memset(_buffer, 0, _len);
        memcpy_P(_buffer, buffer, content_len);
        _buffer_overflow=false;
        _pos_write=content_len;
        _pos_read=0;
    }
    void setBufferContentPosition(unsigned short read_pos, unsigned short write_pos) {
        _pos_write=write_pos;
        _pos_read=read_pos;
    }

    using Print::write;
};
