/**
 * @file kvnet.hpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 * 
 * Lang::Cpp
 */


#pragma once

#include <memory>

#include "message.hpp"

// Interface for facilitating communication between distributed stores
class KVNet {
   public:
    /**
     * @brief Register a node at the given port.
     * 
     * @param port the port
     * @return size_t the index of the node registered
     */
    virtual size_t registerNode(const char* port) = 0;

    /**
     * @brief Send a message, using the message's target to route it.
     * 
     * @param msg the message to send
     */
    virtual void send(std::shared_ptr<Message> msg) = 0;

    /**
     * @brief Receive a message on this node from the queue.
     * 
     * @return std::unique_ptr<Message> the message
     */
    virtual std::unique_ptr<Message> receive() = 0;
};