#pragma once
#include <vector>
#include <string>
#include <algorithm>

namespace UF {
inline bool includes(const std::vector<const char *> &v, const char *cmp) {
  // Use std::find_if and strcmp to compare contents
  const auto it = std::ranges::find_if(
    v,
    [cmp](const char *s) { return std::strcmp(s, cmp) == 0; }
  );
  return (it != v.end());
}

inline bool includes(const std::vector<VkExtensionProperties> &v, const char *cmp) {
  // Use std::find_if and strcmp to compare contents
  const auto it = std::ranges::find_if(
    v,
    [cmp](const VkExtensionProperties &ext) {
      return std::strcmp(ext.extensionName, cmp) == 0;
    }
  );
  return (it != v.end());
}

inline bool includes(const std::vector<VkLayerProperties> &v, const char *cmp) {
  // Use std::find_if and strcmp to compare contents
  const auto it = std::ranges::find_if(
    v,
    [cmp](const VkLayerProperties &lay) {
      return std::strcmp(lay.layerName, cmp) == 0;
    }
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

