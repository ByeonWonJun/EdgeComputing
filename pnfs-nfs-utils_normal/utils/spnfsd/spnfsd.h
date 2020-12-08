/*
 * spnfsd.h
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

#ifndef _PATH_SPNFSDCONF
#define _PATH_SPNFSDCONF "/etc/spnfsd.conf"
#endif

#ifndef DEFAULT_STRIPE_SIZE
#define DEFAULT_STRIPE_SIZE 4096
#endif

#ifndef DEFAULT_DS_PORT
#define DEFAULT_DS_PORT 2049
#endif

struct dserver {
        char	*ds_ip;
        int	ds_port;
        char	*ds_path;
	int	ds_id;
};

/* DMXXX future struct for whole config */
struct spnfsd_config {
	int		verbose;
	int		stripesize;
	int		densestriping;
	int		num_ds;
	struct dserver	dataservers[SPNFS_MAX_DATA_SERVERS];
};

int spnfsd_layoutget(struct spnfs_msg *);
int spnfsd_layoutcommit(struct spnfs_msg *);
int spnfsd_layoutreturn(struct spnfs_msg *);
int spnfsd_getdeviceiter(struct spnfs_msg *);
int spnfsd_getdeviceinfo(struct spnfs_msg *);
int spnfsd_setattr(struct spnfs_msg *);
int spnfsd_open(struct spnfs_msg *);
int spnfsd_close(struct spnfs_msg *);
int spnfsd_create(struct spnfs_msg *);
int spnfsd_remove(struct spnfs_msg *);
int spnfsd_commit(struct spnfs_msg *);
int spnfsd_read(struct spnfs_msg *);
int spnfsd_write(struct spnfs_msg *);
int spnfsd_getfh(char *, unsigned char *, unsigned int *);
