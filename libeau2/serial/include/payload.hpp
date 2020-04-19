/**
 * @file payload.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#pragma once

#include <cstdint>
#include <memory>
#include <vector>

#include "commondefs.hpp"
#include "serial.hpp"

class Serializer;

// Serialized data to include with messages
class Payload {
   private:
    Serial::Type _type = Serial::Type::Unknown;
    Serial::Type _colType = Serial::Type::Unknown;
    std::vector<uint8_t> _data;
    std::shared_ptr<void> _ref = nullptr;

    void _setupThisPayload(Serializer& ss, uint64_t remaining);
    void _serializeColumn(Serializer& ss);
    void _serializeDataFrame(Serializer& ss);
    BStreamIter _deserializeColumn(uint64_t& payloadsLeft, BStreamIter start,
                                   BStreamIter end);
    BStreamIter _deserializeDataFrame(uint64_t& payloadsLeft, BStreamIter start,
                                      BStreamIter end);
    template <typename T>
    void _unpackAsCol(Payload& colData);

    friend class Serializer;

   public:
    Payload();

    template <typename T>
    Payload(T value);

    Serial::Type type() const;

    template <typename T>
    bool add(T value);

    template <typename T>
    bool add(ColPtr<T> value);

    bool add(DFPtr value);

    void serialize(Serializer& ss);

    BStreamIter deserialize(BStreamIter start, BStreamIter end);

    template <typename T>
    ColPtr<T> asColumn();

    Key asKey();

    DFPtr asDataFrame();
};

#include "payload.tpp"