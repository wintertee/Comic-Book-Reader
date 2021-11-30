#ifndef COMICBOOK_H
#define COMICBOOK_H

#include <map>
#include <string>
#include <vector>
#include <cstddef>

typedef unsigned char byte_t;

class ComicBook
{
public:
    ComicBook();
    std::map< std::wstring, std::vector< byte_t > > map;
};

#endif // COMICBOOK_H
