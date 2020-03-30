#pragma once

#include <string>

static const char col_types[] = {'I', 'B', 'D', 'S'};

#define EXPECT_SCHEMA_EQ(a, b) EXPECT_PRED2(isSchemaEq, a, b);
#define ASSERT_SCHEMA_EQ(a, b) ASSERT_PRED2(isSchemaEq, a, b);

[[maybe_unused]] static bool isSchemaEq(const Schema& a, const Schema& b) {
    if (a.width() != b.width()) return false;
    if (a.length() != b.length()) return false;

    for (size_t i = 0; i < a.width(); i++) {
        if (a.col_name(i) != b.col_name(i)) return false;
        if (a.col_type(i) != b.col_type(i)) return false;
    }

    for (size_t i = 0; i < a.length(); i++) {
        if (a.row_name(i) != b.row_name(i)) return false;
    }

    return true;
}

// same as above but for pointers
[[maybe_unused]] static bool isSchemaEq(const Schema* a, const Schema* b) {
    if (a != b) return false;
    if (a != nullptr) return isSchemaEq(*a, *b);

    return true;
}
