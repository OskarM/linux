/*
 *  linux/arch/arm/kernel/sys_arm.c
 *
 *  Copyright (C) People who wrote linux/arch/i386/kernel/sys_i386.c
 *  Copyright (C) 1995, 1996 Russell King.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 *  This file contains various random system calls that
 *  have a non-standard calling sequence on the Linux/arm
 *  platform.
 */
#include <linux/export.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/sem.h>
#include <linux/msg.h>
#include <linux/shm.h>
#include <linux/stat.h>
#include <linux/syscalls.h>
#include <linux/mman.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/ipc.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

/*
 * Since loff_t is a 64 bit type we avoid a lot of ABI hassle
 * with a different argument ordering.
 */
asmlinkage long sys_arm_fadvise64_64(int fd, int advice,
				     loff_t offset, loff_t len)
{
	return sys_fadvise64_64(fd, offset, len, advice);
}

/*
 * Get information about the number of processes, pending signals and
 * file descriptors currently watched by the user.
 */
asmlinkage long sys_userinfo(int *processes, int *signals, int *open_files)
{ 
	struct user_struct *userinfo;
	int errno = 0;

	/* Check user space memory allocation and correct information retrieval */
	if ((userinfo = get_current_user()) != NULL) 
	{
		int p = atomic_read(&userinfo->processes);
		int s = atomic_read(&userinfo->sigpending);
		int f = atomic_read(&userinfo->inotify_watches);

		printk(KERN_ALERT "Processes: %d\n", atomic_read(&userinfo->processes));
		printk(KERN_ALERT "Signals: %d\n", atomic_read(&userinfo->sigpending));
		printk(KERN_ALERT "Open files: %d\n", atomic_read(&userinfo->inotify_watches));

		if (copy_to_user(processes, &p, sizeof(int)))
		{
			errno = -EFAULT;
		}
		else if (copy_to_user(signals, &s, sizeof(int))) 
		{
			errno = -EFAULT;
		}
		else if (copy_to_user(open_files, &f, sizeof(int)))
		{
			errno =  -EFAULT;
		}

		
	} else {
		printk(KERN_ALERT "Unable to get user information\n");
		errno = -1;
	}

	return errno;

}
