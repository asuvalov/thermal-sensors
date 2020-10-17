#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <regex>
#include <vector>
#include <algorithm>
#include <optional>

namespace fs = std::filesystem;

template <class T>
T read_val(const fs::path& p) {
  std::ifstream stream(p);
  T val;
  stream >> val;
  return val;
}

template <class T>
std::optional<T> read_param(const fs::directory_entry& dir, const std::string& name) {
  auto path = dir.path() / name;
  if (!fs::exists(path))
    return {};
  return read_val<T>(path);
}

struct thermal_zone {
  thermal_zone(int64_t num, const fs::directory_entry& dir)
    : number(num) {
    temp = read_param<int64_t>(dir, "temp").value();
    type = read_param<std::string>(dir, "type").value();
  }
  friend bool operator<(const thermal_zone& lhs, const thermal_zone& rhs) {
    return lhs.number < rhs.number;
  }
  friend std::ostream& operator<<(std::ostream& os, const thermal_zone& zone) {
    os.precision(1);
    os << std::left << std::setw(3) << zone.number
       << std::left << std::setw(15) << zone.type
       << std::left << std::setw(8) << std::fixed << zone.temp/1000.0;
    return os;
  }
  static void print_title(std::ostream& os) {
    os << std::left << std::setw(3) << "N"
       << std::left << std::setw(15) << "Type"
       << std::left << std::setw(5) << "Temp[C]"
       << std::endl;
  }
  int number;
  std::string type;
  int64_t temp;
};

int main() {
  std::smatch base_match;
  std::vector<thermal_zone> zones;
  for (auto& p: fs::directory_iterator("/sys/devices/virtual/thermal")) {
    std::string filename = p.path().filename().string();
    const std::regex base_regex("thermal_zone([0-9]+)");
    if (std::regex_match(filename, base_match, base_regex) && base_match.size() == 2) {
      zones.emplace_back(std::stoll(base_match[1]), p);
    }
  }
  std::sort(zones.begin(), zones.end());
  thermal_zone::print_title(std::cout);
  std::copy(zones.begin(), zones.end(),
            std::ostream_iterator<thermal_zone>(std::cout, "\n"));
  return 0;
}

