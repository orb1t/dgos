ENTRY(entry)

SECTIONS {
	.text 0x800 : AT(0x800) {
		__initialized_data_start = .;
		*(.head)
		*(.early)

		. = 0x1A0;
		SHORT(bootinfo_start);

		__ctors_start = .;
		*(.ctors)
		*(.ctors.*)
		*(.init_array)
		*(SORT(.init_array.*))
		__ctors_end = .;

		/* --- additional sectors --- */
		*(.text)
		*(.text.*)
		*(.text.unlikely)

		*(.data)
		*(.data.*)
		*(.rodata)
		*(.rodata.*)

		bootinfo_start = .;
		*(.bootinfo)

		/* sector align end */
		. = ALIGN(2048);
		__initialized_data_end = .;
	} = 0

	.bss : {
		__bss_start = .;
		*(.bss)
		*(.bss.*)
		*(COMMON)
		*(COMMON.*)
		*(COMMON*)

		. = ALIGN(16);
		__heap = .;
		. += 8;
		__bss_end = .;
	}

	/DISCARD/ : { *(.eh_frame) }

	__initial_stack = 0xfff0;
	__heap_end = 0xd000;
}
