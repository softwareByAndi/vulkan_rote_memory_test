#include <vector>
#include <string>
#include <algorithm>

namespace UF {
bool includes(std::vector<const char *> v, const char *cmp) {
  // Use std::find_if and strcmp to compare contents
  auto it = std::find_if(
    v.begin(),
    v.end(),
    [cmp](const char *s) { return std::strcmp(s, cmp) == 0; }
  );
  return (it != v.end());
}

/** use map like python */
template <typename A, typename Func>
auto map(const std::vector<A>& in, Func func) {
  using B = decltype(func(std::declval<A>())); // Figure out B using dark magic
  std::vector<B> out;
  out.reserve(in.size());
  for (const auto& item : in) {
    out.push_back(func(item));
  }
  return out;
}

/** If you want to be explicit */
template <typename A, typename B, typename Func>
std::vector<B> map(const std::vector<A>& in, Func func) {
  std::vector<B> out;
  out.reserve(in.size());
  for (const auto& item : in) {
    out.push_back(func(item));
  }
  return out;
}

}

