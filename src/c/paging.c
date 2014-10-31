#include <paging.h>

static void (*page_fault_handler)(u8int *buf, u16int size) = NULL;

void paging_initialize()
{
	// i need to set up a pointer to my page directory.
	// i can use the memory manager to allocate the first free page for the page directory address.
	u32int *page_directory = (u32int *) allocate_block();
	
	// i need to zero the entries on the page directory
	for (int i = 0; i < 1024; i++)
	{
		//attribute: supervisor level, read/write, not present.
		page_directory[i] = 0 | 2;
	}
	
	// i need to create the first page table.
	// allocate another page for the page table
	u32int *page_table = (u32int *) allocate_block();
	
	// i need to fill in the page table, identity mapping (1:1) the first 4M
	u32int addr = 0;
	for (u32int i = 0; i < 1024; i++)
	{
		u32int page = addr & ~(0xFFF);
		// attributes: supervisor level, read/write, present.
		page_table[i] = page | 3;
		addr += 4096;
	}
	
	// i need to put the page table on the page directory
	// attributes: supervisor level, read/write, present
	page_directory[0] = (u32int) page_table | 3;
	
	// i need to register the page fault interrupt handler
	register_interrupt_handler(14, (isr) &page_fault_interrupt_handler);
	
	// i need to load the page directory address into cr3
	write_cr3((u32int) page_directory);
	
	// i need to set the proper bit on cr0
	write_cr0((u32int) (read_cr0() | 0x80000000));
	
}

void page_fault_set_handler(void (*callback)(u8int *buf, u16int size))
{
	page_fault_handler = callback;
}

void page_fault_interrupt_handler(registers regs)
{
	put_str("\nPage fault interrupt handler called.");
	
	put_str("\nds=");
	put_hex(regs.ds);
	
	put_str("\nedi=");
	put_hex(regs.edi);
	
	put_str(" esi=");
	put_hex(regs.esi);
	
	put_str(" ebp=");
	put_hex(regs.ebp);
	
	put_str(" esp=");
	put_hex(regs.esp);
	
	put_str(" ebx=");
	put_hex(regs.ebx);
	
	put_str(" edx=");
	put_hex(regs.edx);
	
	put_str(" ecx=");
	put_hex(regs.ecx);
	
	put_str(" eax=");
	put_hex(regs.eax);
	
	put_str("\nint_no=");
	put_dec(regs.int_no);
	
	put_str(" err_code=");
	put_hex(regs.err_code);
	
	put_str("\neip=");
	put_hex(regs.eip);
	
	put_str(" cs=");
	put_hex(regs.cs);
	
	put_str(" eflags=");
	put_hex(regs.eflags);
	
	put_str(" useresp=");
	put_hex(regs.useresp);
	
	put_str(" ss=");
	put_hex(regs.ss);
	
	u32int cr2_val = read_cr2();
	u32int cr3_val = read_cr3();
	u32int cr4_val = read_cr4();
	
	put_str("\ncr2 val is ");
	put_hex(cr2_val);
	
	put_str("\ncr3 val is ");
	put_hex(cr3_val);
	
	put_str("\ncr4 val is ");
	put_hex(cr4_val);
	
	u32int present = regs.err_code & 0x1;
	u32int rw = regs.err_code & 0x2;
	u32int us = regs.err_code & 0x4;
	u32int reserved = regs.err_code & 0x8;
	u32int id = regs.err_code & 0x10;
	
	put_str("\nError code evaluation:");
	if (present) put_str(" P");
	if (rw) put_str(" R/W");
	if (us) put_str(" U/S");
	if (reserved) put_str(" RSVD");
	if (id) put_str(" I/D");
	
	// now what?
	
	
	// do i really need to be calling a kernel level function?
	string msg = "\nPage Fault";
	page_fault_handler((u8int *) msg, strlen(msg));
}













