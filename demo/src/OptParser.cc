#include <cstring>

#include "OptParser.hh"

OptParser::OptParser() {}

void OptParser::addFlag(const char* flag, int opts) {
    flagOpts[std::string(flag)] = opts;
}

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
Opt OptParser::getOpt(int argc, char** argv) {
    std::vector<std::string> optVals;
    Opt opt = std::make_pair("", optVals);

    if (optIdx < argc) {
        const char* const arg = argv[optIdx];
        if (isOpt(argv[optIdx])) {
            std::string flag = arg + 1;
            if (flagOpts.count(flag) && optIdx + flagOpts[flag] < argc) {
                opt.first = flag;
                for (int ii = 0; ii < flagOpts[flag]; ii++) {
                    optVals.push_back(std::string(argv[++optIdx]));
                }
            } else {
                opt.first = "?";
            }
        } else {
            // Non-option program arguments
            opt.first = "non-flag";
            optVals.push_back(argv[optIdx]);
        }

        opt.second = optVals;
        optIdx++;
    }

    return opt;
}

bool OptParser::isOpt(const char* value) {
    return strlen(value) >= 2 && value[0] == '-' && isalpha(value[1]);
}