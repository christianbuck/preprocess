#include "preprocess/parallel.hh"
#include "util/murmur_hash.hh"
#include "util/probing_hash_table.hh"
#include "util/scoped.hh"

#include <boost/lexical_cast.hpp>

#include <iostream>

#include <stdint.h>

struct Entry {
  typedef uint64_t Key;
  uint64_t key;
  uint64_t GetKey() const { return key; }
  void SetKey(uint64_t to) { key = to; }
};

class Dedupe {
  public:
    bool operator()(const StringPiece &line) {
      Entry entry;
      entry.key = util::MurmurHashNative(line.data(), line.size()) + 1;
      Table::MutableIterator it;
      return !table_.FindOrInsert(entry, it);
    }

  private:
    typedef util::AutoProbing<Entry, util::IdentityHash> Table;
    Table table_;
};

int main(int argc, char *argv[]) {
  Dedupe dedupe;
  uint64_t input, output;
  int res = FilterCountParallel(dedupe, argc, argv, &input, &output);
  if (res == 0) {
    std::cout << output << "\t" << input << std::endl;
  }
  return res;
}
