#pragma once;

#include <cstdint>

#include <memory>
#include <vector>
namespace codec {
using AudioData = std::shared_ptr<std::vector<uint8_t> >;
}
