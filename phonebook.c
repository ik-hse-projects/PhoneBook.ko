#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

int major_num;

static ssize_t device_read(struct file* flip, char* buffer, size_t len, loff_t* offset) {
	int bytes_read = 0;
	char* msg_ptr = "Hello!\0";
	if (*offset >= strlen(msg_ptr)) {
		return 0;
	}
	while (len && *msg_ptr) {
		put_user(*(msg_ptr++), buffer++);
		len--;
        (*offset)++;
		bytes_read++;
	}
	return bytes_read;
}

static ssize_t device_write(struct file* flip, const char* buffer, size_t len, loff_t* offset) {
	printk(KERN_ALERT "Invalid attempt to write to eg_device.\n");
	return -EINVAL;
}

static int device_open(struct inode* inode, struct file* file) {
	try_module_get(THIS_MODULE);
	return 0;
}

static int device_release(struct inode* inode, struct file* file) {
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
