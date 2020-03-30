// lang::CwC
#pragma once

#include "message.hpp"
#include "payload.hpp"

class Reply : public Message {
   public:
    SerialType type_;
    size_t payloads_;
    Payload** payloads_data_;

    Reply(size_t sender, size_t target, size_t id)
        : Message(MsgKind::Reply, sender, target, id), payloads_(0) {}
    ~Reply() {
        if (payloads_) {
            for (size_t ii = 0; ii < payloads_; ii++) {
                delete payloads_data_[ii];
            }

            delete[] payloads_data_;
        }
    }

    void setPayload(DoubleColumn* col) {
        if (payloads_) {
            printf("Payload already set\n");
            return;
        }

        type_ = SerialType::Double;
        payloads_ = col->size();
        payloads_data_ = new Payload*[payloads_];
        for (size_t ii = 0; ii < payloads_; ii++) {
            Payload* payload = new Payload(col->get(ii));
        }
    }

    void setPayload(StringColumn* col) {
        if (payloads_) {
            printf("Payload already set\n");
            return;
        }

        type_ = SerialType::String;
        payloads_ = col->size();
        payloads_data_ = new Payload*[payloads_];
        for (size_t ii = 0; ii < payloads_; ii++) {
            Payload* payload = new Payload(col->get(ii));
        }
    }

    SerialData* serialize() {
        if (!payloads_) {
            printf("Empty Reply\n");
            return nullptr;
        }
        Serializer s;
        setupCmdHdr(s);
        s.serialize(SerialData::serialTypeToValue(type_)).serialize(payloads_);
        for (size_t ii = 0; ii < payloads_; ii++) {
            s.serialize(*payloads_data_[ii]);
        }

        return s.generate();
    }
};