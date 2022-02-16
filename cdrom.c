#include "cdrom.h"
#include "print.h"

unsigned char cd_buffer[0x800];

static volatile int cdrom_status = 0;

void cdrom_callback()
{
	int prev_index, i;
	printf("IRQ: cdrom\n");
	prev_index = *CD_STATUS & 3;

	CD_SELECT(1);
	cdrom_status = *CD_IF & 7;

	/* Just discard response FIFO for now */
	CD_EAT_FIFO();

	/* Most things */
	if(cdrom_status == 3)
		goto out;

	/* Seek mainly */
	if(cdrom_status == 2)
		goto out;

	if(cdrom_status != 1)
		goto out;

	/* Sector has arrived */

	/* Ack the status */
	CD_SELECT(1);
	*CD_IF = 7;

	CD_SELECT(0);

	/* Tell the CD-ROM we're ready to recieve data */
	*CD_REQ = 0;
	*CD_REQ = 0x80;

	/* Wait for data FIFO to have something in it */
	CD_WAIT_DATA();

	for(i = 0; i < 0x800; i++)
		cd_buffer[i] = *((volatile unsigned char *)(0x1F801802));

	/* Restore saved register select */
	CD_SELECT(prev_index);
	return;

out:
	*CD_IF = 7;
	CD_SELECT(prev_index);
}

void cdrom_init(void)
{
	printf("\tcdrom: CMD_INIT\n");
	CD_WAIT();
	CD_SELECT(0);
	CD_CMD(CMD_INIT);

	printf("\tcdrom: Resetting IF\n");
	CD_WAIT();
	CD_SELECT(1);
	*CD_IF = 0x1F;

	printf("\tcdrom: Enabling CD Audio, setting SPU volume\n");
	/* Enable CD Audio */
	*(volatile unsigned short *)(0x1F801DAA) = 1 | 0xC000;
	/* CD audio volume */
	*(volatile unsigned   int *)(0x1F801DB0) = 0x40004000;
	/* Main volume */
	*(volatile unsigned short *)(0x1F801D80) = 0x3FFE;
	*(volatile unsigned short *)(0x1F801D82) = 0x3FFF;

	printf("\tcdrom: Setting stereo output, volume\n");
	/* Set CD-ROM to stereo, max volume */
	CD_WAIT();
	CD_SELECT(2);
	CD_VOL(128, 0, 128, 0);
	CD_APPLY_VOLUME();
}

void cdrom_read_sect(unsigned int min, unsigned int sec, unsigned int sect)
{
	CD_SELECT(0);

	CD_WAIT();
	CD_PARAM(min);
	CD_PARAM(sec);
	CD_PARAM(sect);
	CD_CMD(2); /* Setloc */

	CD_WAIT();
	CD_CMD(0x15); /* Seek */

	CD_WAIT();
	CD_CMD(0x6); /* ReadN */

	while(cdrom_status != 1)
		;
	cdrom_status = 0;

	CD_WAIT();
	CD_SELECT(0);
	CD_CMD(9);
}
