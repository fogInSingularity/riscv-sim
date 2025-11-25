typedef unsigned int u32;
typedef int i32;
typedef unsigned int usize;
typedef int isize;

extern isize read(int fd, void *buf, usize count);
extern isize write(int fd, const void *buf, usize count);
extern void exit(int code);

#define INPUT_BUF_SIZE 64
#define MAX_N 200000

static char input_buf[INPUT_BUF_SIZE];

static int read_int_from_stdin(void) {
    isize n = read(0, input_buf, INPUT_BUF_SIZE);
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
        value = value * 10 + digit;
    }
    return value;
}

static int is_prime_int(int x) {
    if (x < 2) {
        return 0;
    }
    if (x == 2) {
        return 1;
    }
    if ((x % 2) == 0) {
        return 0;
    }

    for (int d = 3; (d * d) <= x; d += 2) {
        if ((x % d) == 0) {
            return 0;
        }
    }
    return 1;
}

static int u32_to_dec(u32 value, char *out) {
    if (value == 0u) {
        out[0] = '0';
        return 1;
    }

    int len = 0;
    while (value != 0u) {
        u32 q = value / 10u;
        u32 r = value % 10u;
        out[len++] = (char)('0' + (int)r);
        value = q;
    }

    for (int i = 0; i < len / 2; ++i) {
        char tmp = out[i];
        out[i] = out[len - 1 - i];
        out[len - 1 - i] = tmp;
    }

    return len;
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

    u32 sum = 0;
    for (int i = 2; i <= n; ++i) {
        if (is_prime_int(i)) {
            sum += (u32)i;
        }
    }

    char outbuf[32];
    int len = u32_to_dec(sum, outbuf);
    outbuf[len++] = '\n';

    write(1, outbuf, (usize)len);
    return 0;
}
