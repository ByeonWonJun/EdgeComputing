/*
 *  spnfsd.c
 *  Userland daemon for spNFS.
 *  Based heavily on idmapd.c
 *
 */
/*
 *  idmapd.c
 *
 *  Userland daemon for idmap.
 *
 *  Copyright (c) 2002 The Regents of the University of Michigan.
 *  All rights reserved.
 *
 *  Marius Aamodt Eriksen <marius@umich.edu>
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the University nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 *  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 *  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64
#include <sys/types.h>
#include <sys/time.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <time.h>

#include <err.h>
#include <errno.h>
#include <event.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <syslog.h>
#include <pwd.h>
#include <grp.h>
#include <limits.h>
#include <ctype.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */

#include "cfg.h"
#include "queue.h"
#include "nfslib.h"

#include "nfsd4_spnfs.h"
#include "spnfsd.h"

#ifndef PIPEFS_DIR
#define PIPEFS_DIR  "/var/lib/nfs/rpc_pipefs/"
#endif

#ifndef DSMOUNT_DIR
#define DSMOUNT_DIR "/spnfs"
#endif

/* From Niels */
#define CONF_SAVE(w, f) do {                    \
	char *p = f;                            \
	if (p != NULL)                          \
		(w) = p;                        \
} while (0)

struct spnfs_client {
	int		sc_fd;
	char		sc_path[PATH_MAX]; /* DM: full path to spnfs pipe */
	struct event	sc_event;
};

static void spnfscb(int, short, void *);
static int do_mounts(void);

static void spnfs_msg_handler(struct spnfs_client *, struct spnfs_msg *);
static void send_invalid_msg(int);

size_t  strlcat(char *, const char *, size_t);
size_t  strlcpy(char *, const char *, size_t);
ssize_t atomicio(ssize_t (*)(), int, void *, size_t);
void    mydaemon(int, int);
void    release_parent();
static int read_config();
static void dump_config();

int verbose = 0;
int stripesize = DEFAULT_STRIPE_SIZE;
int densestriping = 0;
int num_dev = 1;  /* XXX no multiple device support yet */
int num_ds;
struct dserver dataservers[SPNFS_MAX_DATA_SERVERS];
char dsmountdir[PATH_MAX];
struct spnfs_config spnfs_config;

static int cache_entry_expiration = 0;
static char pipefsdir[PATH_MAX];
static char pipefspath[PATH_MAX];

/* SPNFS */
struct spnfs_client sc;

/* Used by cfg.c */
char *conf_path;

static void
msg_format(char *rtnbuff, int rtnbuffsize, int errval,
	   const char *fmt, va_list args)
{
	char buff[1024];
	int n;

	vsnprintf(buff, sizeof(buff), fmt, args);

	if ((n = strlen(buff)) > 0 && buff[n-1] == '\n')
		buff[--n] = '\0';

	snprintf(rtnbuff, rtnbuffsize, "%s: %s", buff, strerror(errval));
}

void
spnfsd_warn(const char *fmt, ...)
{
	int errval = errno;	/* save this! */
	char buff[1024];
	va_list args;

	va_start(args, fmt);
	msg_format(buff, sizeof(buff), errval, fmt, args);
	va_end(args);

	syslog(LOG_WARNING, "%s", buff);
}

void
spnfsd_warnx(const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vsyslog(LOG_WARNING, fmt, args);
	va_end(args);
}

void
spnfsd_err(int eval, const char *fmt, ...)
{
	int errval = errno;	/* save this! */
	char buff[1024];
	va_list args;

	va_start(args, fmt);
	msg_format(buff, sizeof(buff), errval, fmt, args);
	va_end(args);

	syslog(LOG_ERR, "%s", buff);
	exit(eval);
}

void
spnfsd_errx(int eval, const char *fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vsyslog(LOG_ERR, fmt, args);
	va_end(args);
	exit(eval);
}

int
main(int argc, char **argv)
{
	int opt, fg = 0;
	char *progname;
	struct stat sb;
	int rc, fd, cmd = 1;
	int i;

	fd = open("/proc/fs/spnfs/ctl", O_WRONLY);
	if (fd < 0)
		spnfsd_errx(1, "kernel init failed (%s)", strerror(errno));
	rc = write(fd, &cmd, sizeof(int));
	if (rc < 0 && errno != EEXIST)
		spnfsd_errx(1, "kernel init failed (%s)", strerror(errno));
	close(fd);

	conf_path = _PATH_SPNFSDCONF;
	strlcpy(pipefsdir, PIPEFS_DIR, sizeof(pipefsdir));
	strlcpy(dsmountdir, DSMOUNT_DIR, sizeof(dsmountdir));
	strlcpy(sc.sc_path, PIPEFS_DIR, sizeof(sc.sc_path));
	
	printf("\nsc.sc_path test by Nuri....11/24 7:34\n");
	printf("%s\n\n", sc.sc_path);

	if ((progname = strrchr(argv[0], '/')))
		progname++;
	else
		progname = argv[0];

	openlog(progname, LOG_PID, LOG_DAEMON);

#define GETOPTSTR "vfd:p:U:G:c:CS"
	opterr=0; /* Turn off error messages */
	while ((opt = getopt(argc, argv, GETOPTSTR)) != -1) {
		if (opt == 'c')
			conf_path = optarg;
		if (opt == '?') {
			if (strchr(GETOPTSTR, optopt))
				errx(1, "'-%c' option requires an argument.", optopt);
			else
				errx(1, "'-%c' is an invalid argument.", optopt);
		}
	}
	optind = 1;

	if (stat(conf_path, &sb) == -1 &&(errno == ENOENT || errno == EACCES)) {
		warn("Skipping configuration file \"%s\"", conf_path);
		conf_path = NULL;
	} else {
		conf_init();
		if (read_config() != 0)
			spnfsd_err(1, "Invalid config file\n");
		if (verbose){
			dump_config();
			printf("Test by Nuri....11/24 3:34\n");
		}
	}

	while ((opt = getopt(argc, argv, GETOPTSTR)) != -1)
		switch (opt) {
		case 'v':
			verbose++;
			break;
		case 'f':
			fg = 1;
			break;
		case 'p':
			strlcpy(pipefsdir, optarg, sizeof(pipefsdir));
			strlcpy(sc.sc_path, optarg, sizeof(sc.sc_path));
			break;
		default:
			break;
		}

	strncat(pipefsdir, "/nfs", sizeof(pipefsdir));
	strncat(sc.sc_path, "/nfs/spnfs", sizeof(sc.sc_path));
	memcpy(pipefspath, sc.sc_path, sizeof(pipefspath));
	
	printf("\n\nTest by Nuri.....11/24 3:36_1\n");	

	spnfs_config.stripe_size = stripesize;
	spnfs_config.dense_striping = densestriping;
	spnfs_config.num_ds = num_ds;
	
	printf("stripesize : %d\n", stripesize);
	printf("densestriping : %d\n", densestriping);
	printf("num_ds : %d\n", num_ds);

	for (i = 0; i < num_ds; i++){
		sprintf(spnfs_config.ds_dir[i], "%s/%s",
			dsmountdir, dataservers[i].ds_ip);
		printf("spnfs_config.ds_dir[%d] %s\n", i, spnfs_config.ds_dir[i]);
	}

	printf("Test by Nuri.....11/24 3:36_2\n\n");

	fd = open("/proc/fs/spnfs/config", O_WRONLY);
	if (fd < 0)
		spnfsd_errx(1, "error creating config (%s)", strerror(errno));
	rc = write(fd, &spnfs_config, sizeof(struct spnfs_config));
	if (rc < 0 && errno != EEXIST)
		spnfsd_errx(1, "error writing config (%s)", strerror(errno));
	close(fd);

	signal(SIGHUP, send_invalid_msg);

	printf("Test by nuri....11/24 5:04_1\n");

	if (do_mounts() != 0)
		spnfsd_err(1, "Mounting DSs failed\n");

	printf("Test by nuri....11/24 5:04_2\n");

/* DMXXX in case I forget -f while testing... */
fg = 1;
	if (!fg){
		printf("Test by nuri....11/24 5:53\n");
		mydaemon(0, 0);
	}

	event_init();
	printf("Test by nuri....11/24 5:55\n");

	if (verbose > 0)
		spnfsd_warnx("Expiration time is %d seconds.",
			     cache_entry_expiration);

	if ((sc.sc_fd = open(sc.sc_path, O_RDWR, 0)) == -1) {
		printf("Test by nuri....11/24 6:00_yes\n");
		perror("spnfsd open file");
	} else {
		printf("Test by nuri....11/24 6:00_no\n");
		event_set(&sc.sc_event, sc.sc_fd, EV_READ, spnfscb, &sc);
		event_add(&sc.sc_event, NULL);
	}

	printf("Test by nuri....11/24 6:12_1\n");

	release_parent();

	printf("Test by nuri....11/24 6:12_2\n");
	
	//Stopping at event_dispatch()

	if (event_dispatch() < 0)
		spnfsd_errx(1, "main: event_dispatch returns errno %d (%s)",
			    errno, strerror(errno));
	
	/* NOTREACHED */
	return 1;
}

static void
spnfs_msg_handler(struct spnfs_client *scp, struct spnfs_msg *im)
{
	int err;

	switch (im->im_type) {
	case SPNFS_TYPE_LAYOUTGET:
		printf("LAYOUTGET - test by nuri\n"); 
		err = spnfsd_layoutget(im);
		break;
	case SPNFS_TYPE_LAYOUTCOMMIT:
		printf("LAYOUTCOMMIT - test by nuri\n");
		err = spnfsd_layoutcommit(im);
		break;
	case SPNFS_TYPE_LAYOUTRETURN:
		printf("LAYOUTRETURN - test by nuri\n");
		err = spnfsd_layoutreturn(im);
		break;
	case SPNFS_TYPE_GETDEVICEITER:
		printf("GETDEVICEITER - test by nuri\n");
		err = spnfsd_getdeviceiter(im);
		break;
	case SPNFS_TYPE_GETDEVICEINFO:
		printf("GETDEVICEINFO - test by nuri\n");
		err = spnfsd_getdeviceinfo(im);
		break;
	case SPNFS_TYPE_SETATTR:
		printf("SETATTR - test by nuri\n");
		err = spnfsd_setattr(im);
		break;
	case SPNFS_TYPE_OPEN:
		printf("OPEN - test by nuri\n");
		err = spnfsd_open(im);
		break;
	case SPNFS_TYPE_CLOSE:
		printf("CLOSE - test by nuri\n");
		err = spnfsd_close(im);
		break;
	case SPNFS_TYPE_CREATE:
		printf("CREATE - test by nuri\n");
		err = spnfsd_create(im);
		break;
	case SPNFS_TYPE_REMOVE:
		printf("REMOVE - test by nuri\n");
		err = spnfsd_remove(im);
		break;
	case SPNFS_TYPE_COMMIT:
		printf("COMMIT - test by nuri\n");
		err = spnfsd_commit(im);
		break;
	case SPNFS_TYPE_READ:
		printf("READ - test by nuri\n");
		err = spnfsd_read(im);
		break;
	case SPNFS_TYPE_WRITE:
		printf("WRITE - test by nuri\n");
		err = spnfsd_write(im);
		break;
	default:
		spnfsd_warnx("spnfs_msg_handler: Invalid msg type (%d) in message",
			     im->im_type);
		im->im_status |= SPNFS_STATUS_INVALIDMSG;
		err = -EINVAL;
		break;
	}
}

static void
spnfscb(int fd, short which, void *data)
{
	struct spnfs_client *scp = data;
	struct spnfs_msg im;
	int rval;

	printf("spnfscb Test by nuri....11/24 9:37\n");

	if (which != EV_READ)
		goto out;

	if (atomicio(read, scp->sc_fd, &im, sizeof(im)) != sizeof(im)) {
		if (verbose > 0)
			spnfsd_warn("spnfscb: read(%s)", scp->sc_path);
		if (errno == EPIPE)
			return;
		goto out;
	}

	spnfs_msg_handler(scp, &im);

	/* XXX: I don't like ignoring this error in the id->name case,
	 * but we've never returned it, and I need to check that the client
	 * can handle it gracefully before starting to return it now. */

	if (im.im_status == SPNFS_STATUS_FAIL)
		im.im_status = SPNFS_STATUS_SUCCESS;

	if ((rval=atomicio(write, scp->sc_fd, &im, sizeof(im))) != sizeof(im)) {
		spnfsd_warn("spnfscb: write(%s)", scp->sc_path);
	}

out:
	event_add(&scp->sc_event, NULL);
}

/*
 * mydaemon creates a pipe between the partent and child
 * process. The parent process will wait until the
 * child dies or writes a '1' on the pipe signaling
 * that it started successfully.
 */
int pipefds[2] = { -1, -1};

void
mydaemon(int nochdir, int noclose)
{
	int pid, status, tempfd;

	if (pipe(pipefds) < 0)
		err(1, "mydaemon: pipe() failed: errno %d", errno);

	if ((pid = fork ()) < 0)
		err(1, "mydaemon: fork() failed: errno %d", errno);

	if (pid != 0) {
		/*
		 * Parent. Wait for status from child.
		 */
		close(pipefds[1]);
		if (read(pipefds[0], &status, 1) != 1)
			exit(1);
		exit (0);
	}
	/* Child.	*/
	close(pipefds[0]);
	setsid ();
	if (nochdir == 0) {
		if (chdir ("/") == -1)
			err(1, "mydaemon: chdir() failed: errno %d", errno);
	}

	while (pipefds[1] <= 2) {
		pipefds[1] = dup(pipefds[1]);
		if (pipefds[1] < 0)
			err(1, "mydaemon: dup() failed: errno %d", errno);
	}

	if (noclose == 0) {
		tempfd = open("/dev/null", O_RDWR);
		if (tempfd < 0)
			tempfd = open("/", O_RDONLY);
		if (tempfd >= 0) {
			dup2(tempfd, 0);
			dup2(tempfd, 1);
			dup2(tempfd, 2);
			closeall(3);
		} else
			closeall(0);
	}

	return;
}

void
release_parent()
{
	int status;

	if (pipefds[1] > 0) {
		write(pipefds[1], &status, 1);
		close(pipefds[1]);
		pipefds[1] = -1;
	}
}

static int
read_config()
{
	char *xpipefsdir = NULL;
	char *xdsmountdir = NULL;
	int ds;
	char ipstr[20], portstr[20], rootstr[20], dsidstr[20];

	verbose = conf_get_num("General", "Verbosity", 0);
	stripesize = conf_get_num("General", "Stripe-size",DEFAULT_STRIPE_SIZE);
	densestriping = conf_get_num("General", "Dense-striping", 0);
	CONF_SAVE(xpipefsdir, conf_get_str("General", "Pipefs-Directory"));
	if (xpipefsdir != NULL)
		strlcpy(pipefsdir, xpipefsdir, sizeof(pipefsdir));
	CONF_SAVE(xdsmountdir, conf_get_str("General", "DS-Mount-Directory"));
	if (xdsmountdir != NULL)
		strlcpy(dsmountdir, xdsmountdir, sizeof(dsmountdir));
	num_ds = conf_get_num("DataServers", "NumDS", 0);
	if (num_ds < 1 || num_ds > SPNFS_MAX_DATA_SERVERS)
		spnfsd_err(1, "Invalid number of data servers in config: %d\n",
			num_ds);
	for (ds = 1; ds <= num_ds ; ds++) {
		sprintf(ipstr, "DS%d_IP", ds);
		sprintf(portstr, "DS%d_PORT", ds);
		sprintf(rootstr, "DS%d_ROOT", ds);
		sprintf(dsidstr, "DS%d_ID", ds);
		CONF_SAVE(dataservers[ds-1].ds_ip,
			conf_get_str("DataServers", ipstr));
		if (dataservers[ds-1].ds_ip == NULL)
			spnfsd_err(1, "Missing IP for DS%d\n", ds);
		dataservers[ds-1].ds_port =
			conf_get_num("DataServers", portstr, DEFAULT_DS_PORT);
		CONF_SAVE(dataservers[ds-1].ds_path,
			conf_get_str("DataServers", rootstr));
		if (dataservers[ds-1].ds_ip == NULL)
			spnfsd_err(1, "Missing IP for DS%d\n", ds);
		dataservers[ds-1].ds_id =
			conf_get_num("DataServers", dsidstr, -1);
		if (dataservers[ds-1].ds_id < 0)
			spnfsd_err(1, "Missing or invalid ID for DS%d\n", ds);
	}

	return 0;
}

static void
dump_config()
{
	int ds;

	printf("Verbosity: %d\n", verbose);
	printf("Stripe size: %d\n", stripesize);
	printf("Dense striping: %d\n", densestriping);
	printf("Number of data servers: %d\n", num_ds);

	for (ds = 0 ; ds < num_ds ; ds++) {
		printf("DS%d IP: %s\n", ds+1, dataservers[ds].ds_ip);
		printf("DS%d PORT: %d\n", ds+1, dataservers[ds].ds_port);
		printf("DS%d ROOT: %s\n", ds+1, dataservers[ds].ds_path);
		printf("DS%d DSID: %d\n", ds+1, dataservers[ds].ds_id);
	}
}

static int
do_mounts()
{
	int ds;
	char cmd[1024];
	
	printf("Mount start by nuri....11/24_1 5:08\n");

	return 0;
	for (ds = 0 ; ds < num_ds ; ds++) {
		sprintf(cmd, "mkdir -p %s/%s", dsmountdir,
			dataservers[ds].ds_ip);
		system(cmd);
		sprintf(cmd, "mount -t nfs4 %s:%s %s/%s",
			dataservers[ds].ds_ip, dataservers[ds].ds_path,
			dsmountdir, dataservers[ds].ds_ip);
		system(cmd);
	}
}

static void
send_invalid_msg(int signum)
{
	struct spnfs_msg im;
	int fd, rval;
	
	printf("Send invalid_msg test by nuri....11/24 5:02\n");

	im.im_status = SPNFS_STATUS_FAIL;

	if ((fd = open(pipefspath, O_RDWR, 0)) == -1) {
		perror("spnfsd open pipe");
	} else {
		if ((rval=atomicio(write, fd, &im, sizeof(im))) != sizeof(im)) {
			spnfsd_warn("send_invalid_msg: write(%s)", pipefspath);
		}
	}
}
