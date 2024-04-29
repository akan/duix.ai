#include <stdlib.h>
#include "grtcfg.h"
#include "cJSON.h"
#include "grefmem.h"
#include "Log.h"

#define TAG  "tookencfg"
static int cjson_nval(cJSON* root,const char* name,int* val){
    if(!root||!val) return -1;
    cJSON* jobj=cJSON_GetObjectItem(root,name);
    if(!jobj)return -2;
    *val = jobj->valueint;
    return 0;
}

static int cjson_sval(cJSON* root,const char* name,char** val){
    if(!root||!val) return -1;
    cJSON* jobj=cJSON_GetObjectItem(root,name);
    if(!jobj)return -2;
    *val = jobj->valuestring;
    return 0;
}

static int cjson_nset(cJSON* root,const char* name,int val){
    if(!root||!name)return 0;
    cJSON_DeleteItemFromObject(root,name);
    cJSON_AddNumberToObject(root,name,val);
    return 0;
}

static int cjson_sset(cJSON* root,const char* name,char* val){
    if(!root||!name||!val)return 0;
    cJSON_DeleteItemFromObject(root,name);
    cJSON_AddStringToObject(root,name,val);
    return 0;
}

static int cjson_listsset(cJSON* root,char** name,char** val,int append){
    if(!root||!name||!val)return 0;
    char** pp=name; char** vv=val;
    char *p=*pp; char *v=*vv;
    int cnt=0;
    if(append){
        while(p&&vv){
            cJSON* jobj=cJSON_GetObjectItem(root,p);
            if(!jobj) cJSON_AddStringToObject(root,p,v);
            cnt++; p = *++pp; v = *++vv;
        }
    }else{
        while(p&&vv){
            cJSON_DeleteItemFromObject(root,p);
            cJSON_AddStringToObject(root,p,v);
            cnt++; p = *++pp; v = *++vv;
        }
    }
    return cnt;
}

static int cjson_listnset(cJSON* root,char** name,int* val,int append){
    if(!root||!name||!val)return 0;
    char** pp=name; int* vv=val;
    char *p=*pp; int v=*vv;
    int cnt=0;
    if(append){
        while(p&&vv){
            cJSON* jobj=cJSON_GetObjectItem(root,p);
            if(!jobj) cJSON_AddNumberToObject(root,p,v);
            cnt++; p = *++pp; v = *++vv;
        }
    }else{
        while(p&&vv){
            cJSON_DeleteItemFromObject(root,p);
            cJSON_AddNumberToObject(root,p,v);
            cnt++; p = *++pp; v = *++vv;
        }
    }
    return cnt;
}

static int cjson_listsval(cJSON* root,char** name,char*** val){
    if(!root||!name||!val)return 0;
    char** pp=name; char*** vv=val;
    char *p=*pp; char **v=*vv;
    int cnt=0;
    while(p&&v){
        cJSON* jobj=cJSON_GetObjectItem(root,p);
        if(jobj) *v = jobj->valuestring;
        cnt++; p = *++pp; v = *++vv;
    }
    return cnt;
}

static int cjson_listnval(cJSON* root,char** name,int** val){
    if(!root||!name||!val)return 0;
    char** pp=name; int** vv=val;
    char *p=*pp; int  *v=*vv;
    int cnt=0;
    while(p&&v){
        cJSON* jobj=cJSON_GetObjectItem(root,p);
        if(jobj) *v = jobj->valueint;
        p=*++pp; v=*++vv; cnt++;
    }
    return cnt;
}

static   char* g_ncfgname[] = {
        "action","videowidth", "videoheight", "timeoutms",
        NULL};

static   char* g_scfgname[] = {
        "defdir","wenetfn","unetbin","unetparam",
        "unetmsk","alphabin","alphaparam",
        "cacertfn","scrfdbin","scrfdparam",
        "pfpldbin","pfpldparam",
        NULL};

static void destroy_rtcfg(void* arg){
    rtcfg_t* cfg = (rtcfg_t*)arg;
    if(cfg->base_obj){
        cJSON* root = (cJSON*)cfg->base_obj;
        cJSON_Delete(root);
        cfg->base_obj = NULL;
    }
}



rtcfg_t* make_rtcfgjson(char* str){
    if(!str || !strlen(str))return NULL;
    cJSON* root = NULL;
    root = cJSON_Parse(str);
    if(!root){
        //LOGE(TAG,"===parse json error\n");
        return NULL;
    }
    rtcfg_t* cfg = NULL;
    gjrefobj_alloc(cfg,sizeof(rtcfg_t),destroy_rtcfg);
    memset(cfg,0,sizeof(rtcfg_t));
    cfg->base_obj = root;
    int* arrval[] = {
        &cfg->action, &cfg->videowidth, &cfg->videoheight,
        &cfg->timeoutms,
        NULL};
    cjson_listnval(root,g_ncfgname,arrval);
    char** arrstr[] = {
        &cfg->defdir,
        &cfg->wenetfn,
        &cfg->unetbin,
        &cfg->unetparam,
        &cfg->unetmsk,
        &cfg->alphabin,
        &cfg->alphaparam,
        &cfg->cacertfn,
        &cfg->scrfdbin,
        &cfg->scrfdparam,
        &cfg->pfpldbin,
        &cfg->pfpldparam,
        NULL,
    };
    cjson_listsval(root,g_scfgname,arrstr);
    return cfg;
}


int nval_rtcfg(rtcfg_t* cfg,char* name){
    if(!cfg->base_obj)return 0;
    cJSON* root = (cJSON*)cfg->base_obj;
    int rst = 0;
    cjson_nval(root,name,&rst);
    return rst;
}

char* sval_rtcfg(rtcfg_t* cfg,char* name){
    if(!cfg->base_obj)return NULL;
    cJSON* root = (cJSON*)cfg->base_obj;
    char* val = NULL;
    cjson_sval(root,name,&val);
    return val;
}

char* dump_rtcfg(rtcfg_t* cfg){
    //return cJSON_PrintUnformatted((cJSON*)cfg->base_obj);
    return cJSON_Print((cJSON*)cfg->base_obj);
}

int final_rtcfg(rtcfg_t* cfg){
    if(cfg) gjrefobj_unref(cfg);
    return 0;
}

#ifdef RTCFGTEST
#include <stdio.h>
int main(int argc,char** argv){
    rtcfg_t* cfg =  make_rtcfgpre(1080,0);
    char* txt = dump_rtcfg(cfg);
    printf("cfg %s\n",txt);
    free(txt);
    final_rtcfg(cfg);
    return 0;
}
#endif
