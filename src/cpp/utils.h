#ifndef _UTILS
#define _UTILS
#include <string>
template <typename OutIt> OutIt split(const std::string &text, char sep, OutIt out){
    size_t start = 0, end=0;
    while((end = text.find(sep, start)) != std::string::npos)
    {
        *out++ = text.substr(start, end - start);
        start = end + 1;
    }
    *out++ = text.substr(start);
    return out;
}
#endif
