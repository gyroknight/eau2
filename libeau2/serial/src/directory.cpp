/**
 * @file directory.cpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#include "directory.hpp"

#include <cstdint>
#include <cstring>

#include "serializer.hpp"

Directory::Directory(uint64_t sender, uint64_t target, uint64_t idx)
    : Message(MsgKind::Directory, sender, target, 0), _idx(idx) {}

void Directory::addNode(in_addr_t address, in_port_t port) {
    struct sockaddr_in node;
    memset(&node, 0, sizeof(node));

    node.sin_addr.s_addr = address;
    node.sin_port = port;

    _dir.push_back(node);
}

uint64_t Directory::idx() { return _idx; }

std::vector<struct sockaddr_in>& Directory::dir() { return _dir; }

/**
 * @brief Directory is represented in the following serial format:
 *
 * Command Header - see Message
 * idx - 8 bytes
 * nodes_count - 8 bytes
 * nodes - nodes_count x
 *  address - 4 bytes
 *  port number - 2 bytes
 *
 * @return std::unique_ptr<std::vector<uint8_t>>
 */
std::unique_ptr<std::vector<uint8_t>> Directory::serialize() {
    Serializer ss;
    setupCmdHdr(ss);

    ss.add(_idx).add(static_cast<uint64_t>(_dir.size()));

    for (struct sockaddr_in& node : _dir) {
        ss.add(static_cast<uint32_t>(node.sin_addr.s_addr))
            .add(static_cast<uint16_t>(node.sin_port));
    }

    return ss.generate();
}

std::unique_ptr<Message> Directory::deserializeAs(BStreamIter start,
                                                  BStreamIter end) {
    if (std::distance(start, end) < 2 * sizeof(uint64_t)) {
        std::cerr << "Directory data too small\n";
        return nullptr;
    }

    uint64_t idx = *reinterpret_cast<uint64_t*>(&(*start));
    start += sizeof(uint64_t);
    uint64_t nodes = *reinterpret_cast<uint64_t*>(&(*start));
    start += sizeof(uint64_t);

    if (std::distance(start, end) !=
        nodes * (sizeof(uint32_t) + sizeof(uint16_t))) {
        std::cerr << "Unexpected Directory size\n";
        return nullptr;
    }

    auto dirMsg = std::make_unique<Directory>(0, 0, idx);

    for (size_t ii = 0; ii < nodes; ii++) {
        in_addr_t address = *reinterpret_cast<uint32_t*>(&(*start));
        start += sizeof(uint32_t);
        in_port_t port = *reinterpret_cast<uint16_t*>(&(*start));
        start += sizeof(uint16_t);
        dirMsg->addNode(address, port);
    }

    return dirMsg;
}