/* Registers */
#define CD_STATUS  ((volatile unsigned char *)(0x1F801800))
#define CD_COMMAND ((volatile unsigned char *)(0x1F801801))
#define CD_VOL_LL  ((volatile unsigned char *)(0x1F801802))
#define CD_VOL_LR  ((volatile unsigned char *)(0x1F801803))
#define CD_VOL_RR  ((volatile unsigned char *)(0x1F801801))
#define CD_VOL_RL  ((volatile unsigned char *)(0x1F801802))
#define CD_VOL_SET ((volatile unsigned char *)(0x1F801803))
#define CD_PARAMS  ((volatile unsigned char *)(0x1F801802))
#define CD_IF      ((volatile unsigned char *)(0x1F801803))
#define CD_FIFO    ((volatile unsigned char *)(0x1F801801))
#define CD_REQ     ((volatile unsigned char *)(0x1F801803))

enum CMD {
	CMD_SETLOC = 0x2,
	CMD_PLAY = 0x3,
	CMD_READN = 0x6,
	CMD_PAUSE = 0x9,
	CMD_INIT = 0xA,
	CMD_SETMODE = 0xE,
	CMD_GETTD = 0x14,
	CMD_SEEKL = 0x15
};

/* Parameters for SETMODE */
#define MODE_CCDA 0x1

/* Functions */
/* Wait for busy bit to unset */
#define CD_WAIT() while(*CD_STATUS & 0x80)

/* Wait for FIFO to empty */
#define CD_EAT_FIFO() while(*CD_STATUS & 0x20) *CD_FIFO

/* Wait for data FIFO */
#define CD_WAIT_DATA() while((*CD_STATUS & 0x40) == 0)

/* Select which registers are visible */
#define CD_SELECT(n) *CD_STATUS = (n)

/* Send command to CD-ROM */
#define CD_CMD(n) *CD_COMMAND = (n)

/* Set parameter to command */
#define CD_PARAM(n) *CD_PARAMS = (n)

/* Set volume, left to left/right, right to right/left */
#define CD_VOL(a, b, c, d) do { \
	*CD_VOL_LL = (a); \
	*CD_VOL_LR = (b); \
	CD_SELECT(3); \
	*CD_VOL_RR = (c); \
	*CD_VOL_RL = (d); \
	} while(0)

/* Commit volume changes */
#define CD_APPLY_VOLUME() *CD_VOL_SET = 0x20

void cdrom_callback(void);
void cdrom_read_sect(unsigned int, unsigned int, unsigned int);
extern char cd_buffer[];
