#pragma once
// lang::Cpp
#include <cstdlib>
#include <iostream>
/** Helper class providing some C++ functionality and convenience
 *  functions. This class has no data, constructors, destructors or
 *  virtual functions. Inheriting from it is zero cost.
 */
class Sys {
   public:
    /* Printing functions: the reason for returning this is to allow chaining:
     *  p("hi").p(" ").p("there"); */
    Sys& p(char* c) {
        std::cout << c;
        return *this;
    }
    Sys& p(int i) {
        std::cout << i;
        return *this;
    }
    Sys& p(size_t i) {
        std::cout << i;
        return *this;
    }
    Sys& p(const char* c) {
        std::cout << c;
        return *this;
    }
    Sys& pln() {
        std::cout << "\n";
        return *this;
    }
    Sys& pln(int i) {
        std::cout << i << "\n";
        return *this;
    }
    Sys& pln(char* c) {
        std::cout << c << "\n";
        return *this;
    }
    Sys& pln(const char* c) {
        std::cout << c << "\n";
        return *this;
    }
    // Copying strings
    char* duplicate(const char* s) {
        char* res = new char[strlen(s) + 1];
        strcpy(res, s);
        return res;
    }
    char* duplicate(char* s) {
        char* res = new char[strlen(s) + 1];
        strcpy(res, s);
        return res;
    }
    // Function to terminate execution with a message
    void exit_if_not(bool b, char* c) {
        if (b) return;
        p("Exit message: ").pln(c);
        exit(-1);
    }
    // Definitely fail
    void FAIL() {
        pln("Failing");
        exit(1);
    }
    // Some utilities for lightweight testing
    void OK(const char* m) { pln(m); }
    void t_true(bool p) {
        if (!p) FAIL();
    }
    void t_false(bool p) {
        if (p) FAIL();
    }
};