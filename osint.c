#include <sys/types.h>
#include <psxgpu.h>
#include <psxcd.h>
#include <psxapi.h>

#include <stdio.h>
#include <string.h>

#include "osint.h"
#include "e8910.h"
#include "vecx.h"
#include "padEx.h"

#define CD_SECTOR_SIZE  2048
#define BtoS(len) ( ( len + CD_SECTOR_SIZE - 1 ) / CD_SECTOR_SIZE )


static DRAWENV draw;
static DISPENV disp;
static uint8_t pad_buff[2][34];
static uint8_t EMU_TIMER = 20; /* the emulators heart beats at 20 milliseconds */
char	pribuff[2][65536];		/* Primitive packet buffers */
uint32_t	ot[2][OT_LEN];			/* Ordering tables */
char* nextpri;				/* Pointer to next packet buffer offset */
int		db = 0;					/* Double buffer index */

static long scl_factor;
static long offx;
static long offy;

void osint_render(void){
	LINE_F2* line;

	/* Clear ordering table and set start address of primitive */
	/* buffer for next frame */
	ClearOTagR(ot[db], OT_LEN);
	nextpri = pribuff[db];

	int v;
	for(v = 0; v < vector_draw_cnt; v++){
		line = (LINE_F2*)nextpri;
		uint8_t c = vectors_draw[v].color * 256 / VECTREX_COLORS;

		setRGB0(line, c, c, c);
		setXY2(line, offx + vectors_draw[v].x0 / scl_factor, offy + vectors_draw[v].y0 / scl_factor, offx + vectors_draw[v].x1 / scl_factor, offy + vectors_draw[v].y1 / scl_factor);
	}

}

static char *romfilename = "\\ROM.DAT;1";
static char *cartfilename = NULL;

static void init(){
	CdInit();

	CdlFILE file;
	if (!CdSearchFile(&file, romfilename))
	{
		// Return value is NULL, file is not found
		printf("Rom Not found!\n");
		return;
	}
	CdControl(CdlSetloc, (unsigned char*)&file.pos, 0);
	CdRead((int)BtoS(file.size), (uint32_t*)rom, CdlModeSpeed);
	CdReadSync(0, 0);

	printf("ROM read success!");

	memset(cart, 0, sizeof (cart));
	if(cartfilename){
		CdlFILE cartFile;
		if (!CdSearchFile(&cartFile, cartfilename))
		{
			// Return value is NULL, file is not found
			printf("Cart Not found!\n");
			return;
		}
		CdControl(CdlSetloc, (unsigned char*)&cartFile.pos, 0);
		CdRead((int)BtoS(file.size), (uint32_t*)cart, CdlModeSpeed);
		CdReadSync(0, 0);

		printf("Cart read success!");
	}
}

void resize(int width, int height){
	long sclx, scly;

	long screenx = width;
	long screeny = height;
	
	ResetGraph(0);

	SetDefDrawEnv(&draw, 0, 0, width, height);
	SetDefDispEnv(&disp, 0, 0, width, height);
	disp.isinter = 1;

	/* Set clear color, area clear and dither processing */
	setRGB0(&draw, 0, 0, 0);
	draw.isbg = 1;
	draw.dtd = 1;

	/* Apply the display and drawing environments */
	PutDispEnv(&disp);
	PutDrawEnv(&draw);

	/* Enable video output */
	SetDispMask(1);

	sclx = ALG_MAX_X / 640;
	scly = ALG_MAX_Y / 480;

	scl_factor = sclx > scly ? sclx : scly;

	offx = (screenx - ALG_MAX_X / scl_factor) / 2;
	offy = (screeny - ALG_MAX_Y / scl_factor) / 2;

	InitPAD(&pad_buff[0][0], 34, &pad_buff[1][0], 34);
	StartPAD();
	ChangeClearPAD(0);

	if (GetVideoMode() == MODE_NTSC) {
		EMU_TIMER = 16;
	}
	else {
		EMU_TIMER = 20; // PAL will have more accurate timing
	}
}

static void readevents(){
	PADTYPE* pad1 = (PADTYPE*)&pad_buff[0][0];

	if (pad1->stat == 0) {
		if ((pad1->type == 0x4) || (pad1->type == 0x5) || (pad1->type == 0x7)) {

			// Button 1
			if (!(pad1->btn & PAD_TRIANGLE)) {
				snd_regs[14] &= ~0x01;
			}
			else {
				snd_regs[14] |= 0x01;
			}

			// Button 2
			if (!(pad1->btn & PAD_CIRCLE)) {
				snd_regs[14] &= ~0x02;
			}
			else
			{
				snd_regs[14] |= 0x02;
			}

			// Button 3
			if (!(pad1->btn & PAD_SQUARE)) {
				snd_regs[14] &= ~0x04;
			}
			else {
				snd_regs[14] |= 0x04;
			}

			// Button 4
			if (!(pad1->btn & PAD_CROSS)) {
				snd_regs[14] &= ~0x08;
			}
			else {
				snd_regs[14] |= 0x08;
			}

			// Y-Axis on joystick
			if (!(pad1->btn & PAD_UP)) {
				alg_jch1 = 0xff;
			}
			else if(!(pad1->btn & PAD_DOWN))
			{
				alg_jch1 = 0x00;
			}
			else {
				alg_jch1 = 0x80;
			}

			// X-Axis on joystick
			if (!(pad1->btn & PAD_LEFT)) {
				alg_jch0 = 0x00;
			}
			else if (!(pad1->btn & PAD_RIGHT)) {
				alg_jch0 = 0xff;
			}
			else {
				alg_jch0 = 0x80;
			}
		}
	}
}

void osint_emuloop(){
	vecx_reset();
	for(;;){
		vecx_emu((VECTREX_MHZ / 1000) * EMU_TIMER);
		readevents();

		DrawSync(0);
		VSync(0);

		/* Since draw.isbg is non-zero this clears the screen */
		PutDrawEnv(&draw);

		/* Begin drawing the new frame */
		DrawOTag(ot[db] + (OT_LEN - 1));

		/* Alternate to the next buffer */
		db = !db;
	}
}

void load_overlay(const char *filename){
	
}

void main(){
	printf("Hello");

	resize(640, 480);

	cartfilename = "\\MINE.VEC;1";

	init();

	e8910_init_sound();
	osint_emuloop();
	e8910_done_sound();
}