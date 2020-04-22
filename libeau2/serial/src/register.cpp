/**
 * @file register.cpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 *
 * Lang::Cpp
 */

#include "register.hpp"

#include <sys/socket.h>

#include "serializer.hpp"

Register::Register(in_addr_t address, in_port_t port)
    : Message(MsgKind::Register, 0, 0, Register::getNextID()) {
    _context.sin_family = AF_INET;
    _context.sin_addr.s_addr = address;
    _context.sin_port = port;
}

struct sockaddr_in Register::context() {
    return _context;
}

/**
 * @brief Register is represented as the following serial format:
 *
 * Command Header - see Message
 * IP address - 4 bytes
 * Port number - 2 bytes
 *
 * @return std::unique_ptr<std::vector<uint8_t>>
 */
std::unique_ptr<std::vector<uint8_t>> Register::serialize() {
    Serializer ss;
    setupCmdHdr(ss);
    ss.add(static_cast<uint32_t>(_context.sin_addr.s_addr))
        .add(static_cast<uint16_t>(_context.sin_port));

    return ss.generate();
}

std::unique_ptr<Message> Register::deserializeAs(BStreamIter start,
                                                 BStreamIter end) {
    if (std::distance(start, end) < sizeof(uint32_t) + sizeof(uint16_t)) {
        std::cerr << "Register data is too small\n";
        return nullptr;
    }

    in_addr_t address = *reinterpret_cast<uint32_t*>(&(*start));
    start += sizeof(uint32_t);
    in_port_t port = *reinterpret_cast<uint16_t*>(&(*start));

    return std::make_unique<Register>(address, port);
}