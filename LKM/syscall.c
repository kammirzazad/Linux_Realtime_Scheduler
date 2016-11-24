#include <linux/module.h>        // Added by KAMYAR
#include <linux/moduleparam.h>   // Added by KAMYAR
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/cred.h>          // Added by KAMYAR
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/uidgid.h>        // Added by KAMYAR
//#include <linux/unistd.h>        // Added by KAMYAR
//#include <linux/types.h>         // Added by KAMYAR
 
unsigned long **sys_call_table;

static int userid = 1001;
module_param(userid, int, S_IRUGO);

/*
 *
 * Your code goes here
 * which includes two functions
 * mySysClone and refSysClone
 *
 */

asmlinkage long (*refSysClone)(unsigned long, unsigned long, int __user *, int __user *, int); 

asmlinkage long     mySysClone(unsigned long clone_flags, unsigned long newsp, int __user *parent_tid, int __user *child_tid, int stack_size) 
{
	if( get_current_user()->uid.val == userid)
              printk(KERN_INFO "Alert: %i cloned in %i !\n", get_current_user()->uid.val, current->pid);               

        return refSysClone( clone_flags , newsp , parent_tid , child_tid , stack_size );
}

/*

asmlinkage long (*ref_sys_open)(const char __user *filename, int flags, umode_t mode);
asmlinkage long (*ref_sys_read)(unsigned int fd, char __user *buf, size_t count);
asmlinkage long (*ref_sys_write)(unsigned int fd, const char __user *buf, size_t count);

asmlinkage long new_sys_open(const char __user *filename, int flags, umode_t mode)
{
	return ref_sys_open(filename, flags, mode);
}

asmlinkage long new_sys_read(unsigned int fd, char __user *buf, size_t count)
{
	long ret;
	ret = ref_sys_read(fd, buf, count);

	if(count == 1 && fd == 0)
		printk(KERN_INFO "intercept: 0x%02X", buf[0]);

	return ret;
}

asmlinkage long new_sys_write(unsigned int fd, const char __user *buf, size_t count)
{
	return ref_sys_write(fd, buf, count);
}
*/

static unsigned long **aquire_sys_call_table(void)
{
	unsigned long int offset = PAGE_OFFSET;
	unsigned long **sct;

	while (offset < ULLONG_MAX) {
		sct = (unsigned long **)offset;

		if (sct[__NR_close] == (unsigned long *) sys_close) 
			return sct;

		offset += sizeof(void *);
	}

	return NULL;
}

static void disable_page_protection(void) 
{
	unsigned long value;
	asm volatile("mov %%cr0, %0" : "=r" (value));

	if(!(value & 0x00010000))
		return;

	asm volatile("mov %0, %%cr0" : : "r" (value & ~0x00010000));
}

static void enable_page_protection(void) 
{
	unsigned long value;
	asm volatile("mov %%cr0, %0" : "=r" (value));

	if((value & 0x00010000))
		return;

	asm volatile("mov %0, %%cr0" : : "r" (value | 0x00010000));
}

static int __init interceptor_start(void) 
{
	if(!(sys_call_table = aquire_sys_call_table()))
		return -1;

	disable_page_protection();
	refSysClone = (void *)sys_call_table[__NR_clone];
	sys_call_table[__NR_clone] = (unsigned long *)mySysClone;
	enable_page_protection();

	return 0;
}

static void __exit interceptor_end(void) 
{
	if(!sys_call_table)
		return;

	disable_page_protection();
	sys_call_table[__NR_clone] = (unsigned long *)refSysClone;
	enable_page_protection();
}

module_init(interceptor_start);
module_exit(interceptor_end);
