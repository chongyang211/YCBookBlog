// box_fs — REPL: mount/ls/write/cat/rm/open/close/stat
// 文档：§16 盒 ⑧
#include "fs.hpp"
#include "repl.hpp"
#include "log.hpp"
#include "fmt_shim.hpp"
#include <fstream>
#include <memory>
#include <string>

using namespace mlab::fs;
static std::unique_ptr<FS> g_fs;

static void cmd_mount(const std::vector<std::string>& a) {
    if (a.empty()) { LOG_ERR("usage: mount <image>\n"); return; }
    g_fs = std::make_unique<FS>(a[0]);
    LOG_OK("mounted {}\n", a[0]);
}
static void cmd_ls(const std::vector<std::string>&) {
    if (!g_fs) { LOG_ERR("mount first\n"); return; }
    for (auto& d : g_fs->list_root()) {
        auto ino = g_fs->read_inode(d.inum);
        fmt::print("  {:<20} inum={:3d} size={}B{}\n",
                   d.name, d.inum, ino.size,
                   g_fs->is_opened(d.inum) ? " [OPEN]" : "");
    }
}
static void cmd_write(const std::vector<std::string>& a) {
    if (a.size() < 2) { LOG_ERR("usage: write <name> <size|@file>\n"); return; }
    std::vector<uint8_t> data;
    if (a[1][0] == '@') {
        std::ifstream f(a[1].substr(1), std::ios::binary);
        data.assign(std::istreambuf_iterator<char>(f), {});
    } else {
        data.assign(std::atoi(a[1].c_str()), 'x');
    }
    uint32_t inum = -1;
    try { inum = g_fs->create(a[0]); } catch (...) {
        for (auto& d : g_fs->list_root()) if (d.name == a[0]) { inum = d.inum; break; }
    }
    g_fs->write_all(inum, data);
    LOG_OK("wrote {}B to {} (inum={})\n", data.size(), a[0], inum);
}
static void cmd_cat(const std::vector<std::string>& a) {
    if (a.empty()) { LOG_ERR("usage: cat <name>\n"); return; }
    for (auto& d : g_fs->list_root())
        if (d.name == a[0]) {
            auto data = g_fs->read_all(d.inum);
            fmt::print("{}\n", std::string(data.begin(),
                data.begin() + std::min<std::size_t>(data.size(), 100)));
            if (data.size() > 100) fmt::print("... ({} total)\n", data.size());
            return;
        }
    LOG_ERR("not found\n");
}
static void cmd_rm(const std::vector<std::string>& a) {
    g_fs->unlink(a[0]); LOG_OK("rm {}\n", a[0]);
}
static void cmd_open(const std::vector<std::string>& a) {
    int fd = g_fs->open(a[0]); LOG_OK("open {} → fd={}\n", a[0], fd);
}
static void cmd_close(const std::vector<std::string>& a) {
    g_fs->close(std::atoi(a[0].c_str())); LOG_OK("close fd={}\n", a[0]);
}
static void cmd_stat(const std::vector<std::string>&) {
    auto s = g_fs->stat();
    double used = 100.0 * s.data_blocks_used / (IMG_BLOCKS - DATA_FIRST);
    fmt::print("files={}  inodes_used={}  data_used={}/{}blks ({:.1f}%)  zombies={}\n",
               s.files, s.inodes_used, s.data_blocks_used,
               IMG_BLOCKS - DATA_FIRST, used, s.zombies);
}

int main() {
    mlab::Repl r;
    r.reg("mount", "mount <image>",             cmd_mount);
    r.reg("ls",    "list root",                 cmd_ls);
    r.reg("write", "write <name> <size|@file>", cmd_write);
    r.reg("cat",   "cat <name>",                cmd_cat);
    r.reg("rm",    "rm <name>",                 cmd_rm);
    r.reg("open",  "open <name>",               cmd_open);
    r.reg("close", "close <fd>",                cmd_close);
    r.reg("stat",  "fs stat",                   cmd_stat);
    LOG_INFO("box_fs ready. first: ./bin/mkfs disk.img && mount disk.img\n");
    return r.run();
}
