# Simplex Kernel Module

This is a loadable kernel module (LKM), allowing MPX instructions to be interpreted by all processes, even without a MPX runtime or wrapper library like Simplex. 

It was originally written for Ubuntu 14.04 LTS and linux kernel 3.13.0. Although it still appears to work as of July 2019 on Ubuntu 18.04 LTS with Linux kernel 4.15.0, we do not intend to actively develop it going forward. Because we cannot guarantee that the LKM will work on future systems, and because failing LKMs have the possibility to crash a system, **we highly recommend that you test the LKM on a virtual machine before deploying it on a system where stability is critical.** We make no warranty for fitness or suitability.

## Instructions

To build the kernel module, use `make all`. To clean up the kernel module and intermediate files, use `make clean`. Load and unload the kernel module using the commands in the table below:

### Loading and Unloading the Simplex LKM

| **Command**                | **Purpose**   |
| ---------------------------| --------------|
| `sudo insmod simplexkm.ko` | Load a LKM.   |
| `sudo rmmod simplexkm.ko`  | Unload a LKM. |

### Inspecting the Simplex LKM

| **Command**                           | **Purpose**                                                                                     |
| --------------------------------------| ------------------------------------------------------------------------------------------------|
| `modinfo simplexkm.ko`                | Get information on a LKM object file.                                                           |
| `lsmod`                               | Get information on all loaded LKMs.                                                             |
| `cat /proc/modules \| grep simplexkm` | Get information on one particular loaded LKM, including its kernel memory offset for debugging. |
| `tail -f /var/log/kern.log`           | View the most recently logged messages from an LKM sent using `printk()`.                       |
