CFLAGS = -ffreestanding -fno-builtin -flto -O3 -Iinclude/ -nostartfiles
ASMSRC = $(wildcard *.asm)
ASMOBJ = $(ASMSRC:.asm=.o)
CSRC   = $(wildcard *.c)
COBJ   = $(CSRC:.c=.o)

all: out.exe
clean:
	rm *.exe *.bin *.o

out.exe: $(COBJ) $(ASMOBJ)
	mipsel-unknown-elf-gcc $(COBJ) $(ASMOBJ) -Wl,-Tlink.ld -o out.bin -fwhole-program
	mipsel-unknown-elf-objcopy -j.text -Obinary out.bin out.exe

%.o : %.c
	mipsel-unknown-elf-gcc $^ -o $@ -c $(CFLAGS)

%.o : %.asm
	mipsel-unknown-elf-as $^ -o $@ -msoft-float
