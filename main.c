#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <ncurses.h>

#include "oko.h"

typedef struct {
	WINDOW *win;
	int line;
	int col;
	usage *usage;
} usage_display_data;


static void print_instructions(WINDOW *win, int line, int col)
{
	mvwprintw(win, line, col, "OKO <o> - Press 'q' to exit.");
	wrefresh(win);
}

static void
print_machine_info(WINDOW *win, int line, int col, machine *machine)
{
	wattron(win, A_BOLD);
	mvwprintw(win, line, col, "Machine");
	wattroff(win, A_BOLD);

	mvwprintw(win, ++line, col, "serialnumber: %s", machine->serialnumber);
	mvwprintw(win, ++line, col, "type: %s", machine->type);
	mvwprintw(win, ++line, col, "model: %s", machine->model);
	mvwprintw(win, ++line, col, "hostname: %s", machine->hostname);
	mvwprintw(win, ++line, col, "network.ip: %s", machine->network.ip);
	mvwprintw(win, ++line, col, "network.macaddress: %s", machine->network.macaddress);
	mvwprintw(win, ++line, col, "cpu.arch: %s", machine->cpu.arch);
	mvwprintw(win, ++line, col, "cpu.model: %s", machine->cpu.model);
	mvwprintw(win, ++line, col, "cpu.ncpus: %d", machine->cpu.ncpus);
	mvwprintw(win, ++line, col, "memory.physmem: %lld", machine->memory.physmem);
	mvwprintw(win, ++line, col, "os.name: %s", machine->os.name);
	mvwprintw(win, ++line, col, "os.release %s", machine->os.release);
	mvwprintw(win, ++line, col, "os.version %s", machine->os.version);

	wrefresh(win);
}

static void
print_machine_usage(WINDOW *win, int line, int col, usage *usage)
{
	wattron(win, A_BOLD);
	mvwprintw(win, line, col, "Usage");
	wattroff(win, A_BOLD);

	mvwprintw(win, ++line, col, "cpu.user: %.2f %%", usage->cpu.user);
	mvwprintw(win, ++line, col, "cpu.system: %.2f %%", usage->cpu.system);
	mvwprintw(win, ++line, col, "cpu.idle: %.2f %%", usage->cpu.idle);
	mvwprintw(win, ++line, col, "cpu.nice: %.2f %%", usage->cpu.nice);
	mvwprintw(win, ++line, col, "memory.used: %lld", usage->memory.used);
	mvwprintw(win, ++line, col, "memory.free: %lld", usage->memory.free);
	mvwprintw(win, ++line, col, "memory.swaptotal: %lld", usage->memory.swaptotal);
	mvwprintw(win, ++line, col, "memory.swapused: %lld", usage->memory.swapused);
	mvwprintw(win, ++line, col, "memory.swapfree: %lld", usage->memory.swapfree);

	wrefresh(win);
}

static void*
print_machine_usage_routine(usage_display_data *udd)
{
	int startup = 1;

	for (;;) {
		collect_machine_usage(udd->usage);

		if (startup) sleep(1); /* To avoid strange screen at startup */
		
		box(udd->win, ACS_VLINE, ACS_HLINE);
		print_machine_usage(udd->win, udd->line, udd->col, udd->usage);
		wclear(udd->win);
		
		if (!startup) sleep(2);
		startup = 0;
	}
	
	return NULL;
}

static int
launch_thread(void *routine, void *data)
{
    pthread_attr_t attr;
    pthread_t thread_id;
    int rc, thread_error;
 
    rc = pthread_attr_init(&attr);
    assert(!rc);
    rc = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    assert(!rc);
 
    thread_error = pthread_create(&thread_id, &attr, routine, data);
 
    rc = pthread_attr_destroy(&attr);
    assert(!rc);
    
    if (thread_error != 0) return 1;

    return 0;
}

static int
display_mode()
{
	machine m;
	usage u;
	WINDOW *instw, *topw, *bottomw;
	usage_display_data udd;

	initscr();
	instw = subwin(stdscr, 1, COLS, 0, 0);  
	topw = subwin(stdscr, LINES / 2 - 1, COLS, 1, 0);
	bottomw = subwin(stdscr, LINES / 2, COLS, LINES / 2, 0);
	box(topw, ACS_VLINE, ACS_HLINE);
	
	print_instructions(instw, 0, 0);

	collect_machine_info(&m);
	print_machine_info(topw, 1, 1, &m);

	udd.win = bottomw;
	udd.line = 1;
	udd.col = 1;
	udd.usage = &u;

	launch_thread(&print_machine_usage_routine, (usage_display_data *) &udd);
	
	noecho();
	while (getch() != 'q');
	endwin();

	free(topw);
	free(bottomw);
    
	return (EXIT_SUCCESS);
}

static void
print_usage()
{
	printf("Usage: oko [-h]\n");
}


int
main(int argc, char * const argv[])
{
	int opt;

	if (argc < 2) {
		display_mode();
	}

	while ((opt = getopt(argc, argv, "h")) != -1) {
		switch(opt) {
			case 'h':
				print_usage();
				return (EXIT_SUCCESS);
			
			default:
				print_usage();
				return (EXIT_FAILURE);	
		}
	}

	return (EXIT_SUCCESS);
}
