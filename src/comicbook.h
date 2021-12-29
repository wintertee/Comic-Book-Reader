#ifndef COMICBOOK_H
#define COMICBOOK_H

#include <cstddef>
#include <map>
#include <string>
#include <vector>

typedef unsigned char byte_t;

class ComicBook
{
public:
  ComicBook();
  std::map<std::wstring, std::vector<byte_t>> map;
  std::map<std::wstring, std::vector<byte_t>>::iterator it;
};

#endif // COMICBOOK_H
