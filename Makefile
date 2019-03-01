CFLAGS = -nostdlib -nostartfiles -ffreestanding -fno-builtin -O3 -Iinclude/ 
ASMSRC = $(wildcard *.asm)
ASMOBJ = $(ASMSRC:.asm=.o)
CSRC   = $(wildcard *.c)
COBJ   = $(CSRC:.c=.o)

all: out.exe
clean:
	rm *.exe *.bin *.o

out.exe: $(COBJ) $(ASMOBJ)
	mipsel-unknown-elf-gcc $(COBJ) $(ASMOBJ) $(CFLAGS) -fwhole-program -Wl,-Tlink.ld -o out.bin
	mipsel-unknown-elf-objcopy -j.text -Obinary out.bin out.exe

%.o : %.c
	mipsel-unknown-elf-gcc $^ -o $@ -c $(CFLAGS) -flto

%.o : %.asm
	mipsel-unknown-elf-as $^ -o $@ -msoft-float
