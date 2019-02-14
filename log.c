#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/slab.h>

#include "log.h"

struct bl_log *bl_log = NULL;

static int bl_log_open(struct inode *inode, struct file *file)
{

	struct mm_struct *mm = current->mm;
	file->private_data = bl_log;

	printk("client: %s (%d)\n", current->comm, current->pid);
	printk("code  section: [0x%lx   0x%lx]\n", mm->start_code, mm->end_code);
	printk("data  section: [0x%lx   0x%lx]\n", mm->start_data, mm->end_data);
	printk("brk   section: s: 0x%lx, c: 0x%lx\n", mm->start_brk, mm->brk);
	printk("mmap  section: s: 0x%lx\n", mm->mmap_base);
	printk("stack section: s: 0x%lx\n", mm->start_stack);
	printk("arg   section: [0x%lx   0x%lx]\n", mm->arg_start, mm->arg_end);
	printk("env   section: [0x%lx   0x%lx]\n", mm->env_start, mm->env_end);

	return 0;
}

static int bl_log_release(struct inode *inode, struct file *file)
{
	
	struct bl_log *bl_log = file->private_data;

    kfree(bl_log->buff);	
	kfree(bl_log);

	return 0;
}

static int bl_log_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct bl_log *bl_log = file->private_data;
	unsigned long offset = vma->vm_pgoff << PAGE_SHIFT;
	unsigned long pfn_start = (virt_to_phys(bl_log->buff) >> PAGE_SHIFT) + vma->vm_pgoff;
	unsigned long virt_start = (unsigned long)bl_log->buff + offset;
	unsigned long size = vma->vm_end - vma->vm_start;
	int ret = 0;

	printk("phy: 0x%lx, offset: 0x%lx, size: 0x%lx\n", pfn_start << PAGE_SHIFT, offset, size);

	ret = remap_pfn_range(vma, vma->vm_start, pfn_start, size, vma->vm_page_prot);
	if (ret)
		printk("%s: remap_pfn_range failed at [0x%lx  0x%lx]\n",
			__func__, vma->vm_start, vma->vm_end);
	else
		printk("%s: map 0x%lx to 0x%lx, size: 0x%lx\n", __func__, virt_start,
			vma->vm_start, size);

	return ret;
}

static const struct file_operations bl_log_fops = {
	.owner   = THIS_MODULE,
	.open 	 = bl_log_open,
	.release = bl_log_release,
	.mmap 	 = bl_log_mmap,
};

static struct miscdevice bl_log_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "bl_log",
	.fops = &bl_log_fops,
};

static void log_output(char *fmt, ...) {
	va_list args;
	int len;

	va_start(args, fmt);
	//len = vsnprintf(bl_log->buff, 256, fmt, args);
	vsnprintf(bl_log->buff, LOG_LINE_BUF_SIZE, fmt, args);
	va_end(args);
}

static int __init bl_log_init(void)
{
	int ret = 0;


	bl_log = kzalloc(sizeof(struct bl_log), GFP_KERNEL);
	if(!bl_log) {
		ret = -ENOMEM;
		goto err_bl_log;
	}

	bl_log->buff = kzalloc(LOG_BUF_OUTPUT_BUF_SIZE, GFP_KERNEL);
	if (!bl_log->buff) {
		ret = -ENOMEM;
		goto err_buff;
	}

	log_output("%d, just for test\n", ret);

	printk("%s\n", bl_log->buff);

	ret = misc_register(&bl_log_misc);
	if (unlikely(ret)) {
		pr_err("failed to register misc device!\n");
		goto err_register;
	}

	return 0;

err_register:
	kfree(bl_log->buff);
err_buff:
	kfree(bl_log);
err_bl_log:
	return ret;

}

static void __exit bl_log_exit(void)
{
	misc_deregister(&bl_log_misc);
}

module_init(bl_log_init);
module_exit(bl_log_exit);
MODULE_LICENSE("GPL");
