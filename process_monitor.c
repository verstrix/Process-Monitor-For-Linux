#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define REFRESH_RATE 2

void get_memory_usage(int *total_mem, int *used_mem) {
    *total_mem = 4096;
    *used_mem = 2048;
}

float get_cpu_temperature() {
    FILE *fp = fopen("/sys/class/thermal/thermal_zone0/temp", "r");
    if (!fp) return -1.0;

    int temp;
    fscanf(fp, "%d", &temp);
    fclose(fp);

    return temp / 1000.0;
}

void draw_header() {
    attron(COLOR_PAIR(1));
    mvprintw(1, 2, "PID");
    mvprintw(1, 10, "USER");
    mvprintw(1, 20, "CPU (%)");
    mvprintw(1, 30, "MEM (%)");
    mvprintw(1, 40, "APP");
    attroff(COLOR_PAIR(1));
    mvhline(2, 1, '-', COLS - 2);
}

void draw_bar(int start_x, int start_y, int length, float percentage, int color) {
    int bar_length = (int)(length * (percentage / 100.0));
    attron(COLOR_PAIR(color));
    for (int i = 0; i < bar_length; i++) {
        mvaddch(start_y, start_x + i, ' ');
    }
    attroff(COLOR_PAIR(color));
}

void draw_processes() {
    int row = 3;
    for (int i = 0; i < 5; i++) {
        int pid = 1234 + i;
        char user[32] = "user";
        float cpu = (float)(i * 10);
        float mem = (float)(i * 5);
        char command[128] = "err";

        attron(COLOR_PAIR(2));
        mvprintw(row, 2, "%d", pid);
        mvprintw(row, 10, "%s", user);
        mvprintw(row, 20, "%.1f", cpu);
        mvprintw(row, 30, "%.1f", mem);
        mvprintw(row, 40, "%s", command);
        attroff(COLOR_PAIR(2));

        draw_bar(60, row, 20, cpu, 3);
        draw_bar(85, row, 20, mem, 4);

        row++;
        if (row >= LINES - 2) break;
    }
}

void draw_system_stats() {
    int total_mem, used_mem;
    get_memory_usage(&total_mem, &used_mem);
    float cpu_temp = get_cpu_temperature();

    char ram_usage[64];
    char cpu_temp_str[64];

    snprintf(ram_usage, sizeof(ram_usage), "RAM USAGE: %d MB / %d MB", used_mem, total_mem);
    snprintf(cpu_temp_str, sizeof(cpu_temp_str), "CPU TEMP: %.1f °C", cpu_temp);

    attron(COLOR_PAIR(1));
    mvprintw(LINES - 3, COLS - strlen(ram_usage) - 2, "%s", ram_usage);
    mvprintw(LINES - 2, COLS - strlen(cpu_temp_str) - 2, "%s", cpu_temp_str);
    attroff(COLOR_PAIR(1));
}

int main() {
    initscr();
    start_color();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);

    init_pair(1, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_RED, COLOR_BLACK);

    int ch;
    while (1) {
        clear();
        box(stdscr, 0, 0);

        draw_header();

        draw_processes();

        draw_system_stats();

        attron(COLOR_PAIR(5));
        mvprintw(LINES - 2, 2, "Press 'q' to quit | Press 'r' to refresh | Refreshing every %d seconds...", REFRESH_RATE);
        attroff(COLOR_PAIR(5));

        refresh();

        timeout(REFRESH_RATE * 1000);
        ch = getch();
        if (ch == 'q') break;
        if (ch == 'r') continue;
    }

    endwin();
    return 0;
}
