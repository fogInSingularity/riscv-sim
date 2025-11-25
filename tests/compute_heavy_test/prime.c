typedef unsigned int u32;
typedef int i32;
typedef unsigned int usize;
typedef int isize;

extern isize read(int fd, void *buf, usize count);
extern isize write(int fd, const void *buf, usize count);
extern __attribute__((noreturn)) void exit(int code);

#define INPUT_BUF_SIZE 64
#define MAX_N 50000

static char input_buf[INPUT_BUF_SIZE];
static unsigned char is_composite[MAX_N + 1];

static int read_int_from_stdin(void) {
    isize n = read(0, input_buf, INPUT_BUF_SIZE - 1);
    if (n <= 0) {
        return -1;
    }

    int value = 0;
    for (isize i = 0; i < n; ++i) {
        char c = input_buf[i];
        if (c < '0' || c > '9') {
            break;
        }
        int digit = (int)(c - '0');
        int v8 = value << 3;
        int v2 = value << 1;
        value = v8 + v2 + digit;  /* value = value * 10 + digit, but via shifts */
    }
    return value;
}

static int u32_to_dec(u32 value, char *out) {
    static const u32 pow10[10] = {
        1000000000u, 100000000u, 10000000u, 1000000u, 100000u,
        10000u, 1000u, 100u, 10u, 1u
    };

    if (value == 0) {
        out[0] = '0';
        return 1;
    }

    int started = 0;
    int pos = 0;

    for (int i = 0; i < 10; ++i) {
        u32 base = pow10[i];
        u32 digit = 0;

        if (base == 1u) {
            digit = value;
            value = 0;
        } else {
            while (value >= base) {
                value -= base;
                digit += 1u;
            }
        }

        if (digit != 0u || started) {
            out[pos++] = (char)('0' + (int)digit);
            started = 1;
        }
    }

    if (!started) {
        out[0] = '0';
        return 1;
    }

    return pos;
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    int n = read_int_from_stdin();
    if (n < 2) {
        const char msg[] = "0\n";
        write(1, msg, sizeof(msg) - 1);
        return 0;
    }

    if (n > MAX_N) {
        n = MAX_N;
    }

    for (int i = 0; i <= n; ++i) {
        is_composite[i] = 0;
    }

    for (int i = 2; i <= n; ++i) {
        if (is_composite[i]) {
            continue;
        }
        for (int j = i + i; j <= n; j += i) {
            is_composite[j] = 1;
        }
    }

    u32 sum = 0;
    for (int i = 2; i <= n; ++i) {
        if (!is_composite[i]) {
            sum += (u32)i;
        }
    }

    char outbuf[32];
    int len = u32_to_dec(sum, outbuf);
    outbuf[len++] = '\n';

    write(1, outbuf, (usize)len);
    return 0;
}
