#include "fs_score_common.h"

#define SCORE_DIR    "/score_dir"
#define SCORE_NESTED "/score_dir/nested"
#define SCORE_FILE   "/score_dir/nested/file"

static char buf[SCORE_BLOCK_SIZE];

static void cleanup(void) {
    unlink(SCORE_FILE);
    rmdir(SCORE_NESTED);
    rmdir(SCORE_DIR);
}

int main(int argc, char **argv) {
    stdout_nobuf();
    (void)argc;
    (void)argv;

    cleanup();

    CHECK(mkdir(SCORE_DIR) == 0, "mkdir %s failed", SCORE_DIR);
    CHECK(mkdir(SCORE_NESTED) == 0, "mkdir %s failed", SCORE_NESTED);

    int fd = open(SCORE_FILE, O_CREAT | O_RDWR | O_TRUNC);
    CHECK(fd >= 0, "create nested file ret=%d", fd);
    memmove(buf, "hello from nested dir", 21);
    write_full(fd, buf, 21);
    CHECK(close(fd) == 0, "close nested file failed");

    fd = open(SCORE_FILE, O_RDWR);
    CHECK(fd >= 0, "reopen nested file ret=%d", fd);
    memset(buf, 0, sizeof(buf));
    read_full(fd, buf, 21);
    CHECK(memcmp(buf, "hello from nested dir", 21) == 0, "nested file content mismatch");
    CHECK(close(fd) == 0, "close reopened nested file failed");

    CHECK(dir_contains(SCORE_DIR, "nested"), "getdents did not find nested");
    CHECK(dir_contains(SCORE_NESTED, "file"), "getdents did not find file");
    CHECK(rmdir(SCORE_DIR) < 0, "rmdir non-empty directory unexpectedly succeeded");

    CHECK(unlink(SCORE_FILE) == 0, "unlink nested file failed");
    CHECK(open(SCORE_FILE, O_RDONLY) < 0, "unlinked nested file is still openable");
    CHECK(rmdir(SCORE_NESTED) == 0, "rmdir nested failed");
    CHECK(rmdir(SCORE_DIR) == 0, "rmdir score dir failed");

    pass("fs_score_dir");
    return 0;
}
