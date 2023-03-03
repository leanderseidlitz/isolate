#include <stdio.h>
#include <stdlib.h>
#include <linux/seccomp.h>
#include <seccomp.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <sys/mount.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	fprintf(stderr, "--> SECCOMP WRAPPER ACTIVE <--\n");
    scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_ALLOW);
	// filter all TIOCSTI calls, disallowing writing to the parent tty
	if(seccomp_rule_add(ctx, SCMP_ACT_KILL_PROCESS, SCMP_SYS(ioctl), 1,
						SCMP_A1(SCMP_CMP_EQ, TIOCSTI, TIOCSTI))){
		if (ctx) seccomp_release(ctx);
		fprintf(stderr, "[1471] failed to install seccomp filter\n");
		return 1;
	}
    if(seccomp_load(ctx)){
		fprintf(stderr, "[1472] failed to load seccomp context\n");
		return 1;
	}

	char **args = (char**) malloc(sizeof(char*) * argc);
	int i = 0;
	for (i = 0; i < argc - 1; i++){
		args[i] = argv[i+1];
	}
	args[i] = NULL;
    return execvp(args[0], args);
}
