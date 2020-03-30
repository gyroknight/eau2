#include "payload.h"
#include "reply.h"

class Deserializer : public Object {
   public:
    static Reply* asReply(size_t sender, size_t target, size_t id,
                          uint8_t* bytestream) {}

    static Payload* asPayload(uint8_t* bytestream) {
        return new Payload(bytestream);
    }
};