CFLAGS = -nostdlib -nostartfiles -ffreestanding -fno-builtin -O3 -Iinclude/ -march=3000 -mtune=3000 -msoft-float 
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

mkiso: out.exe
	cp out.exe fs/
	mkpsxiso psx.xml

cdemuiso: out.exe
	cp out.exe fs/
	mkisofs -o fs.iso fs/
	./iso2raw
	cdemu load 0 input.cue
	rm psx.toc
	cdrdao read-cd --device /dev/sr1 --read-raw --datafile psx.bin psx.toc
	toc2cue psx.toc psx.cue
	cdemu unload 0
