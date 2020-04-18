/**
 * @file reply.cpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#include "payload.hpp"

#include <cstring>

#include "dataframe.hpp"

/**
 * @brief Construct a new Payload:: Payload object from a bytestream
 *
 * @param start
 * @param end
 */
Payload::Payload(std::vector<uint8_t>::iterator start,
                 std::vector<uint8_t>::iterator end) {
    if (std::distance(start, end) < Serial::PAYLOAD_HDR_SIZE)
        throw std::invalid_argument("Payload is too small");
    _type = Serial::valueToType(*start++);
    uint64_t dataSize = *reinterpret_cast<uint64_t*>(&(*start));
    start += sizeof(uint64_t);
    if (std::distance(start, end) != dataSize)
        throw std::invalid_argument("Invalid Payload header");
    _data = std::vector<uint8_t>(start, end);
}

Payload::Payload() {}

Serial::Type Payload::type() const { return _type; }

void Payload::serialize(Serializer& ss) {
    switch (_type) {
        case Serial::Type::Unknown:
            throw std::runtime_error("Invalid Payload to serialize");
        case Serial::Type::Column:
            _serializeColumn(ss);
            break;
        case Serial::Type::DataFrame:
            _serializeDataFrame(ss);
            break;
        default:
            _setupThisPayload(ss, 0);
    }
}

template <>
bool Payload::add(ColIPtr value) {
    if (_type != Serial::Type::Unknown) {
        std::cerr << "Payload is already set\n";
        return false;
    }

    if (!Serial::canSerialize(value)) {
        std::cerr << "Unsupported Column type\n";
        return false;
    }

    _type = Serial::Type::Column;
    _ref = value;

    return true;
}

template <>
bool Payload::add(const Key& value) {
    if (_type != Serial::Type::Unknown) {
        std::cerr << "Payload is already set\n";
        return false;
    }

    _type = Serial::Type::Key;

    // home + name + null terminator
    _data.resize(sizeof(uint64_t) + value.name().size() + 1);
    uint64_t home = value.home();
    memcpy(_data.data(), &home, sizeof(uint64_t));
    memcpy(_data.data() + sizeof(uint64_t), value.name().c_str(),
           value.name().size());
    _data.shrink_to_fit();

    return true;
}

template <>
bool Payload::add(DFPtr value) {
    if (_type != Serial::Type::Unknown) {
        std::cerr << "Payload is already set\n";
        return false;
    }

    _type = Serial::Type::DataFrame;
    _ref = value;

    return true;
}

void Payload::_setupThisPayload(Serializer& ss, uint64_t remaining) {
    ss.add(Serial::typeToValue(_type))
        .add(remaining)
        .add(static_cast<uint64_t>(_data.size()));
    for (uint8_t& byte : _data) ss.add(byte);
}

void Payload::_serializeColumn(Serializer& ss) {
    _setupThisPayload(ss, 1);
    auto col = std::static_pointer_cast<ColumnInterface>(_ref);
    if (!col) throw std::runtime_error("Invalid Column reference");
    col->serialize(ss);
}

void Payload::_serializeDataFrame(Serializer& ss) {
    DFPtr df = std::static_pointer_cast<DataFrame>(_ref);
    if (!df) throw std::runtime_error("Invalid DataFrame reference");
    _setupThisPayload(ss, df->ncols());
    for (size_t ii = 0; ii < df->ncols(); ii++) {
        Payload col(df->_data[ii]);
        col.serialize(ss);
    }
}