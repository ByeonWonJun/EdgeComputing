/* template implementation of an Edgex device service using C SDK */

/*
 * Copyright (c) 2018-2019
 * IoTech Ltd
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 */

#include "edgex/devsdk.h"

#include </usr/include/mysql/mysql.h>

#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>

#define ERR_CHECK(x) if (x.code) { fprintf (stderr, "Error: %d: %s\n", x.code, x.reason); edgex_device_service_free (service); free (impl); return x.code; }



typedef struct template_driver
{
  iot_logger_t * lc;
} template_driver;

static void dump_nvpairs (iot_logger_t *lc, const edgex_nvpairs *pairs)
{
  for (const edgex_nvpairs *a = pairs; a; a = a->next)
  {
    iot_log_debug (lc, "    %s = %s", a->name, a->value);
  }
}

static void dump_protocols (iot_logger_t *lc, const edgex_protocols *prots)
{
  for (const edgex_protocols *p = prots; p; p = p->next)
  {
    iot_log_debug (lc, " [%s] protocol:", p->name);
    dump_nvpairs (lc, p->properties);
  }
}

/* --- Initialize ---- */
/* Initialize performs protocol-specific initialization for the device
 * service.
 */
static bool template_init
(
  void *impl,
  struct iot_logger_t *lc,
  const edgex_nvpairs *config
)
{
  template_driver *driver = (template_driver *) impl;
  iot_log_debug (lc, "Template Init. Driver Config follows:");
  dump_nvpairs (lc, config);
  driver->lc = lc;
  iot_log_debug (lc, "Template Init done");
  return true;
}

/* ---- Discovery ---- */
/* Device services which are capable of device discovery should implement it
 * in this callback. It is called in response to a request on the
 * device service's discovery REST endpoint. New devices should be added using
 * the edgex_device_add_device() method
 */
static void template_discover (void *impl) {}

/* ---- Get ---- */
/* Get triggers an asynchronous protocol specific GET operation.
 * The device to query is specified by the protocols. nreadings is
 * the number of values being requested and defines the size of the requests
 * and readings arrays. For each value, the commandrequest holds information
 * as to what is being requested. The implementation of this method should
 * query the device accordingly and write the resulting value into the
 * commandresult.
 *
 * Note - In a commandrequest, the DeviceResource represents a deviceResource
 * which is defined in the device profile.
*/
static bool template_get_handler
(
  void *impl,
  const char *devname,
  const edgex_protocols *protocols,
  uint32_t nreadings,
  const edgex_device_commandrequest *requests,
  edgex_device_commandresult *readings
)
{
  template_driver *driver = (template_driver *) impl;
  
  char *server = "localhost";
  char *usr = "root";
  char *password = "78590q";
  char *database = "new_tracking";
  char query[100];
  char number[100];

  MYSQL *conn;
  MYSQL_RES *res;
  MYSQL_ROW row;
  

	conn = mysql_init(NULL);
	if(conn == NULL){
		printf("no");
		exit(1);
	}
	
	if(mysql_real_connect(conn, server, usr, password, database,0,NULL,0) == NULL){
		printf("error");
		exit(1);
	}
	

  /* Access the location of the device to be accessed and log it */
  iot_log_debug(driver->lc, "GET on device:");
  dump_protocols (driver->lc, protocols);

  for (uint32_t i = 0; i < nreadings; i++)
{
    const char *rdtype = edgex_nvpairs_value (requests[i].attributes, "type");

    if (rdtype)
    {
        if (strcmp (rdtype, "random") == 0)
        {
        /* Set the resulting reading type as Uint64 */
        readings[i].type = Edgex_Uint8;
        /* Set the reading as a random value between 0 and 100 */
        readings[i].value.ui8_result = rand() % 100;
         
        sprintf(query, "INSERT INTO new_tracking.test (name,number) VALUES ('values',%d)",readings[i].value.ui8_result);
        mysql_query(conn, query);
        /*
        if(mysql_query(conn, "INSERT INTO new_tracking.test (name,number) VALUES ('values','%s')")){
		printf("error 2 : %s\n", mysql_error(conn));
		exit(1);
	}
	*/	
        }
        else
        {
        iot_log_error (driver->lc, "Unknown sensor type \"%s\" requested", rdtype);
        return false;
        }
    }
    else
    {
        iot_log_error (driver->lc, "Unable to read value, no \"type\" attribute given");
        return false;
    }
}
mysql_close(conn);
return true;

}

/* ---- Put ---- */
/* Put triggers an asynchronous protocol specific SET operation.
 * The device to set values on is specified by the protocols.
 * nvalues is the number of values to be set and defines the size of the
 * requests and values arrays. For each value, the commandresult holds the
 * value, and the commandrequest holds information as to where it is to be
 * written. The implementation of this method should effect the write to the
 * device.
 *
 * Note - In a commandrequest, the DeviceResource represents a deviceResource
 * which is defined in the device profile.
*/
static bool template_put_handler
(
  void *impl,
  const char *devname,
  const edgex_protocols *protocols,
  uint32_t nvalues,
  const edgex_device_commandrequest *requests,
  const edgex_device_commandresult *values
  
)
{

  template_driver *driver = (template_driver *) impl;
  
  

  /* Access the location of the device to be accessed and log it */
  iot_log_debug (driver->lc, "PUT on device:");
  dump_protocols (driver->lc, protocols);

  for (uint32_t i = 0; i < nvalues; i++)
  {
    /* A Device Service again makes use of the data provided to perform a PUT */
    /* Log the attributes */
    iot_log_debug (driver->lc, "  Requested device write %u:", i);
    dump_nvpairs (driver->lc, requests[i].attributes);
    switch (values[i].type)
    {
      case String:
        iot_log_debug (driver->lc, "  Value: %s", values[i].value.string_result);
        break;
      case Uint8:
        iot_log_debug (driver->lc, "  Valuellll: %d", values[i].value.ui8_result);
        break;
      case Bool:
        iot_log_debug (driver->lc, "  Value: %s", values[i].value.bool_result ? "true" : "false");
        break;
      /* etc etc */
      default:
      break;
    }
  }
  return true;
}

/* ---- Disconnect ---- */
/* Disconnect handles protocol-specific cleanup when a device is removed. */
static bool template_disconnect (void *impl, edgex_protocols *device)
{
  return true;
}

/* ---- Stop ---- */
/* Stop performs any final actions before the device service is terminated */
static void template_stop (void *impl, bool force) {}

int main (int argc, char *argv[])
{
  edgex_device_svcparams params = { "device-template", NULL, NULL, NULL };
  sigset_t set;
  int sigret;
 
  template_driver * impl = malloc (sizeof (template_driver));
  memset (impl, 0, sizeof (template_driver));

  if (!edgex_device_service_processparams (&argc, argv, &params))
  {
    return  0;
  }

  int n = 1;
  while (n < argc)
  {
    if (strcmp (argv[n], "-h") == 0 || strcmp (argv[n], "--help") == 0)
    {
      printf ("Options:\n");
      printf ("  -h, --help\t\t: Show this text\n");
      edgex_device_service_usage ();
      return 0;
    }
    else
    {
      printf ("%s: Unrecognized option %s\n", argv[0], argv[n]);
      return 0;
    }
  }

  edgex_error e;
  e.code = 0;

  /* Device Callbacks */
  edgex_device_callbacks templateImpls =
  {
    template_init,         /* Initialize */
    template_discover,     /* Discovery */
    template_get_handler,  /* Get */
    template_put_handler,  /* Put */
    template_disconnect,   /* Disconnect */
    template_stop          /* Stop */
  };

  /* Initalise a new device service */
  edgex_device_service *service = edgex_device_service_new
    (params.svcname, "1.0", impl, templateImpls, &e);
  ERR_CHECK (e);

  /* Start the device service*/
  edgex_device_service_start (service, params.regURL, params.profile, params.confdir, &e);
  ERR_CHECK (e);

  /* Wait for interrupt */
  sigemptyset (&set);
  sigaddset (&set, SIGINT);
  sigwait (&set, &sigret);

  /* Stop the device service */
  edgex_device_service_stop (service, true, &e);
  ERR_CHECK (e);


  edgex_device_service_free (service);
  free (impl);
  return 0;
}
