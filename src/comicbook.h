#ifndef COMICBOOK_H
#define COMICBOOK_H

#include <cstddef>
#include <map>
#include <string>
#include <vector>

typedef unsigned char byte_t;

class ComicBook {
  public:
    ComicBook();
    void append_content(std::vector<byte_t> &data);
    const std::vector<byte_t> *get_content(void) const;
    size_t size() const;
    bool empty() const;

  private:
    std::vector<std::vector<byte_t>> content;
};

#endif // COMICBOOK_H
