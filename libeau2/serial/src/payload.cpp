/**
 * @file reply.cpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#include "payload.hpp"

#include <cstdint>
#include <cstring>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <string>

#include "column.hpp"
#include "dataframe.hpp"
#include "key.hpp"
#include "serial.hpp"
#include "serializer.hpp"

Payload::Payload() {}

Serial::Type Payload::type() const { return _type; }

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

bool Payload::add(DFPtr value) {
    if (_type != Serial::Type::Unknown) {
        std::cerr << "Payload is already set\n";
        return false;
    }

    _type = Serial::Type::DataFrame;
    _ref = value;

    return true;
}

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

BStreamIter Payload::deserialize(BStreamIter start, BStreamIter end) {
    if (std::distance(start, end) < Serial::PAYLOAD_HDR_SIZE)
        throw std::invalid_argument("Payload is too small");

    // Payload fields
    _type = Serial::valueToType(*start++);
    uint64_t payloadsLeft = *reinterpret_cast<uint64_t*>(&(*start));
    start += sizeof(uint64_t);
    uint64_t dataSize = *reinterpret_cast<uint64_t*>(&(*start));
    start += sizeof(uint64_t);

    // Get this Payload's data
    if (std::distance(start, end) != dataSize)
        throw std::invalid_argument("Invalid Payload data");
    _data = std::vector<uint8_t>(start, start + dataSize);
    start += dataSize;

    switch (_type) {
        case Serial::Type::Column:
            start = _deserializeColumn(payloadsLeft, start, end);
            break;
        case Serial::Type::DataFrame:
            start = _deserializeDataFrame(payloadsLeft, start, end);
            break;
        default:
            // No action needed, default deserialization is sufficient
            break;
    }

    if (payloadsLeft)
        std::cerr << payloadsLeft << " payloads left, expected none\n";

    return start;
}

Key Payload::asKey() {
    if (_type != Serial::Type::Key) {
        std::cerr << "Payload is not a Key\n";
        return Key("", 0);
    }

    if (_data.size() < sizeof(uint64_t) + sizeof(char) ||
        _data.back() != '\0') {
        std::cerr << "Key data is corrupted\n";
        return Key("", 0);
    }

    uint64_t home = *reinterpret_cast<uint64_t*>(_data.data());
    const char* name =
        reinterpret_cast<const char*>(_data.data() + sizeof(uint64_t));

    return Key(name, home);
}

DFPtr Payload::asDataFrame() {
    if (_type != Serial::Type::DataFrame) {
        std::cerr << "Payload is not a DataFrame\n";
        return nullptr;
    }

    return std::static_pointer_cast<DataFrame>(_ref);
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

BStreamIter Payload::_deserializeColumn(uint64_t& payloadsLeft,
                                        BStreamIter start, BStreamIter end) {
    if (payloadsLeft != 1) {
        std::cerr << "Single Payload Columns supported at this time only\n";
        return start;
    }

    Payload colData;

    start = colData.deserialize(start, end);

    _colType = colData.type();

    switch (_colType) {
        case Serial::Type::U8:
            _unpackAsCol<uint8_t>(colData);
            break;
        case Serial::Type::I8:
            _unpackAsCol<int8_t>(colData);
            break;
        case Serial::Type::U16:
            _unpackAsCol<uint16_t>(colData);
            break;
        case Serial::Type::I16:
            _unpackAsCol<int16_t>(colData);
            break;
        case Serial::Type::U32:
            _unpackAsCol<uint32_t>(colData);
            break;
        case Serial::Type::I32:
            _unpackAsCol<int32_t>(colData);
            break;
        case Serial::Type::U64:
            _unpackAsCol<uint64_t>(colData);
            break;
        case Serial::Type::I64:
            _unpackAsCol<int64_t>(colData);
            break;
        case Serial::Type::Float:
            _unpackAsCol<float>(colData);
            break;
        case Serial::Type::Double:
            _unpackAsCol<double>(colData);
            break;
        case Serial::Type::String:
            _unpackAsCol<ExtString>(colData);
            break;
        default:
            std::cerr << "Unsupported Column type\n";
    }

    return start;
}

BStreamIter Payload::_deserializeDataFrame(uint64_t& payloadsLeft,
                                           BStreamIter start, BStreamIter end) {
    auto df = std::make_shared<DataFrame>();

    while (payloadsLeft) {
        Payload col;
        start = col.deserialize(start, end);
        payloadsLeft--;

        switch (col._colType) {
            case Serial::Type::U8:
                df->addCol(std::static_pointer_cast<Column<uint8_t>>(col._ref));
                break;
            case Serial::Type::I8:
                df->addCol(std::static_pointer_cast<Column<int8_t>>(col._ref));
                break;
            case Serial::Type::U16:
                df->addCol(
                    std::static_pointer_cast<Column<uint16_t>>(col._ref));
                break;
            case Serial::Type::I16:
                df->addCol(std::static_pointer_cast<Column<int16_t>>(col._ref));
                break;
            case Serial::Type::U32:
                df->addCol(
                    std::static_pointer_cast<Column<uint32_t>>(col._ref));
                break;
            case Serial::Type::I32:
                df->addCol(std::static_pointer_cast<Column<int32_t>>(col._ref));
                break;
            case Serial::Type::U64:
                df->addCol(
                    std::static_pointer_cast<Column<uint64_t>>(col._ref));
                break;
            case Serial::Type::I64:
                df->addCol(std::static_pointer_cast<Column<int64_t>>(col._ref));
                break;
            case Serial::Type::Float:
                df->addCol(std::static_pointer_cast<Column<float>>(col._ref));
                break;
            case Serial::Type::Double:
                df->addCol(std::static_pointer_cast<Column<double>>(col._ref));
                break;
            case Serial::Type::String:
                df->addCol(
                    std::static_pointer_cast<Column<ExtString>>(col._ref));
                break;
            default:
                std::cerr << "Unexpected Payload, expected Column\n";
                return start;
        }
    }

    _ref = df;

    return start;
}

template <>
void Payload::_unpackAsCol<ExtString>(Payload& colData) {
    auto col = std::make_shared<Column<ExtString>>();

    auto string = std::make_shared<std::string>();

    for (uint8_t& cc : colData._data) {
        if (cc == '\0') {
            col->push_back(string);
            string = std::make_shared<std::string>();
        } else {
            string->push_back(cc);
        }
    }

    if (!string->empty())
        std::cerr << "Malformed data, last string not null terminated\n";

    _ref = col;
}