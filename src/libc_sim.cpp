#include "libc_sim.h"

#include <cassert>
#include <cstdio>
#include <cstring>
#include <string>

#include "container_utils.h"

namespace ravel {

void libc::puts(std::array<std::int32_t, 32> &regs,
                std::vector<std::byte> &storage) {
  std::size_t pos = regs[10];
  regs[10] = std::puts((const char *)(storage.data() + pos));
}

void libc::scanf(std::array<std::int32_t, 32> &regs,
                 std::vector<std::byte> &storage) {
  // TODO
  auto fmtStr = (const char *)(storage.data() + regs[10]);
  std::size_t nPercentSign = 0;
  for (auto p = fmtStr; *p != 0; ++p)
    nPercentSign += *p == '%';
  switch (nPercentSign) {
  case 0:
    regs[10] = std::scanf(fmtStr);
    return;
  case 1:
    regs[10] = std::scanf(fmtStr, storage.data() + regs[11]);
    return;
  case 2:
    regs[10] = std::scanf(fmtStr, storage.data() + regs[11],
                          storage.data() + regs[12]);
    return;
  default:
    assert(false);
  }
}

void libc::printf(std::array<std::int32_t, 32> &regs,
                  const std::vector<std::byte> &storage) {
  auto fmtStr = std::string((const char *)(storage.data() + regs[10]));
  std::size_t nSuccess = 0;
  std::size_t curArg = 0;
  for (std::size_t i = 0; i < fmtStr.size(); ++i) {
    if (fmtStr[i] == '\\') {
      ++i;
      assert(i != fmtStr.size());
      assert(fmtStr[i] == 'n'); // TODO
      std::putchar('\n');
      continue;
    }
    if (fmtStr[i] == '%') {
      ++i;
      assert(i != fmtStr.size());
      assert(fmtStr[i] == 'd'); // TODO
      std::printf("%d", regs[11 + curArg]);
      ++curArg;
      ++nSuccess;
      continue;
    }
    std::putchar(fmtStr[i]);
  }
  regs[10] = nSuccess;
}

void libc::putchar(std::array<std::int32_t, 32> &regs) {
  regs[10] = std::putchar(regs[10]);
}

void libc::malloc(std::array<std::int32_t, 32> &regs,
                  const std::vector<std::byte> &storage, std::size_t &heapPtr,
                  std::unordered_set<std::size_t> &malloced) {
  auto size = (std::size_t)regs[10];
  regs[10] = heapPtr;
  malloced.emplace(heapPtr);
  heapPtr += size;
  assert(heapPtr < storage.size());
}

void libc::free(const std::array<std::int32_t, 32> &regs,
                std::unordered_set<std::size_t> &malloced) {
  std::size_t addr = regs[10];
  assert(isIn(malloced, addr));
  malloced.erase(malloced.find(addr));
}

void libc::memcpy(std::array<std::int32_t, 32> &regs,
                  std::vector<std::byte> &storage) {
  std::size_t dest = regs[10];
  std::size_t src = regs[11];
  std::size_t cnt = regs[12];
  assert(src + cnt < storage.size() && dest + cnt < storage.size());
  std::memcpy(storage.data() + dest, storage.data() + src, cnt);
}

} // namespace ravel