AS = mipsel-unknown-elf-as
OBJCOPY = mipsel-unknown-elf-objcopy
CC = mipsel-unknown-elf-gcc
CFLAGS = -W -Wall -nostdlib -nostartfiles -ffreestanding -fno-builtin -O3 -Iinclude/ -march=3000 -mtune=3000 -msoft-float -mno-gpopt -g
ASMSRC = $(wildcard *.asm)
ASMOBJ = $(ASMSRC:.asm=.o)
CSRC   = $(wildcard *.c)
COBJ   = $(CSRC:.c=.o)

all: psx.bin

psx.bin: out.exe
	cp out.exe fs/
	yes | ./mkiso psx.xml
       
install: psx.bin
	cp psx.cue psx.bin /mnt/samba

clean:
	rm *.exe *.bin *.o

out.bin: $(COBJ) $(ASMOBJ)
	$(CC) $(COBJ) $(ASMOBJ) $(CFLAGS) -Wl,-Tlink.ld -o out.bin -Wl,-Map,map.txt

out.exe: out.bin
	$(OBJCOPY) -j.text -Obinary out.bin out.exe

%.o : %.c
	$(CC) $^ -o $@ -c $(CFLAGS)

%.o : %.asm
	$(AS) $^ -o $@ -msoft-float

mkiso: out.exe
	cp out.exe fs/
	./mkiso psx.xml

zero: zero.bin
	dd if=zero.bin of=fs/zero2.bmp skip=54 bs=1i

zero.bin:
	convert zero2.bmp -resize 128x128 -separate -swap 0,2 -combine -flip -define bmp:subtype=RGB555 zero.bin
