
#include "uart_cmd_parser.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "stdarg.h"
#include "system.h"
#include "UART_DBG.h"

//#define putsUSART(x)	HAL_UART_Transmit(&huart1, x, strlen(x), 0xFFFF);


#define home() 										putsUSART("\33[H") //Move cursor to the indicated row, column (origin at 1,1)
#define clrscr()									putsUSART("\33[2J") //clear the screen, move to (1,1)
#define gotoxy(x,y)								sprintf(buf, "\33[%d;%dH", y, x); putsUSART(buf);
#define invisible_cursor() 				putsUSART("\33[?25l");
#define visible_cursor()					putsUSART("\33[?25l");
#define set_display_atrib(color) 	sprintf(buf,"\33[%dm",color); putsUSART(buf);
#define resetcolor() 							printf("\33[0m");



void printBoolXY(_Bool b, char x, char y);
void printValXY(unsigned int b, char x, char y);
//void clear_val(void);

void UART_debug_print(void) {
    static _Bool init = 0;
    char buf[10] = "";
    // static struct SystemFlags SF;
    //char x;
    if (!init) {
        frame_draw();
        init = 1;
        //        SF.temp = sys_state.temp;
        //        SF.set_temp = sys_state.set_temp;
        //        SF.temp_ctrl = sys_state.temp_ctrl;
        //        SF.moto_step_set = sys_state.moto_step_set;
        //        SF.moto_position = sys_state.moto_position;
        //        SF.moto_destination_pos = sys_state.moto_destination_pos;
        //        SF.moto_max_position = sys_state.moto_max_position;
        //
        //        SF.moto_run = sys_state.moto_run;
        //        SF.moto_stop_request = sys_state.moto_stop_request;
        //        SF.view_mode = sys_state.view_mode;
        //        SF.temp_update = sys_state.temp_update;
        //        SF.blinken = sys_state.blinken;
        //        SF.update_display = sys_state.update_display;
        //        SF.uart_init_flag = sys_state.uart_init_flag;
        //        SF.uart_debug = sys_state.uart_debug;
        //        SF.ERR_FLAG = sys_state.ERR_FLAG;

    }

    //clear_val();
    //   TEMP
    printValXY(DS_TEMP, 9, 3);
    printValXY(SysState.set_temp, 9, 4);
    printBoolXY(SysState.temp_ctrl_f, 12, 5);

    //MOTO
    printValXY(drv_m1.steps, 10, 8);
    printValXY(drv_m1.position, 10, 9);
    printValXY(drv_m1.dest_pos, 10, 10);
    printValXY(drv_m1.max_pos, 10, 11);

//    printBoolXY(MOTO_HOME_SENS_PORT, 12, 12);
//    printBoolXY(S1_PORT, 12, 13); //ENT
//    printBoolXY(S2_PORT, 12, 14); //Up
//    printBoolXY(S3_PORT, 12, 15); //Down
    //FLAGS

    printBoolXY( drv_m1.run_f, 24, 3);
    printBoolXY(drv_m1.stop_req_f, 24, 4);
    //printBoolXY(MOTO_DRV_PORT, 24, 5);
    //printBoolXY(MOTO_DIR_PORT, 24, 6);
    printBoolXY(SysState.view_mode_f, 24, 7);
    printBoolXY((SysCnt.temp_update > SysState.t_updt_time) ? 1 : 0, 24, 8);
    printBoolXY(SysState.ss_blink_f, 24, 9);
    printBoolXY(SysState.ss_update_f, 24, 10);
    //printBoolXY(sys_state.uart_init_flag, 24, 9);
    //printBoolXY(sys_state.uart_debug, 24, 10);
    printBoolXY(SysState.error_f, 24, 11);

    gotoxy(14, 13);
    if (SysState.error_code & 0x01) {
        set_display_atrib(B_RED);

    } else {
        set_display_atrib(B_GREEN);
    }
    putsUSART(" TSens ERR ");

    gotoxy(14, 15);
    if ((SysState.error_code & 0x02) | (SysState.error_code & 0x04)) {
        set_display_atrib(B_RED);

    } else {
        set_display_atrib(B_GREEN);
    }
    putsUSART(" MOTO  ERR ");
    set_display_atrib(B_BLUE);
    resetcolor();


}

void frame_draw(void) {
    char buf[10] = "";
    home();
    clrscr();
    set_display_atrib(B_BLUE);
    invisible_cursor();
    putsUSART(
            " |----------|-----------|\r\n"
            " |  Temp    | Flags     |\r\n"
            " |Temp     C|Mrun      x|\r\n"
            " |Tset     C|Mstop req x|\r\n"
            " |Tctrl    x|MDRV      x|\r\n"
            " |          |MDIR      x|\r\n"
            " |  Moto    |View Mode x|\r\n"
            " |Step      |TempUpdt  x|\r\n"
            " |Pos       |Blinken   x|\r\n"
            " |DestP     |DispUpdt  x|\r\n"
            " |MaxP      |Error     x|\r\n"
            " |ESens    x|-----------|\r\n"
            " |BtnENT   x| TSens ERR |\r\n"
            " |BtnUP    x|-----------|\r\n"
            " |BtnDOWN  x| MOTO  ERR |\r\n"
            " |----------------------|\r\n"
            " |                      |\r\n"
            " |----------------------|\r\n"

            );
    resetcolor();


}

void printBoolXY(_Bool b, char x, char y) {
    char buf[10] = "";
    resetcolor();
    gotoxy(x, y);
    if (b) {
        set_display_atrib(B_RED);
    } else {
        set_display_atrib(B_GREEN);
    }

    sprintf(buf, "%d", b);
    putsUSART(buf);
    resetcolor();

}

void printValXY(unsigned int v, char x, char y) {
#define FILD_LEN 3
    char d_cnt, fild[FILD_LEN + 1];
    char buf[10] = "";
    //    unsigned int n;
    //
    //    n = v;
    //    if (n == 0)
    //        d_cnt = 1;
    //    else
    //        while (b > 0) {
    //            b = b / 10;
    //            d_cnt++;
    //        }
    resetcolor();
    sprintf(fild, "%d", v);
    d_cnt = strlen(fild);
    set_display_atrib(B_BLUE);
    gotoxy(x, y);
    d_cnt = FILD_LEN - d_cnt;
    while (d_cnt--) {
        putsUSART(" ");
    }
    putsUSART(fild);
    resetcolor();
}

//void clear_val(void) {
//    gotoxy(10, 3);
//    putrsUSART("  ");
//    gotoxy(10, 4);
//    putrsUSART("  ");
//
//
//    gotoxy(10, 8);
//    putrsUSART("   ");
//    gotoxy(10, 9);
//    putrsUSART("   ");
//    gotoxy(10, 10);
//    putrsUSART("   ");
//    gotoxy(10, 11);
//    putrsUSART("   ");
//
//
//}

void print_display(char type_, int v) {
    //2-ERR
    //1-TEMP
    //0-position
    char buf[10] = "";
    gotoxy(5, 17);
    resetcolor();
    if (type_ == 2) {
        //set_display_atrib(F_RED);
        sprintf(buf, "E %d", v);
    } else if (type_ == 1) {
        //set_display_atrib(F_RED);
        if (v != 0xFFFF)
            sprintf(buf, "t%d", v);
        else
            sprintf(buf, "t--");
    } else {
        //set_display_atrib(F_GREEN);
        sprintf(buf, "%d", v);
    }
    putsUSART(buf);
    resetcolor();
}
