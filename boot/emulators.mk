run-iso-singlecpu: $(ISO_FILE)
	$(QEMU) $(QEMU_FLAGS) -m $(QEMU_RAM) -s -cdrom $(ISO_FILE) \
		$(QEMU_DEBUGCON) \
		$(QEMU_NET)

run-iso: $(ISO_FILE)
	$(QEMU) -m $(QEMU_RAM) -s -cdrom $(ISO_FILE) \
		-machine $(QEMU_MACHINE) -cpu $(QEMU_CPU) \
		$(QEMU_DEBUGCON) \
		$(QEMU_SMP) \
		$(QEMU_NET)

run-iso-singlecpu-kvm: $(ISO_FILE)
	$(QEMU) $(QEMU_FLAGS) -m $(QEMU_RAM) -s -cdrom $(ISO_FILE) \
		$(QEMU_DEBUGCON) \
		$(QEMU_NET) \
		-cpu host -enable-kvm

run-iso-kvm: $(ISO_FILE)
	$(QEMU) $(QEMU_FLAGS) -m $(QEMU_RAM) -s -cdrom $(ISO_FILE) \
		-machine $(QEMU_MACHINE) \
		$(QEMU_DEBUGCON) \
		$(QEMU_SMP) \
		$(QEMU_NET) \
		-cpu host -enable-kvm

run-iso-numa-kvm: $(ISO_FILE)
	$(QEMU) $(QEMU_FLAGS) -m $(QEMU_RAM) -s -cdrom $(ISO_FILE) \
		-machine $(QEMU_MACHINE) \
		$(QEMU_DEBUGCON) \
		$(QEMU_SMP) \
		-numa node,mem=256M,cpus=0-1 \
		-numa node,mem=256M,cpus=2-3 \
		$(QEMU_NET) \
		-cpu host -enable-kvm

run-iso-numa: $(ISO_FILE)
	$(QEMU) $(QEMU_FLAGS) -m $(QEMU_RAM) -s -cdrom $(ISO_FILE) \
		-machine $(QEMU_MACHINE) -cpu $(QEMU_CPU) \
		$(QEMU_DEBUGCON) \
		$(QEMU_SMP) \
		-numa node,mem=256M,cpus=0-1 \
		-numa node,mem=256M,cpus=2-3 \
		$(QEMU_NET)

debug-iso-boot: $(ISO_FILE)
	 $(GDB) $(SYMBOLFILE) --tui \
		-ex 'b elf64_run' \
		-ex 'target remote | exec $(QEMU) $(QEMU_FLAGS) \
			-machine $(QEMU_MACHINE) -cpu $(QEMU_CPU) \
			-m $(QEMU_RAM) -S -cdrom $(ISO_FILE) \
			-gdb stdio \
			$(QEMU_NET)'

debug-iso-kernel: $(ISO_FILE)
	$(GDB) --symbols $(KERNELSYMBOLFILE) --tui \
		-ex 'b entry' \
		-ex 'target remote | exec $(QEMU) $(QEMU_FLAGS) \
			-machine $(QEMU_MACHINE) -cpu $(QEMU_CPU) \
			-m $(QEMU_RAM) -S -cdrom $(ISO_FILE) \
			-gdb stdio \
			$(QEMU_NET)'

debug-iso-wait-singlecpu: $(ISO_FILE)
	 $(QEMU) $(QEMU_FLAGS) \
		 -machine $(QEMU_MACHINE) -cpu $(QEMU_CPU) \
		 -m $(QEMU_RAM) -S -cdrom $(ISO_FILE) -s -S \
		 $(QEMU_DEBUGCON) \
		 $(QEMU_NET)

debug-iso-wait-singlecpu-ide: $(ISO_FILE)
	 $(QEMU) $(QEMU_FLAGS) \
		 -cpu $(QEMU_CPU) \
		 -m $(QEMU_RAM) -S -cdrom $(ISO_FILE) -s -S \
		 $(QEMU_DEBUGCON) \
		 $(QEMU_NET)

debug-iso-wait-singlecpu-kvm: $(ISO_FILE)
	 $(QEMU) $(QEMU_FLAGS) \
		 -machine $(QEMU_MACHINE) \
		 -m $(QEMU_RAM) -S -cdrom $(ISO_FILE) -s -S \
		 $(QEMU_DEBUGCON) \
		 $(QEMU_NET) \
		 -cpu host -enable-kvm

bin/mem-garbage:
	dd if=/dev/urandom of=bin/mem-garbage bs=1048576 count=512

debug-iso-wait: $(ISO_FILE) #bin/mem-garbage
	 $(QEMU) $(QEMU_FLAGS) \
		 -machine $(QEMU_MACHINE) -cpu $(QEMU_CPU) \
		 $(QEMU_SMP) \
		 -m $(QEMU_RAM) -S -cdrom $(ISO_FILE) -s -S \
		 $(QEMU_DEBUGCON) \
		 $(QEMU_NET)

#		 -object memory-backend-file,id=mem,size=512M,mem-path=bin/mem-garbage,share=on \
#		 -numa node,memdev=mem

debug-iso-wait-many-ahci: $(ISO_FILE)
	 $(QEMU) $(QEMU_FLAGS) \
		 -machine $(QEMU_MACHINE) -cpu $(QEMU_CPU) \
		 $(QEMU_SMP) \
		 -m $(QEMU_RAM) -S -cdrom $(ISO_FILE) -s -S \
		 $(QEMU_DEBUGCON) \
		 -device ich9-ahci,id=ahci2 \
		 -device ich9-ahci,id=ahci3 \
		 -device ich9-ahci,id=ahci4

debug-iso-bridge-test-wait: $(ISO_FILE)
	 $(QEMU) $(QEMU_FLAGS) \
		 -machine $(QEMU_MACHINE) -cpu $(QEMU_CPU) \
		 $(QEMU_SMP) \
		 -m $(QEMU_RAM) -S -cdrom $(ISO_FILE) -s -S \
		 $(QEMU_DEBUGCON) \
		 -device i82801b11-bridge,bus=pcie.0,id=pcie.1 \
		 -device i82801b11-bridge,bus=pcie.1,id=pcie.2 \
		 -device i82801b11-bridge,bus=pcie.2,id=pcie.3 \
		 -device i82801b11-bridge,bus=pcie.3,id=pcie.4 \
		 -device i82801b11-bridge,bus=pcie.4,id=pcie.5 \
		 -device i82801b11-bridge,bus=pcie.5,id=pcie.6 \
		 -device ich9-ahci,bus=pcie.1,id=ahci2 \
		 -device ich9-ahci,bus=pcie.2,id=ahci3 \
		 -device ich9-ahci,bus=pcie.2,id=ahci4 \
		 -device ich9-ahci,bus=pcie.4,id=ahci5 \
		 -device ich9-ahci,bus=pcie.5,id=ahci6 \
		 -device ich9-ahci,bus=pcie.6,id=ahci7 \
		 -device ich9-ahci,bus=pcie.6,id=ahci8

debug-iso-wait-kvm: $(ISO_FILE)
	 $(QEMU) $(QEMU_FLAGS) \
		 -machine $(QEMU_MACHINE) \
		 $(QEMU_SMP) \
		 $(QEMU_DEBUGCON) \
		 -m $(QEMU_RAM) -S -cdrom $(ISO_FILE) -s -S \
		 -cpu host -enable-kvm

#
# Debug in KVM

$(BINDIR)/debug-kvm-gdbcommands: utils/debug-kvm-commands.template
	$(SED) "s|\$$SYMBOLFILE|$(SYMBOLFILE)|g" $< | \
		$(SED) "s|\$$DISKIMAGE|$(DISKIMAGE)|g" > $@

$(BINDIR)/debug-kernel-kvm-gdbcommands: utils/debug-kernel-kvm-commands.template
	$(SED) "s|\$$SYMBOLFILE|$(KERNELSYMBOLFILE)|g" $< | \
		$(SED) "s|\$$DISKIMAGE|$(DISKIMAGE)|g" > $@

debug: all debuggable-disk $(BINDIR)/debug-kvm-gdbcommands
	$(GDB) -x $(BINDIR)/debug-kvm-gdbcommands

debug-kernel: all $(BINDIR)/debug-kernel-kvm-gdbcommands
	$(GDB) -x $(BINDIR)/debug-kernel-kvm-gdbcommands

debug-attach:
	$(GDB) --symbols=$(KERNELSYMBOLFILE) --tui \
		-iex 'set architecture i386:x86-64' \
		-ex 'target remote localhost:1234' \
		-ex 'layout regs' \
		-ex 'set radix 16' \
		-ex 'b assert_failed' \
		-ex 'b unhandled_exception_handler'

debug-boot-attach:
	$(GDB) $(SYMBOLFILE) --tui \
		-iex 'set architecture i8086' \
		-ex 'target remote localhost:1234' \
		-ex 'layout regs' \
		-ex 'set radix 16' \
		-ex 'b *0x7C00'

run-debug: all bootable-disk debuggable-kernel-disk inject-kernel
	$(QEMU) \
		-drive file=$(DISKIMAGE),format=raw \
		-no-shutdown -no-reboot -d cpu_reset \
		-machine $(QEMU_MACHINE) -cpu $(QEMU_CPU) \
		-s -m $(QEMU_RAM) \
		$(QEMU_SMP) \
		$(QEMU_DEBUGCON) \
		$(QEMU_NET)

# -drive file=$(DISKIMAGE),format=raw,if=ide

run-debug-singlecpu: all debuggable-kernel-disk
	$(QEMU) -s \
		-drive file=$(DISKIMAGE),format=raw \
		-no-shutdown -no-reboot -d cpu_reset \
		-machine $(QEMU_MACHINE) -cpu $(QEMU_CPU) \
		-m $(QEMU_RAM) \
		$(QEMU_DEBUGCON) \
		$(QEMU_NET)

#		-drive id=disk,file=$(DISKIMAGE),if=none \
#		-device ahci,id=ahci \
#		-device ide-drive,drive=disk,bus=ahci.0 \

run-debug-wait: all debuggable-kernel-disk
	$(QEMU) -s -S  \
		-drive file=$(DISKIMAGE),format=raw \
		-no-shutdown -no-reboot -d cpu_reset,guest_errors \
		-machine $(QEMU_MACHINE) -cpu $(QEMU_CPU) \
		-m $(QEMU_RAM) \
		$(QEMU_SMP) \
		$(QEMU_DEBUGCON) \
		$(QEMU_NET)

run-debug-wait-kvm: all debuggable-kernel-disk
	$(QEMU) -s -S  \
		-drive file=$(DISKIMAGE),format=raw \
		-no-shutdown -no-reboot -d cpu_reset,guest_errors \
		-machine $(QEMU_MACHINE) \
		-m $(QEMU_RAM) \
		$(QEMU_SMP) \
		$(QEMU_DEBUGCON) \
		$(QEMU_NET) \
		-cpu host -enable-kvm

run-debug-wait-singlecpu: all debuggable-kernel-disk
	$(QEMU) -s -S  \
		-drive file=$(DISKIMAGE),format=raw \
		-no-shutdown -no-reboot -d cpu_reset,guest_errors \
		-machine $(QEMU_MACHINE) -cpu $(QEMU_CPU) \
		-m $(QEMU_RAM) \
		$(QEMU_DEBUGCON) \
		$(QEMU_NET)

run-debug-wait-singlecpu-kvm: all debuggable-kernel-disk
	$(QEMU) -s -S  \
		-drive file=$(DISKIMAGE),format=raw \
		-no-shutdown -no-reboot -d cpu_reset,guest_errors \
		-machine $(QEMU_MACHINE) \
		-m $(QEMU_RAM) \
		$(QEMU_DEBUGCON) \
		$(QEMU_NET) \
		-cpu host -enable-kvm

monitor-debug-output:
	while true; do cat dump/qemu-debug-out; done

run: all bootable-disk
	$(QEMU) -m 5G -drive file=$(DISKIMAGE),format=raw \
		$(QEMU_NET)

#		-netdev user,id=mynet0,net=192.168.66.0/24,dhcpstart=192.168.66.9 \
#		-net nic,model=i82551 \
#		-net nic,model=i82557b \
#		-net nic,model=i82559er \
#		-net nic,model=pcnet \
#		-net nic,model=smc91c111 \
#		-net nic,model=lance \
#		-net nic,model=ne2k_isa \
#		-net nic,model=mcf_fec

#
# Debug in bochs

bochs-symbols: $(BOCHSCOMBINEDSYMBOLS)

$(BOCHSCOMBINEDSYMBOLS): $(BOCHSKERNELSYMBOLS) $(BOCHSSYMBOLS)
	$(CAT) $^ > $@

$(BOCHSKERNELSYMBOLS): kernel
	$(OBJDUMP) --wide --syms ../kernel/bin/kernel | \
		$(GREP) -P '^[0-9A-Fa-f]+\s.*\s[a-zA-Z_][a-zA-Z0-9_]+$$' | \
		$(SED) -r 's/^(\S+)\s+.*\s+(\S+)$$/\1 \2/' | \
		$(SORT) > $@

$(BOCHSSYMBOLS): $(SYMBOLFILE)
	$(OBJDUMP) --wide --syms $^ | \
		$(GREP) -P '^[0-9A-Fa-f]+\s.*\s[a-zA-Z_][a-zA-Z0-9_]+$$' | \
		$(SED) -r 's/^(\S+)\s+.*\s+(\S+)$$/\1 \2/' | \
		$(SORT) > $@

$(BINDIR)/bochs-config.bxrc: utils/bochs-config.bxrc.template
	$(SED) "s|\$$SYMBOLFILE|$(SYMBOLFILE)|g" $< | \
		$(SED) "s|\$$DISKIMAGE|$(DISKIMAGE)|g" > $@

debug-bochs: all bochs-symbols iso $(BINDIR)/bochs-config.bxrc
	$(BOCHS) \
		-qf $(BINDIR)/bochs-config.bxrc \
		-rc utils/bochs-debugger-commands

debug-bochs-boot: all bochs-symbols iso $(BINDIR)/bochs-config.bxrc
	$(BOCHS) \
		-qf $(BINDIR)/bochs-config.bxrc \
		-rc utils/bochs-debugger-boot-commands

run-bochs: all bochs-symbols debuggable-disk $(BINDIR)/bochs-config.bxrc
	$(BOCHS) -qf $(BINDIR)/bochs-config.bxrc -q