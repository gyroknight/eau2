#include "directory.hpp"

Directory::Directory(uint64_t sender, uint64_t target, uint64_t idx)
    : Message(MsgKind::Directory, sender, target, 0) {}

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