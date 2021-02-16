#include <linux/module.h>

MODULE_LICENSE("GPL");

int rust_foo(void);

static int mod_init(void) {
    panic("Hello world");
    return 0;
}

module_init(mod_init);
