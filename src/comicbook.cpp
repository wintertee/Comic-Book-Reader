#include "comicbook.h"

ComicBook::ComicBook() {}

void ComicBook::append_content(std::vector<byte_t> &data) { content.emplace_back(data); }

const std::vector<byte_t> *ComicBook::get_content() const { return content.data(); }

size_t ComicBook::size() const { return content.size(); }

bool ComicBook::empty() const { return content.empty(); }
