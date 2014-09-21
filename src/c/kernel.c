#include <multiboot.h>
#include <system.h>

#define MAX_TERMINAL_BUFFER_SIZE 4096

u32int initial_esp;

static char terminal_buffer[MAX_TERMINAL_BUFFER_SIZE];
static u16int terminal_buffer_length = 0;
static u16int terminal_last_put = 0;
static char terminal_seperator = '>';

int kernel_main(__attribute__ ((unused)) struct multiboot *mboot_ptr, u32int initial_stack)
{
	initial_esp = initial_stack;
	
	gdt_initialize();
	idt_initialize();
	memset((u8int *) &interrupt_handler, 0, sizeof(isr) * 256);
	
	set_text_color(LIGHT_GREY, BLUE);
	clear_screen();
	vga_buffer_put_str("Welcome to Patrick's Operating System!\n");
	
	enable_interrupts();
	timer_initialize(100);
	keyboard_initialize();
	keyboard_set_handler(kernel_keyboard_handler);
	vga_set_handler(kernel_vga_handler);
	memset((u8int *) terminal_buffer, 0, MAX_TERMINAL_BUFFER_SIZE); // clear the terminal buffer (initalize it to 0 when we start running)
	
	vga_buffer_put_char(terminal_seperator);
	
	for (;;)
	{
		// this is the main loop of the kernel
		keyboard_flush();
		
		// this section is starting to come together, but the whole thing is full if freaking bugs.
		
		if (terminal_buffer_length > 0)
		{
			if (terminal_buffer[terminal_buffer_length - 1] == '\n')
			{
				// replace the \n w/ a null terminator to turn the buffered characters into a "string"
				terminal_buffer[terminal_buffer_length - 1] = '\0';
				// get the first token off the buffer
				int token_size;
				char token[MAX_TERMINAL_BUFFER_SIZE];
				// for each character on the buffer
				for (token_size = 0; token_size < terminal_buffer_length; token_size++)
				{
					// if it's a space, new line, or null terminator
					if (terminal_buffer[token_size] == ' ' || terminal_buffer[token_size] == '\0')
					{
						// put a null terminator on the token
						token[token_size] = '\0';
						// we're done
						break;
					}
					// put it on the token
					token[token_size] = terminal_buffer[token_size];
				}
				
				
				/*
				// shows that we've succcssfully gotten the token, and gotten it correctly.
				vga_buffer_put_str("\n");
				vga_buffer_put_str(token);	// works, prints the token.
				
				vga_buffer_put_str("\n");
				vga_buffer_put_str(terminal_buffer); // works, prints the entire contents of the buffer.
				
				vga_buffer_put_str("\n");
				vga_buffer_put_str(&terminal_buffer[token_size + 1]); // works, prints the contents of everything after the token.
				*/
				
				// testing my string comparison function.
				/* they all seem to work
				string a = "foo";
				string b = "foo";
				string c = "bar";
				string d = "baz";
				string e = "foobar";
				
				vga_buffer_put_str("\n");
				vga_buffer_put_dec(strcmp(a, b));
				
				vga_buffer_put_str("\n");
				vga_buffer_put_dec(strcmp(a, c));
				
				vga_buffer_put_str("\n");
				if (strcmp(c, a) == -1)
				{
					vga_buffer_put_str("-1");
				}
				
				vga_buffer_put_str("\n");
				if (strcmp(d, e) == -1)
				{
					vga_buffer_put_str("-1");
				}
				
				vga_buffer_put_str("\n");
				vga_buffer_put_dec(strcmp(e, d));
				*/
				
				// this is where i evaluate the token, and get ready to send the control elsewhere.
				// i need to make a string comparison function, and then use if/else if/else to find a hook that the kernel can run.
				// this is screwed up some how. when I attempt to use this i get anomylous outputs.
					// things being printed for no apparent reason
					// characters not being printed.
				
				if (strcmp((string) token, "echo") == 0)
				{
					vga_buffer_put_str("\n");
					vga_buffer_put_str(&terminal_buffer[token_size + 1]);
					vga_buffer_put_str("\n");
				}
				else if (strcmp((string) token, "ticks") == 0)
				{
					vga_buffer_put_str("\n");
					vga_buffer_put_dec(get_tick());
					vga_buffer_put_str("\n");
				}
				// there's an issue w/ how i'm doing this.
				// if i don't include the new line statement then the terminal character ends up pushed over about
				// two tabs worth of space. if i leave the new line, then the terminal cursor is one line to low, and not at the top of the screen.
				else if (strcmp((string) token, "clear") == 0)
				{
					vga_buffer_put_str("\n");
					clear_screen();
				}
				// else if () {}
				// else if () {}
				else
				{
					vga_buffer_put_str("\n");
					vga_buffer_put_str("Unknown command.");
					vga_buffer_put_str("\n");
				}
				
				
				
				// clear the buffer.
				memset((u8int *) terminal_buffer, 0, MAX_TERMINAL_BUFFER_SIZE);
				terminal_buffer_length = 0;
				terminal_last_put = 0;
				
				// move the cursor to the next line, and reprint the terminal character
				//vga_buffer_put_str("\n");
				vga_buffer_put_char(terminal_seperator);
			}
			else if (terminal_buffer[terminal_buffer_length - 1] == '\b')
			{
				if (terminal_buffer_length > 0)
				{
					vga_buffer_put_str("\b \b");
					terminal_buffer_length = terminal_buffer_length - 2;
				}
				if (terminal_last_put > 0)
				{
					terminal_last_put--;
				}
			}
			else
			{
				// should i put the last character on the vga buffer?
				if (terminal_last_put < terminal_buffer_length)
				{
					vga_buffer_put_char(terminal_buffer[terminal_buffer_length - 1]);
					terminal_last_put++;
				}
			}

		}
		
		vga_flush();
	}
	
	return 0;
}

void kernel_keyboard_handler(u8int *buf, u16int size)
{
	for (int i = 0; i < size; i++)
		terminal_buffer[terminal_buffer_length++] = (char) buf[i];
}

void kernel_vga_handler(u8int *buf, u16int size)
{
	for (int i = 0; i < size; i++)
		put_char((char) buf[i]);
}
