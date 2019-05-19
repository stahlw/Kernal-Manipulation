//Wesley Stahl
//Peter Park
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/sched.h>
#include<linux/list.h>
#include<linux/sched/signal.h>							//May not work with kernel version 4.10

void procs_info_print(void) {
  struct task_struct *parent_list, *child_list;       				//Structs for all processes
  size_t unrun = 0, run = 0, stopped = 0, children = 0, child_pid = 0, pid = 0; //Vars of type size_t used for counting/storing pid of each process
  struct list_head *children_list;  						//Used to iterate through the list of child processes
  char *parent, *child, *noChild = "No Children";
  for_each_process(parent_list) {  						//Counts the number of unrunnable, runnable, and stopped processes
    if (parent_list->state == -1) {
      unrun++;
    } else if (parent_list->state == 0) {
      run++;
    } else {
      stopped++;
    }
  }
  printk(KERN_INFO "Unrunnable: %zu\n", unrun);					//Prints number of unrunnable processes
  printk(KERN_INFO "Runnable: %zu\n", run);					//Prints number of runnable processes
  printk(KERN_INFO "Stopped: %zu\n", stopped);					//Prints number of stopped processes
  for_each_process(parent_list) {						//Begins iteration for printing to the kernel log and the /proc log
    parent = parent_list->comm;
    pid = parent_list->pid;
    list_for_each(children_list, &(parent_list->children)) {			//Iterates through thee child processes of the parent process
      children++;
      if (children == 1) {							//If we are at the first child, get name of child and the pid of the child
        child_list = list_entry(children_list, struct task_struct, sibling);
        child_pid = child_list->pid;
        child = child_list->comm;
      }
    }
    if (children == 0) { 							//If the process has no children
      printk(KERN_INFO "Process ID = %zu Name = %s %s\n", pid, parent, noChild);
    } else {									//Process has atleast one child
      printk(KERN_INFO "Process ID = %zu Name = %s number_of_children = %zu first_child_pid = %zu first_child_name = %s\n", pid, parent, children, child_pid, child); 
      children = 0;
    }
  }
}

int proc_init(void) {
  printk(KERN_INFO "PROCESS REPORTER\n");
  procs_info_print();								//Begins scanning the list of processes
  return 0;
}

void proc_cleanup(void) {
  printk(KERN_INFO "procReport: performing cleanup of module\n");
}

MODULE_LICENSE("GPL");
module_init(proc_init);
module_exit(proc_cleanup);

