/*
 * vim:noexpandtab:shiftwidth=8:tabstop=8:
 *
 * Copyright CEA/DAM/DIF  (2008)
 * contributeur : Philippe DENIEL   philippe.deniel@cea.fr
 *                Thomas LEIBOVICI  thomas.leibovici@cea.fr
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 * ---------------------------------------
 */

/**
 * file    mnt_Dump.c
 * brief   MOUNTPROC_Dump for Mount protocol v1 and v3.
 *
 */
#include "config.h"
#include "log.h"
#include "nfs_core.h"
#include "nfs_exports.h"
#include "mount.h"
#include "nfs_proto_functions.h"

/**
 * @brief The Mount proc Dump function, for all versions.
 *
 * @param[in]  parg     Arguments (ignored)
 * @param[in]  preq     Ignored
 * @param[out] pres     Pointer to results
 *
 */

int mnt_Dump(nfs_arg_t *arg, struct svc_req *req, nfs_res_t *res)
{
	LogDebug(COMPONENT_NFSPROTO, "REQUEST PROCESSING: Calling MNT_DUMP");

	/* Ganesha does not support the mount list so this is a NOOP */

	res->res_dump = NULL;

	return NFS_REQ_OK;
}				/* mnt_Null */

/**
 * mnt_Dump_Free: Frees the result structure allocated for mnt_Dump.
 *
 * Frees the result structure allocated for mnt_Dump.
 *
 * @param res        [INOUT]   Pointer to the result structure.
 *
 */
void mnt_Dump_Free(nfs_res_t *res)
{
	/* Nothing to do */
}
