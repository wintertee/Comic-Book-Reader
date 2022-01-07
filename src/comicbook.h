#ifndef COMICBOOK_H
#define COMICBOOK_H

#include <vector>

typedef unsigned char byte_t;

class ComicBook {
  public:
    ComicBook();

    /// append data to content
    void append_content(std::vector<byte_t> &data);

    /// return content.data()
    const std::vector<byte_t> *get_content(void) const;

    /// return content.size()
    size_t size() const;

    /// return content.empty()
    bool empty() const;

  private:
    /// elements are images
    std::vector<std::vector<byte_t>> content;
};

#endif // COMICBOOK_H
