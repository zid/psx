#include "cdrom.h"
#include "print.h"
#include "dma.h"

unsigned char cd_buffer[0x800];

static volatile int cdrom_data_ready;

void cdrom_dma(void)
{
	/* Clear interrupt bit */
	DICR = 0x880000;
	cdrom_data_ready = 1;
	printf("DMA IRQ\n");
}

void cdrom_callback(void)
{
	int prev_index, cdrom_status;

	prev_index = *CD_STATUS & 3;

	CD_SELECT(1);
	cdrom_status = *CD_IF & 7;

	printf("\nINT%d: Status: %02X, Response: %02X\n", cdrom_status, *CD_STATUS, *CD_RFIFO);

	/* Most things */
	if(cdrom_status == 3)
		goto out;

	/* Seek mainly */
	if(cdrom_status == 2)
		goto out;

	if(cdrom_status != 1)
		goto out;

	/* Tell the CD-ROM we're ready to recieve data */
	CD_SELECT(0);
	*CD_REQ = 0;
	*CD_REQ = 0x80;

	/* Lifted from bios */
	*((volatile unsigned int *)0xBF801018) = 0x20943;
	*((volatile unsigned int *)0xBF801020) = 0x132C;

	printf("\tChecking FIFO is ready...\n");
	/* Wait for data FIFO to have something in it */
	CD_WAIT_DATA();

	printf("\tStarting DMA\n");
	DMA3_MADR = (unsigned int)cd_buffer;
	DMA3_BCR  = 0x10200;
	DMA3_CHCR = 0x11000000; /* Start */
	printf("\tDMA sent.\n");
	for(int i = 0; i < 16; i++)
	{
		printf("%02X %02X %02X %02X %02X %02X %02X %02X\n",
			cd_buffer[i*8+0],
			cd_buffer[i*8+1],
			cd_buffer[i*8+2],
			cd_buffer[i*8+3],
			cd_buffer[i*8+4],
			cd_buffer[i*8+5],
			cd_buffer[i*8+6],
			cd_buffer[i*8+7]
		);
	}
	cdrom_data_ready = 1;
out:
	*CD_IF = 7;
	CD_SELECT(prev_index);
}

void cdrom_init(void)
{
	CD_WAIT();
	CD_SELECT(1);
	*CD_IE = 0;
	*CD_IF = 7;

	printf("cdrom: CMD_INIT\n");
	CD_WAIT();
	CD_SELECT(0);
	CD_CMD(CMD_INIT);

	printf("cdrom: Getstat\n");
	CD_WAIT();
	CD_SELECT(0);
	CD_CMD(1);

	printf("\tcdrom: Setting IE/IF\n");
	CD_WAIT();
	CD_SELECT(1);
	*CD_IE = 0x1F;
	*CD_IF = 7;

	printf("\tcdrom: Setmode 2x, 2k, pause, CDDA\n");
	CD_WAIT();
	CD_SELECT(0);
	CD_PARAM(0x83); /* Double speed, 2k sectors, autopause, cdda */
	CD_CMD(0x0E);

	printf("\tcdrom: Enabling CD Audio, setting SPU volume\n");
	/* Enable CD Audio */
	*(volatile unsigned short *)(0xBF801DAA) = 1 | 0xC000;
	/* CD audio volume */
	*(volatile unsigned   int *)(0xBF801DB0) = 0x40004000;
	/* Main volume */
	*(volatile unsigned short *)(0xBF801D80) = 0x3FFE;
	*(volatile unsigned short *)(0xBF801D82) = 0x3FFF;

	printf("\tcdrom: Setting stereo output, volume\n");
	/* Set CD-ROM to stereo, max volume */
	CD_WAIT();
	CD_SELECT(2);
	CD_VOL(128, 0, 128, 0);
	CD_APPLY_VOLUME();

	/* Unmask DMA3 IRQ */
	DICR = 0x880000;
}

void cdrom_read_sect(unsigned int min, unsigned int sec, unsigned int sect)
{
	CD_SELECT(0);

	printf("\tcdrom: Setloc\n");
	CD_WAIT();
	CD_PARAM(min);
	CD_PARAM(sec);
	CD_PARAM(sect);
	CD_CMD(2); /* Setloc */

	printf("\tcdrom: Seek\n");
	CD_WAIT();
	CD_CMD(0x15); /* Seek */

	printf("\tcdrom: ReadN\n");
	CD_WAIT();
	CD_CMD(0x6); /* ReadN */

	printf("\tcdrom: Waiting for cd-rom...\n");
	while(!cdrom_data_ready)
		;
	cdrom_data_ready = 0;

	printf("\tcdrom: Pause\n");
	CD_WAIT();
	CD_SELECT(0);
	CD_CMD(9);
}
