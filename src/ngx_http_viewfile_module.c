#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#define NGX_MODULE_NAME ngx_http_viewfile_module
#define MY_NGX_MODULE_FUNCTION(name) ngx_http_viewfile_##name

static void * MY_NGX_MODULE_FUNCTION(create_loc_conf) (ngx_conf_t *cf);
static char * MY_NGX_MODULE_FUNCTION(merge_loc_conf) (ngx_conf_t *cf, void *parent, void *child);
static char * MY_NGX_MODULE_FUNCTION(command_aclcheck) (ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_int_t MY_NGX_MODULE_FUNCTION(post_conf) (ngx_conf_t *cf);

static ngx_int_t MY_NGX_MODULE_FUNCTION(handler) (ngx_http_request_t *r);

typedef struct {
    ngx_uint_t  aclcheck;
} ngx_http_fileacl_loc_conf_t;

static ngx_http_module_t  ngx_http_fileacl_module_ctx = {
    NULL,                                  /* preconfiguration */
    MY_NGX_MODULE_FUNCTION(post_conf),        /* postconfiguration */

    NULL,                                  /* create main configuration */
    NULL,                                  /* init main configuration */

    NULL,                                  /* create server configuration */
    NULL,                                  /* merge server configuration */

    MY_NGX_MODULE_FUNCTION(create_loc_conf),  /* create location configuration */
    MY_NGX_MODULE_FUNCTION(merge_loc_conf)    /* merge location configuration */
};

static ngx_command_t  ngx_http_fileacl_commands[] = {

    { ngx_string("file_aclcheck"),
      NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
      MY_NGX_MODULE_FUNCTION(command_aclcheck),
      NGX_HTTP_LOC_CONF_OFFSET,                                 /* which conf */
      offsetof(ngx_http_fileacl_loc_conf_t, aclcheck),          /* field offset */
      NULL                                                      /* post config handler */
    },

    ngx_null_command
};


ngx_module_t NGX_MODULE_NAME = {
    NGX_MODULE_V1,
    &ngx_http_fileacl_module_ctx,          /* module context */
    ngx_http_fileacl_commands,             /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};


/* Start Conf */

static void* MY_NGX_MODULE_FUNCTION(create_loc_conf) (ngx_conf_t *cf){
  ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "create config \"%s\"", "file_aclcheck");

  ngx_http_fileacl_loc_conf_t  *conf;
  conf = ngx_pcalloc(cf->pool, sizeof(ngx_http_fileacl_loc_conf_t));  
  if (conf == NULL) {
        return NULL;
  }

  // conf->aclcheck = NGX_CONF_UNSET_UINT;
  conf->aclcheck = 0;

  return conf;
}

static char* MY_NGX_MODULE_FUNCTION(merge_loc_conf) (ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_fileacl_loc_conf_t *prev = parent;
    ngx_http_fileacl_loc_conf_t *conf = child;

    ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "merge config \"%s\" = %d %d", "file_aclcheck", conf->aclcheck, prev->aclcheck);

    if(0){
      ngx_conf_merge_uint_value(conf->aclcheck, prev->aclcheck, 1);
    }

    return NGX_CONF_OK;
}

static char * MY_NGX_MODULE_FUNCTION(command_aclcheck) (ngx_conf_t *cf, ngx_command_t *cmd, void *conf){
   ngx_http_fileacl_loc_conf_t *loc_conf = conf;

   ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "apply config \"%s\"", "file_aclcheck");

   loc_conf->aclcheck = 1;

  return NGX_CONF_OK;
}
/* End Conf */


static ngx_int_t MY_NGX_MODULE_FUNCTION(post_conf) (ngx_conf_t *cf)
{
    ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "post config \"%s\"", "file_aclcheck");

    ngx_http_fileacl_loc_conf_t *fileacl_conf;
    fileacl_conf = ngx_http_conf_get_module_loc_conf(cf, NGX_MODULE_NAME);


    if(1 || fileacl_conf->aclcheck){
      ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "init handler for \"%s\"", "file_aclcheck");

      ngx_http_core_main_conf_t  *cmcf;
      cmcf = ngx_http_conf_get_module_main_conf(cf, ngx_http_core_module);

      ngx_http_handler_pt *h = ngx_array_push(&cmcf->phases[NGX_HTTP_PREACCESS_PHASE].handlers);
      if (h == NULL) {
          return NGX_ERROR;
      }

      ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "append handler \"%s\"", "ngx_http_fileacl_handler");
      *h = MY_NGX_MODULE_FUNCTION(handler);
    } else {
      ngx_log_error(NGX_LOG_EMERG, cf->log, 0, "skip handler for \"%s\"", "file_aclcheck");
    }

    return NGX_OK;
}

static ngx_int_t MY_NGX_MODULE_FUNCTION(handler) (ngx_http_request_t *r)
{
    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "handle request \"%s\"", "file_aclcheck");

    ngx_http_fileacl_loc_conf_t *fileacl_conf;
    fileacl_conf = ngx_http_get_module_loc_conf(r, NGX_MODULE_NAME);


    if(fileacl_conf && fileacl_conf->aclcheck){
      ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "deny due to \"%s\"", "file_aclcheck");
      return NGX_HTTP_FORBIDDEN;
    }

    ngx_log_error(NGX_LOG_EMERG, r->connection->log, 0, "declined due to \"%s\"", "file_aclcheck");

    return NGX_DECLINED;
}


