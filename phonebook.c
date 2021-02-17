#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");

int major_num;

struct phonebook_user {
    char *name;
    char *surname;
    char *email;
    char *phone;
    long age;
};

struct phonebook_response {
    char *start;
    size_t position;
} phonebook_response;

struct phonebook_request {
    char* start;
    size_t len;
} phonebook_request;

static void free_request(void) {
    if (phonebook_request.start != NULL) {
        kfree(phonebook_request.start);
    }
    phonebook_request.start = 0;
    phonebook_request.len = 0;
}

static void set_response(char* s) {
    phonebook_response.start = s;
    phonebook_response.position = 0;
}

static void static_response(char* s) {
    size_t len = strlen(s);
    char* copied = kmalloc_array(len, sizeof(char), GFP_KERNEL);
    if (copied == 0) {
        return;
    }
    memcpy(copied, s, len);
    set_response(copied);
}

static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset) {
    if (buffer == 0) {
        return -EINVAL;
    }
    if (phonebook_response.start == 0) {
        // For debugging.
        if (phonebook_request.len <= *offset) {
            return 0;
        }
        size_t remaining = phonebook_request.len - (*offset);
        int to_read = min(len, remaining);
        int i;
        for (i=0; i<to_read; i++) {
            put_user(phonebook_request.start[*offset], buffer++);
            (*offset)++;
        }
        return to_read;
    }
    {
        int bytes_read = 0;
        while (len) {
            char ch = phonebook_response.start[phonebook_response.position++];
            if (ch == 0) {
                kfree(phonebook_response.start);
                phonebook_response.start = 0;
                phonebook_response.position = 0;
                break;
            }
            put_user(ch, buffer++);
            len--;
            (*offset)++;
            bytes_read++;
        }
        return bytes_read;
    }
}

static int format_user(struct phonebook_user user, char* buf, size_t size) {
    return snprintf(buf, size,
                    "Name: %s\nSurname: %s\nEmail: %s\nPhone: %s\nAge: %ld\n",
                    user.name, user.surname, user.email, user.phone, user.age);
}

static void handle_request(void) {
    if (phonebook_request.start == NULL) {
        return;
    }

    char* fields[5];
    int i;
    for (i=0; i<5; i++) {
        char* field = strsep(&phonebook_request.start, "\n");
        if (field == NULL) {
            static_response("Expected 5 lines.\n");
            goto end;
        }
        fields[i] = field;
    }
    struct phonebook_user user = {
            .surname = fields[0],
            .name = fields[1],
            .email = fields[2],
            .phone = fields[3],
    };
    if (kstrtol(fields[4], 10, &user.age) != 0) {
        static_response("Invalid age given.\n");
        goto end;
    }

    int size = format_user(user, NULL, 0);
    size++;
    char* buf = kmalloc(size, GFP_KERNEL);
    format_user(user, buf, size);
    set_response(buf);

end:
    free_request();
}

static ssize_t device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset) {
    if (buffer == NULL) {
        return -EINVAL;
    }
    if (len == 0) {
        return 0;
    }

    size_t input_len = strnlen(buffer, len);

    if (input_len < len && buffer[input_len] == '\0') {
        input_len++;
    }

    size_t old_len = phonebook_request.len;
    size_t new_len = phonebook_request.len + input_len;
    char *new_req = kmalloc(new_len, GFP_KERNEL);
    if (new_req == 0) {
        free_request();
        return -ENOMEM;
    }
    if (phonebook_request.start != NULL) {
        memcpy(new_req, phonebook_request.start, phonebook_request.len);
        free_request();
    }
    memcpy(new_req + old_len, buffer, input_len);

    phonebook_request.start = new_req;
    phonebook_request.len = new_len;

    // I hate C.
    if (buffer[input_len-1] == '\0') {
        handle_request();
    }

    return input_len;
}

static int device_open(struct inode *inode, struct file *file) {
    if (!try_module_get(THIS_MODULE)) {
        return -EIO;
    }
    return 0;
}

static int device_release(struct inode *inode, struct file *file) {
    module_put(THIS_MODULE);
    return 0;
}

static void mod_exit(void) {
    unregister_chrdev(major_num, "eg_device");
}

struct file_operations file_ops = {
        .read = device_read,
        .write = device_write,
        .open = device_open,
        .release = device_release
};

static int mod_init(void) {
    major_num = register_chrdev(0, "eg_device", &file_ops);
    if (major_num < 0) {
        printk(KERN_ALERT "Could not register device: %d\n", major_num);
        return major_num;
    } else {
        printk(KERN_INFO "eg_device device major number %d\n", major_num);
        return 0;
    }
}

module_init(mod_init);
module_exit(mod_exit);
