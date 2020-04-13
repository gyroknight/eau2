/**
 * @file reply.cpp
 * @author Vincent Zhao (zhao.v@northeastern.edu)
 * @author Michael Hebert (mike.s.hebert@gmail.com)
 * 
 * Lang::Cpp
 */

#pragma once

template <typename T>
inline void Reply::setPayload(T payload) {
    if (_payload) throw std::runtime_error("Payload already set");
    _payload = std::make_shared<Payload>(payload);
}