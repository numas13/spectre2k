# PoC of Spectre vulnerability running on Elbrus2000

Build with optimizations (O2 or O3) and run.

```
$ lcc -O2 main.c -o spectre2k
$ ./spectre2k
Hello OpenE2K
$ ./spectre2k 'Hello, world'
Hello, world!
```

Lower level of optimizations (O0 or O1) does not have vulnerability.

```
$ lcc -O1 main.c -o spectre2k
$ ./spectre2k
\\x04\\x0c\\x0b\\x1b\\x1b\\x0c\\x13+\\x13\\x0b\\x14\\x13\\x0b
$ ./spectre2k
\\x07\\x07\\x17\\x07\\x07\\x07\\x08\\x07\\x18\\x17\\x1f\\x0f\\x0f
```

Protected mode (ЗРИ/ТБВ) also has vulnerability with high level of optimizations (O2/O3):

```
$ lcc -O2 -mptr128 main.c -o spectre2k
$ ./spectre2k
Hello OpenE2K
```

Similarly, O0 and O1 levels of optimization are free from vulnerability:

```
$ lcc -O1 -mptr128 main.c -o spectre2k
$ ./spectre2k
\\x06\\x06\\x07\\x07\\x0e\\x06\\x06\\x07\\x06\\x07\\x06\\x0e\\x07
$ ./spectre2k
\\x06\\x06\\x06\\x06\\x0f\\x06\\x06\\x06\\x06\\x06\\x06\\x07\\x06
```

Used LCC version:
```
$ lcc --version
lcc:1.26.22:Jan-10-2024:e2k-v4-linux
gcc (GCC) 9.3.0 compatible
```
