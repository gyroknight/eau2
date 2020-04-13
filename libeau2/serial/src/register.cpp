#include "register.hpp"

Register::Register(in_addr_t address, in_port_t port)
    : Message(MsgKind::Register, 0, 0, Register::getNextID()) {
    _context.sin_family = AF_INET;
    _context.sin_addr.s_addr = address;
    _context.sin_port = port;
}

std::unique_ptr<std::vector<uint8_t>> Register::serialize() {
    Serializer ss;
    setupCmdHdr(ss);
    ss.add(static_cast<uint32_t>(_context.sin_addr.s_addr))
        .add(static_cast<uint16_t>(_context.sin_port));

    return ss.generate();
}