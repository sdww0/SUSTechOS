#include "defs.h"
#include "../fs/fs.h"

extern void ramfs_init();

void fstest(uint64) {
    infof("fstest");
    char buf[512];
    int ret;

    ramfs_init();

    struct file *f, *f2;

    // test. 1: open and close a file
    assert_eq(0, vfs_open(&f, "/hello", O_RDWR));
    assert_eq(0, vfs_close(f));
    infof("test1 passed");
    
    // test. 2: open a file, read nothing, write 12 bytes
    assert_eq(0, vfs_open(&f, "/hello", O_RDWR));
    infof("hello inode: %d, ref: %d", file_inode(f)->ino, file_inode(f)->ref);
    assert_eq(0, vfs_read(f, buf, 512));
    strncpy(buf, "hello world", 12);
    assert_eq(12, vfs_write(f, buf, 12));
    assert_eq(0, vfs_close(f));
    infof("test2 passed");
    
    // test. 3: open a file, read 12 bytes, seek to the beginning, read again
    assert_eq(0, vfs_open(&f, "/hello", O_RDWR));
    assert_eq(12, vfs_read(f, buf, 512));
    assert(strncmp(buf, "hello world", 12) == 0);
    assert_eq(0, vfs_lseek(f, 0, SEEK_SET));
    assert_eq(12, vfs_read(f, buf, 512));
    assert(strncmp(buf, "hello world", 12) == 0);
    assert_eq(0, vfs_close(f));
    infof("test3 passed");

    // test. 4: open a file twice, they share the same inode
    assert_eq(0, vfs_open(&f, "/hello", O_RDWR));
    assert_eq(0, vfs_open(&f2, "/hello", O_RDWR));
    assert_eq(file_inode(f), file_inode(f2));
    assert_eq(2, file_inode(f)->ref);
    assert_eq(0, vfs_close(f));
    assert_eq(1, file_inode(f2)->ref);
    assert_eq(0, vfs_close(f2));
    infof("test4 passed");

    // test. 5: create a file
    assert_eq(0, vfs_create(&f, "/hello2"));
    assert_eq(1, file_inode(f)->ref);
    assert_eq(0, vfs_close(f));
    assert_eq(0, vfs_open(&f, "/hello2", O_RDWR));
    assert_eq(20, vfs_lseek(f, 20, SEEK_END));
    assert_eq(20, f->pos);
    assert_eq(12, vfs_write(f, buf, 12));
    assert_eq(32, f->pos);
    assert_eq(0, vfs_close(f));

    assert_eq(0, vfs_open(&f, "/hello2", O_RDWR));
    assert_eq(32, file_inode(f)->size);
    assert_eq(0, vfs_close(f));
    infof("test5 passed");
    
    exit(0);
}