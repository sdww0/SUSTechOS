#include "fs_score_common.h"

#define INDEX_PATH "/score_index"

static char block[SCORE_BLOCK_SIZE];
static char expected[SCORE_BLOCK_SIZE];

static void write_blocks(int fd, int blocks) {
    for (int i = 0; i < blocks; i++) {
        fill_pattern(block, sizeof(block), i);
        write_full(fd, block, sizeof(block));
    }
}

static void verify_block(int fd, int blockno) {
    int off = blockno * SCORE_BLOCK_SIZE;
    CHECK(lseek(fd, off, SEEK_SET) == off, "seek block %d failed", blockno);
    memset(block, 0, sizeof(block));
    read_full(fd, block, sizeof(block));
    fill_pattern(expected, sizeof(expected), blockno);
    CHECK(memcmp(block, expected, sizeof(block)) == 0, "block %d data mismatch", blockno);
}

static void verify_interesting_blocks(int fd, int blocks) {
    verify_block(fd, 0);
    verify_block(fd, 11);
    verify_block(fd, 12);
    verify_block(fd, 13);
    if (blocks > 1035)
        verify_block(fd, 1035);
    if (blocks > 1036)
        verify_block(fd, 1036);
    if (blocks > 17000)
        verify_block(fd, 17000);
    verify_block(fd, blocks - 1);
}

int main(int argc, char **argv) {
    stdout_nobuf();

    int blocks = 14;
    char *mode = "single";
    if (argc > 1 && strcmp(argv[1], "double") == 0) {
        blocks = 1280;
        mode = "double";
    } else if (argc > 1 && strcmp(argv[1], "triple") == 0) {
        blocks = 17920;
        mode = "triple";
    }

    unlink(INDEX_PATH);
    int fd = open(INDEX_PATH, O_CREAT | O_RDWR | O_TRUNC);
    CHECK(fd >= 0, "create %s ret=%d", INDEX_PATH, fd);

    write_blocks(fd, blocks);
    CHECK(file_size(INDEX_PATH) == blocks * SCORE_BLOCK_SIZE, "file size after index write mismatch");

    verify_interesting_blocks(fd, blocks);
    close_ok(fd);

    fd = open(INDEX_PATH, O_RDONLY);
    CHECK(fd >= 0, "reopen index file ret=%d", fd);
    verify_interesting_blocks(fd, blocks);
    close_ok(fd);
    CHECK(unlink(INDEX_PATH) == 0, "unlink index file failed");

    printf("PASS fs_score_index %s blocks=%d\n", mode, blocks);
    return 0;
}
