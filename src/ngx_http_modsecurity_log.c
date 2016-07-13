/*
 * ModSecurity connector for nginx, http://www.modsecurity.org/
 * Copyright (c) 2015 Trustwave Holdings, Inc. (http://www.trustwave.com/)
 *
 * You may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * If any of the files related to licensing are missing or if you have any
 * other questions related to licensing please contact Trustwave Holdings, Inc.
 * directly using the email address security@modsecurity.org.
 *
 */


#include "ddebug.h"
#ifndef DDEBUG
#define DDEBUG 0
#endif


#include <nginx.h>
#include "ngx_http_modsecurity_log.h"


void ngx_http_modsecurity_log(void *log, const char* msg)
{
    if (log == NULL)
    {
        return;
    }

    ngx_log_error(NGX_LOG_INFO, (ngx_log_t *)log, 0, msg);
}


ngx_int_t ngx_http_modsecurity_log_handler(ngx_http_request_t *r)
{
    ngx_http_modsecurity_ctx_t *ctx = NULL;
    ngx_http_modsecurity_loc_conf_t *cf;

    dd("catching a new _log_ pahase handler");

    cf = ngx_http_get_module_loc_conf(r, ngx_http_modsecurity);
    if (cf == NULL || cf->enable != 1)
    {
        dd("ModSecurity not enabled... returning");
        return NGX_OK;
    }

    if (r->method != NGX_HTTP_GET && r->method != NGX_HTTP_POST) {
        dd("ModSecurity is not ready to deal with anything different from " \
            "POST or GET");
        return NGX_OK;
    }

    ctx = ngx_http_get_module_ctx(r, ngx_http_modsecurity);

    dd("recovering ctx: %p", ctx);

    if (ctx == NULL)
    {
        dd("something really bad happened here. returning NGX_ERROR");
        return NGX_ERROR;
    }

    dd("calling msc_process_logging for %p", ctx);
    msc_process_logging(ctx->modsec_transaction);

    return NGX_OK;
}
