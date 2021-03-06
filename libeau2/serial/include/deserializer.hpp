/**
 * @file deserializer.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 * 
 * Lang::Cpp
 */

#include "payload.h"
#include "reply.h"

// WIP

class Deserializer : public Object {
   public:
    static Reply* asReply(size_t sender, size_t target, size_t id,
                          uint8_t* bytestream) {}

    static Payload* asPayload(uint8_t* bytestream) {
        return new Payload(bytestream);
    }
};