#include "fs_score_common.h"

#define BASIC_PATH "/score_basic"

static char buf[SCORE_BLOCK_SIZE];
static char tmp[SCORE_BLOCK_SIZE];

int main(int argc, char **argv) {
    stdout_nobuf();
    (void)argc;
    (void)argv;

    cleanup_path(BASIC_PATH);

    int fd = open("/", O_RDONLY);
    CHECK(fd >= 0, "root directory open failed ret=%d", fd);
    CHECK(close(fd) == 0, "close root failed");

    fd = open("/hello", O_RDWR);
    CHECK(fd >= 0, "fixture /hello open failed ret=%d", fd);
    CHECK(close(fd) == 0, "close /hello failed");

    fd = open(BASIC_PATH, O_CREAT | O_RDWR | O_TRUNC);
    CHECK(fd >= 0, "create %s ret=%d", BASIC_PATH, fd);
    memmove(buf, "abcdef", 6);
    write_full(fd, buf, 6);

    CHECK(lseek(fd, 0, SEEK_SET) == 0, "seek start after create failed");
    memset(tmp, 0, sizeof(tmp));
    read_full(fd, tmp, 6);
    CHECK(memcmp(tmp, "abcdef", 6) == 0, "read after write mismatch");

    CHECK(lseek(fd, 2, SEEK_SET) == 2, "seek for overwrite failed");
    memmove(buf, "XY", 2);
    write_full(fd, buf, 2);
    CHECK(lseek(fd, 0, SEEK_SET) == 0, "seek start after overwrite failed");
    memset(tmp, 0, sizeof(tmp));
    read_full(fd, tmp, 6);
    CHECK(memcmp(tmp, "abXYef", 6) == 0, "in-place overwrite mismatch");
    CHECK(close(fd) == 0, "close after overwrite failed");

    fd = open(BASIC_PATH, O_RDWR | O_TRUNC);
    CHECK(fd >= 0, "truncate reopen ret=%d", fd);
    memmove(buf, "ok", 2);
    write_full(fd, buf, 2);
    CHECK(lseek(fd, 0, SEEK_SET) == 0, "seek start after trunc failed");
    memset(tmp, 0, sizeof(tmp));
    read_full(fd, tmp, 2);
    CHECK(memcmp(tmp, "ok", 2) == 0, "truncate data mismatch");
    CHECK(read(fd, tmp, 1) == 0, "truncate left stale bytes");
    CHECK(close(fd) == 0, "close truncated file failed");

    CHECK(unlink(BASIC_PATH) == 0, "unlink %s failed", BASIC_PATH);
    pass("fs_score_basic");
    return 0;
}
