#include "../../include/terminal.h"
#include "../../include/keyboard.h"
#include "../../include/io.h"
#include "../../include/string.h"
#include "../../include/mouse.h"

#define VGA_MEMORY ((unsigned short*)0xB8000)
#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define BLACK   0x00
#define WHITE   0x0F
#define LTGREEN 0x0A
#define LTBLUE  0x09
#define LTCYAN  0x0B
#define LTRED   0x0C
#define LTGREY  0x07
#define DKGREY  0x08
#define YELLOW  0x0E
#define LTMAG   0x0D
#define MAX_HISTORY 50
#define MAX_COMMAND_LEN 256
#define MAX_PATH 256
#define MAX_FILES 100
#define MAX_FILE_SIZE 4096
#define BLINK_HALF 20000000

typedef struct { char name[64]; char content[MAX_FILE_SIZE]; int size; int is_dir; } file_t;
static file_t files[MAX_FILES];
static int file_count = 0;

int cursor_x = 0, cursor_y = 12, header_lines = 12;
static uint8_t current_color = 0x0F;
static char current_path[MAX_PATH] = "~";
static char history[MAX_HISTORY][MAX_COMMAND_LEN];
static int history_count = 0;
static int blink_visible = 1;
static unsigned int blink_counter = 0;

static void int_to_str(int num, char* buf) {
    int i=0;
    if(num==0){buf[0]='0';buf[1]=0;return;}
    if(num<0){buf[0]='-';num=-num;i=1;}
    char tmp[10];int j=0;
    while(num>0){tmp[j++]='0'+(num%10);num/=10;}
    while(j>0)buf[i++]=tmp[--j];
    buf[i]=0;
}

void terminal_write_int(int n) { char b[20]; int_to_str(n,b); terminal_write(b); }
void enable_hw_cursor(void) { outb(0x3D4,0x0A); outb(0x3D5,0x0E); outb(0x3D4,0x0B); outb(0x3D5,0x0F); }
void update_hw_cursor(void) {
    uint16_t p=cursor_y*VGA_WIDTH+cursor_x;
    outb(0x3D4,0x0F); outb(0x3D5,(uint8_t)(p&0xFF));
    outb(0x3D4,0x0E); outb(0x3D5,(uint8_t)((p>>8)&0xFF));
}
static void sw_cursor_draw(int s) {
    int p=cursor_y*VGA_WIDTH+cursor_x;
    VGA_MEMORY[p]=((BLACK<<4)|WHITE)<<8|(s?'_':' ');
}
static void blink_tick(void) {
    blink_counter++;
    if(blink_counter>=BLINK_HALF){blink_counter=0;blink_visible=!blink_visible;sw_cursor_draw(blink_visible);}
}
static void terminal_write_at(const char* s,int x,int y) {
    int p=y*VGA_WIDTH+x;
    while(*s)VGA_MEMORY[p++]=(current_color<<8)|(unsigned char)*s++;
}

void terminal_putchar(char c) {
    if(c=='\n'){
        cursor_x=0;cursor_y++;
        if(cursor_y>=VGA_HEIGHT){
            for(int y=header_lines;y<VGA_HEIGHT-1;y++)
                for(int x=0;x<VGA_WIDTH;x++)
                    VGA_MEMORY[y*VGA_WIDTH+x]=VGA_MEMORY[(y+1)*VGA_WIDTH+x];
            for(int x=0;x<VGA_WIDTH;x++)
                VGA_MEMORY[(VGA_HEIGHT-1)*VGA_WIDTH+x]=(current_color<<8)|' ';
            cursor_y=VGA_HEIGHT-1;
        }
    }else if(c=='\b'){
        if(cursor_x>0){cursor_x--;VGA_MEMORY[cursor_y*VGA_WIDTH+cursor_x]=(current_color<<8)|' ';}
    }else if(c>=' '&&c<='~'){
        VGA_MEMORY[cursor_y*VGA_WIDTH+cursor_x]=(current_color<<8)|(unsigned char)c;
        cursor_x++;
        if(cursor_x>=VGA_WIDTH){cursor_x=0;cursor_y++;}
    }
    update_hw_cursor();enable_hw_cursor();
}

void terminal_write(const char* s) { while(*s)terminal_putchar(*s++); }
void terminal_writeline(const char* s) { terminal_write(s); terminal_putchar('\n'); }
void terminal_clear(void) {
    for(int y=header_lines;y<VGA_HEIGHT;y++)
        for(int x=0;x<VGA_WIDTH;x++)
            VGA_MEMORY[y*VGA_WIDTH+x]=(BLACK<<8)|' ';
    cursor_x=0;cursor_y=header_lines;update_hw_cursor();enable_hw_cursor();
}
void terminal_set_color(uint8_t f,uint8_t b) { current_color=(b<<4)|f; }

void draw_header(void) {
    for(int i=0;i<VGA_WIDTH*VGA_HEIGHT;i++)VGA_MEMORY[i]=(BLACK<<8)|' ';
    for(int x=0;x<VGA_WIDTH;x++)VGA_MEMORY[x]=(LTCYAN<<8)|'=';
    int cx=26;
    current_color=(BLACK<<4)|LTRED; terminal_write_at("   ___  _   _ ____   ___  ",cx,1);
    current_color=(BLACK<<4)|LTMAG; terminal_write_at("  / _ \\| | | |  _ \\ / _ \\ ",cx,2);
    current_color=(BLACK<<4)|LTCYAN; terminal_write_at(" | | | | | | | |_) | | | |",cx,3);
    current_color=(BLACK<<4)|LTGREEN; terminal_write_at(" | |_| | |_| |  _ <| |_| |",cx,4);
    current_color=(BLACK<<4)|YELLOW; terminal_write_at("  \\___/ \\___/|_| \\_\\\\___/ ",cx,5);
    current_color=(BLACK<<4)|WHITE; terminal_write_at("    Quro OS Terminal v1.0   ",cx,7);
    for(int x=0;x<VGA_WIDTH;x++)VGA_MEMORY[8*VGA_WIDTH+x]=(LTCYAN<<8)|'-';
    current_color=(BLACK<<4)|LTGREY; terminal_write_at(" Type 'help' for commands  ",cx,9);
    terminal_write_at("  'clear' to clean screen  ",cx,10);
    for(int x=0;x<VGA_WIDTH;x++)VGA_MEMORY[11*VGA_WIDTH+x]=(LTCYAN<<8)|'=';
    header_lines=12;current_color=0x0F;
}

static void add_history(const char* c) {
    if(strlen(c)==0)return;
    if(history_count>0&&strcmp(history[history_count-1],c)==0)return;
    if(history_count<MAX_HISTORY)strcpy(history[history_count++],c);
}
static void fs_init(void) {
    strcpy(files[0].name,"README.txt");files[0].is_dir=0;
    strcpy(files[0].content,"Welcome to Quro OS!\n\nType 'help' for commands.\n");
    files[0].size=strlen(files[0].content);file_count=1;
}

static void cmd_help(void) {
    terminal_set_color(LTCYAN,BLACK);
    terminal_writeline("Commands: help version clear ls mkdir touch edit cat rm");
    terminal_writeline("          qmatrix qurofetch sysinfo calc time date history");
    terminal_set_color(LTGREY,BLACK);
    terminal_writeline("          reboot shutdown");
    terminal_writeline("");
}
static void cmd_version(void) { terminal_set_color(LTGREEN,BLACK);terminal_writeline("Quro OS v1.0.0");terminal_set_color(LTGREY,BLACK);terminal_write("Build: ");terminal_writeline(__DATE__" "__TIME__); }
static void cmd_ls(void) { terminal_set_color(LTCYAN,BLACK);terminal_writeline("Files:");terminal_set_color(LTGREY,BLACK);for(int i=0;i<file_count;i++){terminal_write(files[i].is_dir?"[DIR]  ":"[FILE] ");terminal_writeline(files[i].name);} }
static void cmd_mkdir(char* a) { if(!a||strlen(a)==0){terminal_writeline("Usage: mkdir <dirname>");return;}if(file_count>=MAX_FILES){terminal_writeline("Filesystem full");return;}strcpy(files[file_count].name,a);files[file_count].is_dir=1;file_count++;terminal_write("Created: ");terminal_writeline(a); }
static void cmd_touch(char* a) { if(!a||strlen(a)==0){terminal_writeline("Usage: touch <filename>");return;}if(file_count>=MAX_FILES){terminal_writeline("Filesystem full");return;}strcpy(files[file_count].name,a);files[file_count].is_dir=0;files[file_count].content[0]='\0';files[file_count].size=0;file_count++;terminal_write("Created: ");terminal_writeline(a); }
static void cmd_cat(char* a) { if(!a||strlen(a)==0){terminal_writeline("Usage: cat <filename>");return;}for(int i=0;i<file_count;i++){if(strcmp(files[i].name,a)==0&&!files[i].is_dir){terminal_set_color(WHITE,BLACK);terminal_writeline(files[i].content);terminal_set_color(LTGREY,BLACK);return;}}terminal_writeline("File not found"); }
static void cmd_rm(char* a) { if(!a)return;for(int i=0;i<file_count;i++){if(strcmp(files[i].name,a)==0&&!files[i].is_dir){for(int j=i;j<file_count-1;j++)files[j]=files[j+1];file_count--;terminal_write("Removed: ");terminal_writeline(a);return;}} }
static void cmd_time(void) { outb(0x70,0x04);for(volatile int i=0;i<1000;i++)asm volatile("nop");uint8_t h=inb(0x71);outb(0x70,0x02);for(volatile int i=0;i<1000;i++)asm volatile("nop");uint8_t m=inb(0x71);outb(0x70,0x00);for(volatile int i=0;i<1000;i++)asm volatile("nop");uint8_t s=inb(0x71);h=((h>>4)*10)+(h&0x0F);m=((m>>4)*10)+(m&0x0F);s=((s>>4)*10)+(s&0x0F);terminal_set_color(LTCYAN,BLACK);terminal_write("Current time: ");terminal_set_color(WHITE,BLACK);terminal_write_int(h);terminal_write(":");if(m<10)terminal_write("0");terminal_write_int(m);terminal_write(":");if(s<10)terminal_write("0");terminal_write_int(s);terminal_writeline("");terminal_set_color(LTGREY,BLACK); }
static void cmd_date(void) { outb(0x70,0x07);for(volatile int i=0;i<1000;i++)asm volatile("nop");uint8_t d=inb(0x71);outb(0x70,0x08);for(volatile int i=0;i<1000;i++)asm volatile("nop");uint8_t m=inb(0x71);outb(0x70,0x09);for(volatile int i=0;i<1000;i++)asm volatile("nop");uint8_t y=inb(0x71);d=((d>>4)*10)+(d&0x0F);m=((m>>4)*10)+(m&0x0F);y=((y>>4)*10)+(y&0x0F);terminal_set_color(LTCYAN,BLACK);terminal_write("Current date: ");terminal_set_color(WHITE,BLACK);terminal_write_int(d);terminal_write("/");if(m<10)terminal_write("0");terminal_write_int(m);terminal_write("/20");if(y<10)terminal_write("0");terminal_write_int(y);terminal_writeline("");terminal_set_color(LTGREY,BLACK); }
static void cmd_calc(char* a) { if(!a||strlen(a)==0){terminal_writeline("Usage: calc <expression>");return;}int v[10],vc=0;char o[10];int oc=0;for(int i=0;a[i];i++){if(a[i]>='0'&&a[i]<='9'){int n=0;while(a[i]>='0'&&a[i]<='9'){n=n*10+(a[i]-'0');i++;}i--;v[vc++]=n;}else if(a[i]=='*'||a[i]=='/'){while(oc>0&&(o[oc-1]=='*'||o[oc-1]=='/')){int b=v[--vc],aa=v[--vc];char op=o[--oc];v[vc++]=(op=='*')?aa*b:aa/b;}o[oc++]=a[i];}else if(a[i]=='+'||a[i]=='-'){while(oc>0){int b=v[--vc],aa=v[--vc];char op=o[--oc];if(op=='+')v[vc++]=aa+b;else if(op=='-')v[vc++]=aa-b;else if(op=='*')v[vc++]=aa*b;else v[vc++]=aa/b;}o[oc++]=a[i];}}while(oc>0){int b=v[--vc],aa=v[--vc];char op=o[--oc];if(op=='+')v[vc++]=aa+b;else if(op=='-')v[vc++]=aa-b;else if(op=='*')v[vc++]=aa*b;else v[vc++]=aa/b;}terminal_write("Result: ");terminal_write_int(v[0]);terminal_writeline(""); }

static void execute_command(char* l) {
    add_history(l); char c[64]={0},a[256]={0};int i=0,j=0;
    while(l[i]&&l[i]!=' '&&j<63)c[j++]=l[i++];
    if(l[i]==' '){i++;j=0;while(l[i]&&j<255)a[j++]=l[i++];}
    if(strcmp(c,"help")==0)cmd_help();
    else if(strcmp(c,"version")==0)cmd_version();
    else if(strcmp(c,"clear")==0){terminal_clear();draw_header();cursor_x=0;cursor_y=header_lines;update_hw_cursor();enable_hw_cursor();}
    else if(strcmp(c,"ls")==0)cmd_ls();
    else if(strcmp(c,"mkdir")==0)cmd_mkdir(a);
    else if(strcmp(c,"touch")==0)cmd_touch(a);
    else if(strcmp(c,"cat")==0)cmd_cat(a);
    else if(strcmp(c,"rm")==0)cmd_rm(a);
    else if(strcmp(c,"qmatrix")==0)game_qmatrix();
    else if(strcmp(c,"qurofetch")==0)qurofetch();
    else if(strcmp(c,"sysinfo")==0)cmd_sysinfo();
    else if(strcmp(c,"calc")==0)cmd_calc(a);
    else if(strcmp(c,"time")==0)cmd_time();
    else if(strcmp(c,"date")==0)cmd_date();
    else if(strcmp(c,"history")==0){for(int h=0;h<history_count;h++)terminal_writeline(history[h]);}
    else if(strcmp(c,"reboot")==0){terminal_writeline("Rebooting...");outb(0x64,0xFE);}
    else if(strcmp(c,"shutdown")==0){terminal_clear();terminal_set_color(WHITE,BLACK);terminal_write("SYSTEM HALTED");asm volatile("cli");while(1)asm volatile("hlt");}
    else if(strlen(c)>0){terminal_set_color(LTRED,BLACK);terminal_write("Command not found: ");terminal_writeline(c);terminal_set_color(LTGREY,BLACK);}
}

void terminal_initialize(void) { draw_header();fs_init();cursor_x=0;cursor_y=header_lines;update_hw_cursor();enable_hw_cursor(); }

void terminal_run(void) {
    char ib[MAX_COMMAND_LEN];
    while(1) {
        cursor_x = 0;
        terminal_set_color(LTGREEN,BLACK);terminal_write("quro@user:");
        terminal_set_color(LTBLUE,BLACK);terminal_write(current_path);
        terminal_set_color(WHITE,BLACK);terminal_write("$ ");
        blink_visible=1;blink_counter=0;sw_cursor_draw(1);
        int ip=0;ib[0]='\0';
        while(1){
            blink_tick();
            if(!key_available())continue;
            sw_cursor_draw(0);char ch=keyboard_getchar();
            if(ch==0){sw_cursor_draw(1);continue;}
            if(ch=='\n'||ch=='\r'){ib[ip]='\0';terminal_putchar('\n');break;}
            if(ch=='\b'){if(ip>0){ip--;terminal_putchar('\b');}}
            else if(ch>=' '&&ch<='~'&&ip<MAX_COMMAND_LEN-1){ib[ip++]=ch;terminal_putchar(ch);}
            blink_visible=1;blink_counter=0;sw_cursor_draw(1);
        }
        if(ip>0)execute_command(ib);
        if(cursor_y>=VGA_HEIGHT){
            for(int y=header_lines;y<VGA_HEIGHT-1;y++)
                for(int x=0;x<VGA_WIDTH;x++)
                    VGA_MEMORY[y*VGA_WIDTH+x]=VGA_MEMORY[(y+1)*VGA_WIDTH+x];
            for(int x=0;x<VGA_WIDTH;x++)VGA_MEMORY[(VGA_HEIGHT-1)*VGA_WIDTH+x]=(BLACK<<8)|' ';
            cursor_y=VGA_HEIGHT-1;
        }
        cursor_x=0;
    }
}
char terminal_getchar(void) { return keyboard_getchar(); }
void terminal_read_line(char* b,int m) {
    int p=0;
    while(1){
        blink_tick();if(!key_available())continue;
        sw_cursor_draw(0);char c=keyboard_getchar();
        if(c==0){sw_cursor_draw(1);continue;}
        if(c=='\n'||c=='\r'){b[p]=0;terminal_putchar('\n');break;}
        if(c=='\b'&&p>0){p--;terminal_putchar('\b');}
        else if(c>=' '&&c<='~'&&p<m-1){b[p++]=c;terminal_putchar(c);}
        blink_visible=1;blink_counter=0;sw_cursor_draw(1);
    }
}
