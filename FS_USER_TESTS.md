# File System User-Space Score Tests

This repository includes user-space tests for the file-system project in
`project.md`. The tests are grading helpers: they exercise the VFS-visible
behavior an Ext2 implementation must provide.

## Syscall Interface

The user programs depend on these calls:

- `open(path, flags)`, `close(fd)`, `read(fd, buf, len)`, `write(fd, buf, len)`
- `lseek(fd, offset, whence)`
- `mkdir(path)`, `rmdir(path)`, `unlink(path)`
- `getdents(fd, struct dirent *buf, len)`
- `stat(path, struct stat *st)`, `fstat(fd, struct stat *st)`
- `mkfifo(path)`
- `gettimeofday(struct timeval *tv)` for bonus benchmarks

The shared ABI definitions live in `os/fs/uapi.h`. An Ext2 directory iterator
should convert Ext2's variable-length directory entries into the fixed
`struct dirent` used by these tests.

## Score Mapping

| Project item | Points | User program | Passing condition |
| --- | ---: | --- | --- |
| Mount and superblock parsing | 4 | `fs_score_basic` | Root directory and `/hello` fixture can be opened after boot. |
| File create/read/write/touch-like create | 5 | `fs_score_basic` | `O_CREAT`, `O_TRUNC`, read/write round trip, and in-place overwrite pass. |
| Directory create/find/delete/getdents | 2 | `fs_score_dir` | Nested directory file is found through paths and `getdents`; non-empty `rmdir` fails; cleanup succeeds. |
| Direct and indirect indexing | 2 | `fs_score_index` | Default mode crosses the 12 direct blocks into indirect storage; optional modes stress double/triple indirect. |
| Metadata update | 1 | `fs_score_meta` | `stat/fstat` report correct type, link count, and size across write, overwrite, extension, and unlink. |
| FIFO interface | 1 | `fs_score_fifo` | `mkfifo`, open, cross-process read/write, and unlink pass. |
| Bonus sequential I/O | up to 4 | `fs_bench_seq` | Prints read/write timing. |
| Bonus random I/O | up to 4 | `fs_bench_rand` | Prints random read/write timing. |
| Bonus small-file latency | up to 4 | `fs_bench_small` | Prints create/read/write/unlink timing. |

## Recommended Run Order

Build and boot the OS image:

```sh
make build
make run
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

## Notes and Preconditions

- The root file system must be mounted before user programs run. The test
  framework provides a weak `fs_mount_root()` hook that mounts simplefs by
  default; an Ext2 implementation can override that hook.
- If your branch generates Ext2 images from the top-level `Makefile`,
  `FS_BLOCK_SIZE` and `FS_BLOCKS` can control the generated test image. For
  example, `make run FS_BLOCK_SIZE=1024 FS_BLOCKS=90000` creates a larger
  1 KiB-block image suitable for the triple-indirect test.
- The default `fs_score_index` writes 14 blocks of 4 KiB, enough to cross the
  first 12 direct pointers on a 4 KiB-block file system.
- `fs_score_index double` writes 5 MiB and is intended to require double
  indirect mapping on 4 KiB-block Ext2.
- `fs_score_index triple` writes 70 MiB. It is intended for a sufficiently
  large 1 KiB-block Ext2 image, where the triple-indirect region starts after
  roughly 64 MiB. On a 4 KiB-block Ext2 image, reaching triple-indirect data
  would require more than 4 GiB and is not a practical default test here.
- The current ABI only checks size, mode, and link count. If the project also
  exposes access and modification timestamps through `struct stat`, add those
  checks to `fs_score_meta`.
- The bonus programs are timing probes, not PASS/FAIL grading gates. Use larger
  arguments when running on a larger disk image, for example
  `fs_bench_seq 32768`.
