#pragma once

#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

using Opt = std::pair<std::string, std::vector<std::string>>;

/**
 * @brief Parses program arguments and options.
 *
 * An option is defined as "-[]" where [] is an alphabetic character.
 *
 * Inspired by the behavior of getopt() from the UNIX Standard Library.
 */
class OptParser {
   public:
    OptParser();

    void addFlag(const char* flag, int opts);

    /**
     * @brief Gets the next option from the argument list.
     *
     * This will set or clear optArg as needed.
     *
     * @param argc Number of arguments provided.
     * @param argv List of program arguments
     * @return int Either the option flag, 1 for non-options, '?' for unknown
     * options, or -1 for end of options.
     */
    Opt getOpt(int argc, char** argv);

   private:
    //! The next option to process
    int optIdx = 1;
    std::unordered_map<std::string, int> flagOpts;

    bool isOpt(const char* value);
};
