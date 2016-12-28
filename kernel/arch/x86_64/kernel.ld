ENTRY(entry)

SECTIONS {
	/*
	 * Total address space is 256TB
	 * 64TB of address space is reserved for user mode programs
	 * 64TB is reserved for the kernel
	 * This sidesteps any issues with dealing with canonical addresses
	 * By 2024 we should have about 1TB of ram
	 * By 2033 we might have a problem with 64TB limit
	 */

	/* Code and read-only data */
	.text 0x0000400000000000 : {
		PROVIDE(___text_st = .);
		*(.entry)

		. = ALIGN(64);
		*(.text.startup .text.startup.*)
		*(.text.hot .text.hot.*)

		. = ALIGN(64);
		*(.text)
		*(SORT(.text.*))

		. = ALIGN(64);
		*(.text.unlikely)

		. = ALIGN(64);
		PROVIDE(___init_st = .);
		*(.init_array)
		*(SORT(.init_array.*))
		PROVIDE(___init_en = .);

		PROVIDE(___fini_st = .);
		*(.fini_array)
		*(SORT(.fini_array.*))
		PROVIDE(___fini_en = .);

		. = ALIGN(32);

		PROVIDE(___callout_array_st = .);
		*(SORT(.callout_array.*))
		PROVIDE(___callout_array_en = .);

		. = ALIGN(64);
		*(.rodata)
		*(SORT(.rodata.*))

		*(.text .stub .text.* .gnu.linkonce.t.*)
		/* .gnu.warning sections are handled specially by elf32.em.  */
		*(.gnu.warning)

		/* Expose addresses/sizes to kernel as initialized data */

		/* Provide the highest address taken by kernel as
		   a global variable */
		. = ALIGN(8);
		___top_physaddr = .;
		QUAD((___init_brk - ___text_st) + 0x100000);

		/* Provide the TLS size as a global variable */
		___tls_size = .;
		QUAD(___tbss_en - ___tdata_st);

		/* Provide the TLS address as a global variable */
		___tls_init_data_ptr = .;
		QUAD(___tdata_st);

		/* Provide the initial TLS address
		   as a global variable */
		___tls_main_tls_bottom_ptr = .;
		QUAD(___main_tls_bottom);

		/* Provide the address of the main TEB
		   as a global variable */
		___main_teb_ptr = .;
		QUAD(___main_teb);

		PROVIDE(___text_en = .);
	} = 0xCC

	.eh_frame_hdr : ALIGN(4096) {
		*(.eh_frame_hdr)
		*(.eh_frame_entry .eh_frame_entry.*)
	}
	.eh_frame : ALIGN(4096) {
		KEEP(*(.eh_frame))
		*(.eh_frame.*)
	}

	/* Initialized data */
	.data  : ALIGN(4096) {
		*(.data)
		*(SORT(.data.*))

		/* Statically allocate room for the main */
		/* kernel thread TLS */
		. = ALIGN(16);
		PROVIDE(___main_tls_bottom = .);
		. += ___tbss_en - ___tdata_st;
		PROVIDE(___main_teb = .);
		/* 8 is sizeof thread_env_t! */
		QUAD(___main_teb);
		PROVIDE(___main_teb_end = .);
		. = ALIGN(16);
	} = 0x00

	/* Zero page pool initialized */
	.bss : ALIGN(4096) {
		PROVIDE(___bss_st = .);
		*(.bss)
		*(SORT(.bss.*))
		*(COMMON)
		PROVIDE(___bss_en = .);
	}

	.got : ALIGN(4096) {
		*(.got*)
		*(.got.plt)
	}

	.rela.dyn : {
		*(.rela.init)
		*(.rela.text .rela.text.* .rela.gnu.linkonce.t.*)
		*(.rela.fini)
		*(.rela.rodata .rela.rodata.* .rela.gnu.linkonce.r.*)
		*(.rela.data .rela.data.* .rela.gnu.linkonce.d.*)
		*(.rela.tdata .rela.tdata.* .rela.gnu.linkonce.td.*)
		*(.rela.tbss .rela.tbss.* .rela.gnu.linkonce.tb.*)
		*(.rela.ctors)
		*(.rela.dtors)
		*(.rela.got)
		*(.rela.bss .rela.bss.* .rela.gnu.linkonce.b.*)
		*(.rela.ldata .rela.ldata.* .rela.gnu.linkonce.l.*)
		*(.rela.lbss .rela.lbss.* .rela.gnu.linkonce.lb.*)
		*(.rela.lrodata .rela.lrodata.* .rela.gnu.linkonce.lr.*)
		*(.rela.ifunc)
	}

	.rela.plt : {
		*(.rela.plt)
		PROVIDE_HIDDEN (__rela_iplt_start = .);
		*(.rela.iplt)
		PROVIDE_HIDDEN (__rela_iplt_end = .);
	}

	.plt : {
		*(.plt)
		*(.iplt)
	}

	.plt.got : {
		*(.plt.got)
	}

	.plt.bnd : {
		*(.plt.bnd)
	}

	. = ALIGN(4096);

	/* Copy on write initialized thread_local */
	.tdata : {
		PROVIDE(___tdata_st = .);
		*(.tdata)
		. = ALIGN(16);
		PROVIDE(___tdata_en = .);
	}

	/* Zero page pool initialized thread_local */
	.tbss : {
		PROVIDE(___tbss_st = .);
		*(.tbss)
		*(.tcommon)
		. = ALIGN(16);
		PROVIDE(___tbss_en = .);
	}

	. = ALIGN(4096);
	PROVIDE(___init_brk = .);

	.debug_info    0 : { *(.debug_info)    }
	.debug_abbrev  0 : { *(.debug_abbrev)  }
	.debug_aranges 0 : { *(.debug_aranges) }
	.debug_macro   0 : { *(.debug_macro)   }
	.debug_line    0 : { *(.debug_line)    }
	.debug_str     0 : { *(.debug_str)     }
	.comment       0 : { *(.comment)       }
}