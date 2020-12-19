/*
 * include/linux/nfsd4_spnfs.h
 *
 * spNFS - simple pNFS implementation with userspace daemon
 *
 */

/******************************************************************************

(c) 2007 Network Appliance, Inc.  All Rights Reserved.

Network Appliance provides this source code under the GPL v2 License.
The GPL v2 license is available at
http://opensource.org/licenses/gpl-license.php.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

******************************************************************************/

#ifndef NFS_SPNFS_H
#define NFS_SPNFS_H


#ifdef __KERNEL__
#include "exportfs.h"
#include "sunrpc/svc.h"
#include "nfsd/nfsfh.h"
#else
#include <sys/types.h>
#endif /* __KERNEL__ */

#define SPNFS_STATUS_INVALIDMSG		0x01
#define SPNFS_STATUS_AGAIN		0x02
#define SPNFS_STATUS_FAIL		0x04
#define SPNFS_STATUS_SUCCESS		0x08

#define SPNFS_TYPE_LAYOUTGET		0x01
#define SPNFS_TYPE_LAYOUTCOMMIT		0x02
#define SPNFS_TYPE_LAYOUTRETURN		0x03
#define SPNFS_TYPE_GETDEVICEITER	0x04
#define SPNFS_TYPE_GETDEVICEINFO	0x05
#define SPNFS_TYPE_SETATTR		0x06
#define SPNFS_TYPE_OPEN			0x07
#define	SPNFS_TYPE_CLOSE		0x08
#define SPNFS_TYPE_CREATE		0x09
#define SPNFS_TYPE_REMOVE		0x0a
#define SPNFS_TYPE_COMMIT		0x0b
#define SPNFS_TYPE_READ			0x0c
#define SPNFS_TYPE_WRITE		0x0d

#define	SPNFS_MAX_DEVICES		1
#define	SPNFS_MAX_DATA_SERVERS		16
#define SPNFS_MAX_IO			512

/* layout */
struct spnfs_msg_layoutget_args {
	unsigned long inode;
	unsigned long generation;
};

struct spnfs_filelayout_list {
	u_int32_t       fh_len;
	unsigned char   fh_val[128]; /* DMXXX fix this const */
};

struct spnfs_msg_layoutget_res {
	int status;
	u_int64_t devid;
	u_int64_t stripe_size;
	u_int32_t stripe_type;
	u_int32_t stripe_count;
	struct spnfs_filelayout_list flist[SPNFS_MAX_DATA_SERVERS];
};

/* layoutcommit */
struct spnfs_msg_layoutcommit_args {
	unsigned long inode;
	unsigned long generation;
	u_int64_t file_size;
};

struct spnfs_msg_layoutcommit_res {
	int status;
};

/* layoutreturn */
/* No op for the daemon */
/*
struct spnfs_msg_layoutreturn_args {
};

struct spnfs_msg_layoutreturn_res {
};
*/

/* getdeviceiter */
struct spnfs_msg_getdeviceiter_args {
	unsigned long inode;
	u_int64_t cookie;
	u_int64_t verf;
};

struct spnfs_msg_getdeviceiter_res {
	int status;
	u_int64_t devid;
	u_int64_t cookie;
	u_int64_t verf;
	u_int32_t eof;
};

/* getdeviceinfo */
struct spnfs_data_server {
	u_int32_t dsid;
	char netid[5];
	char addr[29];
};

struct spnfs_device {
	u_int64_t devid;
	int dscount;
	struct spnfs_data_server dslist[SPNFS_MAX_DATA_SERVERS];
};

struct spnfs_msg_getdeviceinfo_args {
	u_int64_t devid;
};

struct spnfs_msg_getdeviceinfo_res {
	int status;
	struct spnfs_device devinfo;
};

/* setattr */
struct spnfs_msg_setattr_args {
	unsigned long inode;
	unsigned long generation;
	int file_size;
};

struct spnfs_msg_setattr_res {
	int status;
};

/* open */
struct spnfs_msg_open_args {
	unsigned long inode;
	unsigned long generation;
	int create;
	int createmode;
	int truncate;
};

struct spnfs_msg_open_res {
	int status;
};

/* close */
/* No op for daemon */
struct spnfs_msg_close_args {
	int x;
};

struct spnfs_msg_close_res {
	int y;
};

/* create */
/*
struct spnfs_msg_create_args {
	int x;
};

struct spnfs_msg_create_res {
	int y;
};
*/

/* remove */
struct spnfs_msg_remove_args {
	unsigned long inode;
	unsigned long generation;
};

struct spnfs_msg_remove_res {
	int status;
};

/* commit */
/*
struct spnfs_msg_commit_args {
	int x;
};

struct spnfs_msg_commit_res {
	int y;
};
*/

/* read */
struct spnfs_msg_read_args {
	unsigned long inode;
	unsigned long generation;
	loff_t offset;
	unsigned long len;
};

struct spnfs_msg_read_res {
	int status;
	char data[SPNFS_MAX_IO];
};

/* write */
struct spnfs_msg_write_args {
	unsigned long inode;
	unsigned long generation;
	loff_t offset;
	unsigned long len;
	char data[SPNFS_MAX_IO];
};

struct spnfs_msg_write_res {
	int status;
};

/* bundle args and responses */
union spnfs_msg_args {
	struct spnfs_msg_layoutget_args		layoutget_args;
	struct spnfs_msg_layoutcommit_args	layoutcommit_args;
/*
	struct spnfs_msg_layoutreturn_args	layoutreturn_args;
*/
	struct spnfs_msg_getdeviceiter_args     getdeviceiter_args;
	struct spnfs_msg_getdeviceinfo_args     getdeviceinfo_args;
	struct spnfs_msg_setattr_args		setattr_args;
	struct spnfs_msg_open_args		open_args;
	struct spnfs_msg_close_args		close_args;
/*
	struct spnfs_msg_create_args		create_args;
*/
	struct spnfs_msg_remove_args		remove_args;
/*
	struct spnfs_msg_commit_args		commit_args;
*/
	struct spnfs_msg_read_args		read_args;
	struct spnfs_msg_write_args		write_args;
};

union spnfs_msg_res {
	struct spnfs_msg_layoutget_res		layoutget_res;
	struct spnfs_msg_layoutcommit_res	layoutcommit_res;
/*
	struct spnfs_msg_layoutreturn_res	layoutreturn_res;
*/
	struct spnfs_msg_getdeviceiter_res      getdeviceiter_res;
	struct spnfs_msg_getdeviceinfo_res      getdeviceinfo_res;
	struct spnfs_msg_setattr_res		setattr_res;
	struct spnfs_msg_open_res		open_res;
	struct spnfs_msg_close_res		close_res;
/*
	struct spnfs_msg_create_res		create_res;
*/
	struct spnfs_msg_remove_res		remove_res;
/*
	struct spnfs_msg_commit_res		commit_res;
*/
	struct spnfs_msg_read_res		read_res;
	struct spnfs_msg_write_res		write_res;
};

/* a spnfs message, args and response */
struct spnfs_msg {
	unsigned char		im_type;
	unsigned char		im_status;
	union spnfs_msg_args	im_args;
	union spnfs_msg_res	im_res;
};

/* spnfs configuration info */
struct spnfs_config {
	unsigned char		dense_striping;
	int			stripe_size;
	int			num_ds;
	char			ds_dir[SPNFS_MAX_DATA_SERVERS][80];  /* XXX */
};

#ifdef __KERNEL__

/* pipe mgmt structure.  messages flow through here */
struct spnfs {
	char			spnfs_path[48];   /* path to pipe */
	struct dentry		*spnfs_dentry;    /* dentry for pipe */
	wait_queue_head_t	spnfs_wq;
	struct spnfs_msg	spnfs_im;         /* spnfs message */
	struct mutex		spnfs_lock;       /* Serializes upcalls */
	struct mutex		spnfs_plock;
};

int spnfs_layout_type(void);
int spnfs_layoutget(struct inode *, struct pnfs_layoutget_arg *);
int spnfs_layoutcommit(void);
int spnfs_layoutreturn(struct inode *, void *);
int spnfs_getdeviceiter(struct super_block *, struct pnfs_deviter_arg *);
int spnfs_getdeviceinfo(struct super_block *, struct pnfs_devinfo_arg *);
int spnfs_setattr(void);
int spnfs_open(struct inode *, void *);
int spnfs_close(struct inode *);
int spnfs_get_state(struct inode *, void *, void *);
int spnfs_remove(unsigned long, unsigned long);
int spnfs_read(struct inode *, loff_t, unsigned long *, int, struct svc_rqst *);
int spnfs_write(struct inode *, loff_t, size_t, int, struct svc_rqst *);
int spnfs_getfh(int, struct nfs_fh *);
int spnfs_test_layoutrecall(char *);
int spnfs_layoutrecall(struct inode *, int);

int nfsd_spnfs_new(void);
void nfsd_spnfs_delete(void);
int spnfs_upcall(struct spnfs *, struct spnfs_msg *, union spnfs_msg_res *);
int spnfs_enabled(void);
int nfs4_spnfs_propagate_open(struct super_block *, struct svc_fh *, void *);
int spnfs_init_proc(void);

#endif /* __KERNEL__ */

#endif /* NFS_SPNFS_H */
