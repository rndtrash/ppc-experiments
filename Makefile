SOURCES_C := src/main.c
SOURCES_AS := src/ofwmagic.s
OBJECTS := $(SOURCES_C:src/%.c=bin/%.c.o) $(SOURCES_AS:src/%.s=bin/%.s.o)

RELOC := E00000

CFLAGS := -target powerpc-none-eabi -static -msoft-float -ffreestanding
ASFLAGS := -filetype=obj --arch=ppc32
LDFLAGS := -N --Bstatic -nostdlib -e _start -Ttext ${RELOC} -v

.PHONY: clean qemu

all: bin/hellorld.iso

bin/hellorld.iso: bin/boot.elf boot.tbxi bootinfo.txt hfs.map
	#dd if=/dev/zero of=bin/hellorld.dmg bs=1M count=16 status=progress
	##parted bin/hellorld.dmg --script mklabel mac mkpart primary hfs 2048s 100%
	#mkdir -p dmg_mount
	##sudo ./kpartx.sh
	#sudo mkfs.hfs -v Hellorld bin/hellorld.dmg
	#sudo mount -o loop bin/hellorld.dmg ./dmg_mount/
	#sudo mkdir -p ./dmg_mount/ppc
	#sudo cp bootinfo.txt ./dmg_mount/ppc
	#sudo cp bin/boot.elf ./dmg_mount/ppc
	#sudo umount ./dmg_mount/
	#sudo kpartx -d bin/hellorld.dmg
	mkdir -p ./dmg_mount/ppc
	cp bootinfo.txt boot.tbxi bin/boot.elf ./dmg_mount/ppc
	genisoimage \
		-joliet-long -r \
		-V 'Hellorld' \
		-o bin/hellorld.iso \
		--iso-level 4 \
		--netatalk -hfs -probe \
		-map hfs.map \
		-hfs-parms MAX_XTCSIZE=2656248 \
		--chrp-boot \
		-part -no-desktop \
		-hfs-bless ppc \
		-hfs-volid Hellorld_boot \
		./dmg_mount/
	

bin/boot.elf: $(OBJECTS)
	ld.lld $(OBJECTS) $(LDFLAGS) -o bin/boot.elf

bin/%.c.o: src/%.c Makefile
	clang $< $(CFLAGS) -c -o $@

bin/%.s.o: src/%.s Makefile
	llvm-mc $< $(ASFLAGS) -o $@

clean:
	rm -f bin/hellorld.dmg
	rm -f bin/*.o

qemu: bin/hellorld.iso
	qemu-system-ppc -L openbios-ppc -boot d -M mac99 -m 256 -cdrom bin/hellorld.iso -device VGA,edid=on
