# File System User-Space Tests

Build and boot the OS image. For Ext2 grading, pass `FS_TYPE=ext2` so the
top-level `Makefile` creates an Ext2 `fs.img`:

```sh
make run FS_TYPE=ext2
```

After booting into `sh`, run:

```text
fs_score_basic
fs_score_dir
fs_score_index
fs_score_index double
fs_score_index triple
fs_score_meta
fs_score_fifo
fs_bench_seq
fs_bench_rand
fs_bench_small
```

Reference success output:

```text
PASS fs_score_basic
PASS fs_score_dir
PASS fs_score_index single blocks=14
PASS fs_score_index double blocks=1280
PASS fs_score_index triple blocks=17920
PASS fs_score_meta
PASS fs_score_fifo
BENCH fs_bench_seq bytes=2097152 write_us=... read_us=...
BENCH fs_bench_rand blocks=256 ops=256 write_us=... read_us=...
BENCH fs_bench_small files=64 create_rw_us=... unlink_us=...
```

Any correctness failure prints `FAIL ...` with the source line and the syscall
return value that caused the failure.
