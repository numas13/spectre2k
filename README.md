# PoC of Spectre vulnerability running on Elbrus2000

Build with optimizations (O2 or O3) and run.

```
$ lcc -O2 main.c -o spectre2k
$ spectre2k 
Hello OpenE2K
$ spectre2k 'Hello, world'
Hello, world!
```

O1 will not work.

```
$ lcc -O1 main.c -o spectre2k
$ spectre2k 
\\x04\\x0c\\x0b\\x1b\\x1b\\x0c\\x13+\\x13\\x0b\\x14\\x13\\x0b
$ spectre2k 
\\x07\\x07\\x17\\x07\\x07\\x07\\x08\\x07\\x18\\x17\\x1f\\x0f\\x0f
```

```
$ lcc --version
lcc:1.26.22:Jan-10-2024:e2k-v4-linux
gcc (GCC) 9.3.0 compatible
```
