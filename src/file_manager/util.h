#ifndef UTIL_H_INCLUDED
#define UTIL_H_INCLUDED

#include <cstring>
#include <sstream>

std::vector<std::string> split(std::string str, const char* delimiter) {
    std::vector<std::string> internal;
    std::stringstream ss(str);
    std::string tok;

    while(getline(ss, tok, delimiter[0])) {
        internal.push_back(tok);
    }

    return internal;
}

std::string getHost(const std::string& host) {
    return split(host, ":")[0];
}

unsigned short getPort(const std::string& host) {
    return (unsigned short)strtoul(split(host, ":")[1].c_str(), nullptr, 10);
}



#endif // UTIL_H_INCLUDED
