// common/csv.hpp — 简洁 CSV 写入
#pragma once
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

namespace mlab {

class CsvWriter {
public:
    CsvWriter(const std::string& path,
              const std::vector<std::string>& header,
              bool append = false) {
        std::filesystem::create_directories(
            std::filesystem::path(path).parent_path());
        bool need_header = !append || !std::filesystem::exists(path);
        f_.open(path, append ? std::ios::app : std::ios::out);
        if (need_header) {
            for (std::size_t i = 0; i < header.size(); ++i) {
                if (i) f_ << ',';
                f_ << header[i];
            }
            f_ << '\n';
        }
    }

    template <class... Args>
    void row(Args&&... a) {
        write_cells({to_str(std::forward<Args>(a))...});
    }

    void write_cells(const std::vector<std::string>& cells) {
        for (std::size_t i = 0; i < cells.size(); ++i) {
            if (i) f_ << ',';
            // 若含逗号/引号/换行，用双引号包裹
            if (cells[i].find_first_of(",\"\n") != std::string::npos) {
                f_ << '"';
                for (char c : cells[i]) { if (c == '"') f_ << '"'; f_ << c; }
                f_ << '"';
            } else {
                f_ << cells[i];
            }
        }
        f_ << '\n';
        f_.flush();
    }

private:
    template <class T>
    static std::string to_str(T&& v) {
        if constexpr (std::is_same_v<std::decay_t<T>, std::string>)
            return std::forward<T>(v);
        else if constexpr (std::is_same_v<std::decay_t<T>, const char*>)
            return std::string(v);
        else
            return std::to_string(v);
    }

    std::ofstream f_;
};

}  // namespace mlab
