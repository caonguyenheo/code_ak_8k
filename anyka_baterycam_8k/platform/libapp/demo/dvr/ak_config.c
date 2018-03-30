#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "ak_config.h"
#include "ak_ini.h"
#include "ak_common.h"
#include "ak_time.h"
//#include "PTZControl.h"
//#include "font_lib.h"

#include "kernel.h"

#define BUF_SIZE (100)
#define CONFIG_VALUE_BUF_SIZE		50
#define CONFIG_DUMP_INFO			0

#define CONFIG_ANYKA_FILE_NAME  "/etc/jffs2/anyka_cfg.ini"
#define CONFIG_PTZ_UNHIT_NAME  "/tmp/ak_sys_start_flag"

struct ptz_pos {
	int left;
	int up;
};

struct sys_config {
	void *handle;

	struct sys_user_config *user;
	struct video_config *video;
	struct video_record_config *record_plan;
	struct sys_alarm_config *alarm;
	struct sys_cloud_config *cloud;
	struct sys_onvif_config *onvif;
	struct sys_net_config *net;
	struct sys_wifi_config *wifi;
	struct camera_disp_config *camera;
};

static struct sys_config config = {NULL};

/** save_week_config_info: enable appointed weed day
 * @day_index: appointed day index
 * @week[OUT]: week day info
 * return: none
*/
static void enable_week_day(int day_index, int *week)
{
	if(day_index >= DAY_PER_WEEK){
		week[0] = 0x01;
	}else{
		week[day_index] = 0x01;
	}
}

/** enable_week_day: transfer string value to week day value
 * @value: week day string
 * @week[OUT]: week day info
 * return: none
 */
static void save_week_config_info(char *value, int *week)
{
	int i = 0;
	int index = 0;
	int str_len = strlen(value);
	
	for(i=0; i<str_len; ++i){
		switch(value[i]){
		case '-':
			if(0x00 == i){
				ak_print_normal_ex("week active config INVALID\n");
			}else{
				int start_day = atoi(&value[i-1]);	//we reset start day
				int end_day = atoi(&value[i+1]);

 				if(start_day < 0x01){
					start_day = 0x01;
				}
				if(end_day > 0x07){
					end_day = 0x07;
				}

				/* including end_day, ex: 1-7 is Monday to Sunday */
				for(index=start_day; index<=end_day; ++index){
					enable_week_day(index, week);
				}
				
				++i;//skip number after '-'
			}
			break;
		case ',':	//just skip ','
			break;
		default:
			/* single week day */
			index = atoi(&value[i]);
			enable_week_day(index, week);
			break;
		}
	}
}

struct video_config* ak_config_get_sys_video(void)
{
    return config.video;
}

struct video_record_config *ak_config_get_record_plan(void)
{
    return config.record_plan;
}

static void init_video_param(struct video_config *video)
{
	char value[CONFIG_VALUE_BUF_SIZE] = {0};
	
	/* get all item values of video title, 
		using default value if doesn't exist */
    ak_ini_get_item_value(config.handle, "video", "minqp", value);     
    video->minQp = atoi(value);
    ak_print_normal("video->minQp=%d\n",video->minQp);
    ak_ini_get_item_value(config.handle, "video", "maxqp", value);  
    ak_print_normal("video->minQp=%d\n",video->minQp);
    video->maxQp = atoi(value);
    ak_print_normal("video->minQp=%d\n",video->minQp);
    ak_ini_get_item_value(config.handle, "video", "v720pfps", value);  
    video->V720Pfps = atoi(value);
    ak_ini_get_item_value(config.handle, "video", "v720pminkbps", value);  
    video->V720Pminkbps = atoi(value);
    ak_ini_get_item_value(config.handle, "video", "v720pmaxkbps", value);  
    video->V720Pmaxkbps = atoi(value);
    ak_ini_get_item_value(config.handle, "video", "save_cyc_flag", value);  
    video->save_cyc_flag = atoi(value);
    ak_ini_get_item_value(config.handle, "video", "savefilefps", value);  
    video->savefilefps = atoi(value);
    ak_ini_get_item_value(config.handle, "video", "savefilekbps", value);  
    video->savefilekbps = atoi(value);
   	ak_ini_get_item_value(config.handle, "video", "minfilefps", value);  
    video->minfilefps = atoi(value);
    ak_ini_get_item_value(config.handle, "video", "recpath", value);  
    strcpy(video->recpath , value);
    ak_ini_get_item_value(config.handle, "video", "vgapfps", value);  
    video->VGAPfps = atoi(value);
    ak_ini_get_item_value(config.handle, "video", "vgaminkbps", value);  
    video->VGAminkbps = atoi(value);
    ak_ini_get_item_value(config.handle, "video", "vgamaxkbps", value);  
    video->VGAmaxkbps = atoi(value);
    ak_ini_get_item_value(config.handle, "video", "goplen", value);  
    video->gopLen = atoi(value);
    ak_ini_get_item_value(config.handle, "video", "quality", value);  
    video->quality = atoi(value);
    ak_ini_get_item_value(config.handle, "video", "pic_ch", value);  
    video->pic_ch = atoi(value);	
    ak_ini_get_item_value(config.handle, "video", "video_mode", value);  
    video->video_mode = atoi(value);

//    video->minQp = atoi("40");
    ak_print_normal("video->minQp=%d,video->maxQp=%d \n ",video->minQp,video->maxQp);
}

static void init_sys_alarm(struct sys_alarm_config *alarm)
{
	char value[CONFIG_VALUE_BUF_SIZE] = {0};
	
    ak_ini_get_item_value(config.handle, "alarm", "motion_detection", value);  
    alarm->motion_detection = atoi(value);
    ak_ini_get_item_value(config.handle, "alarm", "motion_detection_1", value);  
    alarm->motion_detection_1 = atoi(value);
    ak_ini_get_item_value(config.handle, "alarm", "motion_detection_2", value);  
    alarm->motion_detection_2 = atoi(value);
    ak_ini_get_item_value(config.handle, "alarm", "motion_detection_3", value);  
    alarm->motion_detection_3 = atoi(value);
    ak_ini_get_item_value(config.handle, "alarm", "opensound_detection", value);  
    alarm->opensound_detection = atoi(value);
    ak_ini_get_item_value(config.handle, "alarm", "opensound_detection_1", value);  
    alarm->opensound_detection_1 = atoi(value);
    ak_ini_get_item_value(config.handle, "alarm", "opensound_detection_2", value);  
    alarm->opensound_detection_2 = atoi(value);
    ak_ini_get_item_value(config.handle, "alarm", "opensound_detection_3", value);  
    alarm->opensound_detection_3 = atoi(value);
    ak_ini_get_item_value(config.handle, "alarm", "openi2o_detection", value);  
    alarm->openi2o_detection = atoi(value);
    ak_ini_get_item_value(config.handle, "alarm", "smoke_detection", value);  
    alarm->smoke_detection = atoi(value);
    ak_ini_get_item_value(config.handle, "alarm", "shadow_detection", value);  
    alarm->shadow_detection = atoi(value);
    ak_ini_get_item_value(config.handle, "alarm", "other_detection", value);  
    alarm->other_detection = atoi(value);
    ak_ini_get_item_value(config.handle, "alarm", "alarm_send_type", value);  
    alarm->send_type = atoi(value);	
    ak_ini_get_item_value(config.handle, "alarm", "alarm_interval_time", value);  
    alarm->interval_time = atoi(value);
    ak_ini_get_item_value(config.handle, "alarm", "alarm_move_over_time", value);  
    alarm->move_over_time = atoi(value);
    ak_ini_get_item_value(config.handle, "alarm", "alarm_send_msg_time", value);  
    alarm->send_msg_time = atoi(value);	
    ak_ini_get_item_value(config.handle, "alarm", "alarm_save_record", value);  
    alarm->save_record = atoi(value);
    ak_ini_get_item_value(config.handle, "alarm", "motion_size_x", value);  
    alarm->motion_size_x = atoi(value);
    ak_ini_get_item_value(config.handle, "alarm", "motion_size_y", value);
    alarm->motion_size_y = atoi(value);
}

static void init_sys_net(struct sys_net_config *net)
{
    char value[CONFIG_VALUE_BUF_SIZE] = {0};
    
    ak_ini_get_item_value(config.handle, "ethernet", "dhcp", value);  
    net->dhcp = atoi(value);
    ak_ini_get_item_value(config.handle, "ethernet", "ipaddr", value);  
    strcpy(net->ipaddr, value);
    ak_ini_get_item_value(config.handle, "ethernet", "netmask", value);  
    strcpy(net->netmask, value);
    ak_ini_get_item_value(config.handle, "ethernet", "gateway", value);  
    strcpy(net->gateway, value);
    ak_ini_get_item_value(config.handle, "ethernet", "firstdns", value);  
    strcpy(net->firstdns, value);
    ak_ini_get_item_value(config.handle, "ethernet", "backdns", value);  
    strcpy(net->backdns, value);
}

static void get_net_info(void)
{ 
    system("ifconfig eth0 >> /tmp/eth.txt");	//save ethnet info 
    int eth0_file;
    char *net_info = (char *)malloc(4096);
    char *find_start = NULL;
    char *find_end = NULL;
	int i = 0;
	char gateway[64]={0};
				
    if(net_info == NULL){
        return;
    }
    eth0_file = open("/tmp/eth.txt", O_RDWR);	//open the info file 
    if(eth0_file < 0) {
        free(net_info);
        return ;
    }
    memset(net_info, 0, 4096);
    read(eth0_file, net_info, 4096);

    /* find lable */
    if(NULL != (find_start = strstr(net_info, "inet addr:"))){
        find_start += strlen("inet addr:");
        find_end= strstr(find_start , " ");
		/** get info we need **/
        memset(config.net->ipaddr, 0, sizeof(config.net->ipaddr));
        memcpy(config.net->ipaddr, find_start, (unsigned long)find_end - (unsigned long)find_start);
    }
    
    if(NULL != (find_start = strstr(net_info, "Mask:"))){
        find_start += strlen("Mask:");	//find lable
        find_end= strstr(find_start , "\x0a");
		/** get info we need **/
        memset(config.net->netmask, 0, sizeof(config.net->netmask));
        memcpy(config.net->netmask, find_start, (unsigned long)find_end - (unsigned long)find_start);
    }
    close(eth0_file);
    free(net_info);
    system("rm -rf /tmp/eth.txt");	//remove the info file 

//	do_sys_cmd("route | grep 'default' | awk '{print $2}'", 
//		sizeof(gateway), gateway); // lgd remove
    while(gateway[i]){
        if (gateway[i]=='\n' || gateway[i]=='\r' || gateway[i]==' ')
            gateway[i] = '\0';
        i++;
    }

    memset(config.net->gateway, 0, sizeof(config.net->gateway));
    strcpy(config.net->gateway, gateway);
}  

struct sys_net_config* ak_config_get_sys_net(void)
{
#if 0
//isn't suitable to to rtos, remove
    if(config.net->dhcp == 1){
        get_net_info();
    }

#endif    
    return config.net;
}

void ak_config_set_sys_net(struct sys_net_config *net)
{
    char value[CONFIG_VALUE_BUF_SIZE] = {0};
	
	memcpy(config.net, net, sizeof(struct sys_net_config));
    sprintf(value,"%d", config.net->dhcp);
    ak_ini_set_item_value(config.handle, "ethernet", "dhcp", value);  
    sprintf(value,"%s", config.net->ipaddr);
    ak_ini_set_item_value(config.handle, "ethernet", "ipaddr", value);  
    sprintf(value,"%s", config.net->netmask);
    ak_ini_set_item_value(config.handle, "ethernet", "netmask", value);  
    sprintf(value,"%s", config.net->gateway);
    ak_ini_set_item_value(config.handle, "ethernet", "gateway", value);  
    sprintf(value,"%s", config.net->firstdns);
    ak_ini_set_item_value(config.handle, "ethernet", "firstdns", value);  
    sprintf(value,"%s", config.net->backdns);
    ak_ini_set_item_value(config.handle, "ethernet", "backdns", value);
}

static void init_sys_wifi(struct sys_wifi_config * net)
{
	char value[CONFIG_VALUE_BUF_SIZE] = {0};
	
	ak_ini_get_item_value(config.handle, "softap", "s_ssid", value);	
	strcpy(net->ssid, value);
	ak_ini_get_item_value(config.handle, "softap", "s_password", value);	
	strcpy(net->ssid, value);
	
    ak_ini_get_item_value(config.handle, "wireless", "ssid", value);  	
    strcpy(net->ssid, value);
    ak_ini_get_item_value(config.handle, "wireless", "mode", value);  
    strcpy(net->mode, value);
    ak_ini_get_item_value(config.handle, "wireless", "password", value);  
    strcpy(net->passwd, value);
    ak_ini_get_item_value(config.handle, "wireless", "security", value);  
	if(strstr(value, "WPA")){
		net->enc_type = WIFI_ENCTYPE_WPA2_TKIP;
	}else if(strstr(value, "WEP")){
		net->enc_type = WIFI_ENCTYPE_WEP;
	}else{
		net->enc_type = WIFI_ENCTYPE_NONE;
	}
}

struct sys_wifi_config* ak_config_get_sys_wifi(void)
{
    return config.wifi;
}

void ak_config_set_sys_wifi(struct sys_wifi_config *wifi)
{
    char value[CONFIG_VALUE_BUF_SIZE] = {0};

    memcpy(config.wifi, wifi, sizeof(struct sys_wifi_config)); 
	//the ssid save to /tmp/wireless/
    //sprintf(value,"%s", config.wifi->ssid );
    //ak_ini_set_item_value(config.handle, "wireless", "ssid", value); 
    sprintf(value,"%s", config.wifi->passwd );
    ak_ini_set_item_value(config.handle, "wireless", "password", value); 
}

static void init_sys_onvif(struct sys_onvif_config * onvif)
{
	char value[CONFIG_VALUE_BUF_SIZE] = {0};
	
    ak_ini_get_item_value(config.handle, "onvif", "fps1", value);  
    onvif->fps1 = atoi(value);
    ak_ini_get_item_value(config.handle, "onvif", "kbps1", value);  
    onvif->kbps1 = atoi(value);
    ak_ini_get_item_value(config.handle, "onvif", "quality1", value);  
    onvif->quality1 = atoi(value);
    ak_ini_get_item_value(config.handle, "onvif", "fps2", value);  
    onvif->fps2 = atoi(value);
    ak_ini_get_item_value(config.handle, "onvif", "kbps2", value);  
    onvif->kbps2 = atoi(value);
    ak_ini_get_item_value(config.handle, "onvif", "quality2", value);  
    onvif->quality2 = atoi(value);    
}

struct sys_onvif_config* ak_config_get_sys_onvif(void)
{
    return config.onvif;
}

void ak_config_set_sys_onvif(struct sys_onvif_config *onvif)
{
    char value[CONFIG_VALUE_BUF_SIZE] = {0};
    
	memcpy(config.onvif, onvif, sizeof(struct sys_onvif_config));
    sprintf(value, "%d", config.onvif->fps1 );
    ak_ini_set_item_value(config.handle, "onvif", "fps1", value);  
    sprintf(value, "%d", config.onvif->kbps1);
    ak_ini_set_item_value(config.handle, "onvif", "kbps1", value);  
    sprintf(value, "%d", config.onvif->quality1); 
    ak_ini_set_item_value(config.handle, "onvif", "quality1", value);  
    sprintf(value, "%d", config.onvif->fps2 );
    ak_ini_set_item_value(config.handle, "onvif", "fps2", value);  
    sprintf(value, "%d", config.onvif->kbps2);
    ak_ini_set_item_value(config.handle, "onvif", "kbps2", value);  
    sprintf(value, "%d", config.onvif->quality2);
    ak_ini_set_item_value(config.handle, "onvif", "quality2", value);
}

static void init_sys_cloud_config(struct sys_cloud_config *cloud)
{
    char value[CONFIG_VALUE_BUF_SIZE] = {0};
    
#ifdef CONFIG_ONVIF_SUPPORT
    ak_ini_get_item_value(config.handle, "cloud", "onvif", value);  
#else
	ak_ini_get_item_value(config.handle, "cloud", "onvif", value);  
#endif
    cloud->onvif = atoi(value);

#ifdef CONFIG_DANA_SUPPORT
    ak_ini_get_item_value(config.handle, "cloud", "dana", value);  
#else
	ak_ini_get_item_value(config.handle, "cloud", "dana", value);  
#endif
    cloud->dana = atoi(value);
    
    ak_ini_get_item_value(config.handle, "cloud", "tutk", value);  
    cloud->tutk = atoi(value);
    
#ifdef CONFIG_TENCENT_SUPPORT
    ak_ini_get_item_value(config.handle, "cloud", "tencent", value);  
#else
    ak_ini_get_item_value(config.handle, "cloud", "tencent", value);  
#endif
    cloud->tencent= atoi(value);
}

struct sys_cloud_config* ak_config_get_sys_cloud(void)
{
    return config.cloud;
}

struct sys_alarm_config* ak_config_get_sys_alarm(void)
{
    return config.alarm;
}

void ak_config_set_sys_alarm(void)
{
	char value[CONFIG_VALUE_BUF_SIZE] = {0};
	
	sprintf(value,"%d", config.alarm->motion_detection );
    ak_ini_set_item_value(config.handle, "alarm", "motion_detection", value);  
    sprintf(value,"%d", config.alarm->motion_detection_1 );
    ak_ini_set_item_value(config.handle, "alarm", "motion_detection_1", value);  
    sprintf(value,"%d", config.alarm->motion_detection_2 );
    ak_ini_set_item_value(config.handle, "alarm", "motion_detection_2", value);  
    sprintf(value,"%d", config.alarm->motion_detection_3 );
    ak_ini_set_item_value(config.handle, "alarm", "motion_detection_3", value);  
    sprintf(value,"%d", config.alarm->opensound_detection );
    ak_ini_set_item_value(config.handle, "alarm", "opensound_detection", value);  
    sprintf(value,"%d", config.alarm->opensound_detection_1 );
    ak_ini_set_item_value(config.handle, "alarm", "opensound_detection_1", value);  
    sprintf(value,"%d", config.alarm->opensound_detection_2 );
    ak_ini_set_item_value(config.handle, "alarm", "opensound_detection_2", value);  
    sprintf(value,"%d", config.alarm->opensound_detection_3 );
    ak_ini_set_item_value(config.handle, "alarm", "opensound_detection_3", value);  
    sprintf(value,"%d", config.alarm->openi2o_detection );
    ak_ini_set_item_value(config.handle, "alarm", "openi2o_detection", value);  
    sprintf(value,"%d", config.alarm->shadow_detection );
    ak_ini_set_item_value(config.handle, "alarm", "shadow_detection", value);  
    sprintf(value,"%d", config.alarm->shadow_detection );
    ak_ini_set_item_value(config.handle, "alarm", "shadow_detection", value);  
    sprintf(value,"%d", config.alarm->other_detection );
    ak_ini_set_item_value(config.handle, "alarm", "other_detection", value);  
    sprintf(value,"%d", config.alarm->send_type );
    ak_ini_set_item_value(config.handle, "alarm", "alarm_send_type", value); 
}

static void init_record_plan(struct video_record_config *record)
{
    #if 0
    //lgd remove
	char value[CONFIG_VALUE_BUF_SIZE] = {0};
	
    ak_ini_get_item_value(config.handle, "record", "video_index", value);  
    record->video_index = atoi(value);

    /* record plan 1 */
    ak_ini_get_item_value(config.handle, "record", "record_start_run1", value);  
    record->plan[0].active = atoi(value);
    ak_ini_get_item_value(config.handle, "record", "record_start_time1", value);  
    record->plan[0].start_time = day_clock_to_seconds(value);
    ak_ini_get_item_value(config.handle, "record", "record_end_time1", value);  
    record->plan[0].end_time = day_clock_to_seconds(value);
    
    ak_ini_get_item_value(config.handle, "record", "record_week_active1", value);  
    memset(record->plan[0].week_enalbe, 0x00, sizeof(record->plan[0].week_enalbe));
    save_week_config_info(value, record->plan[0].week_enalbe);
    
    ak_ini_get_item_value(config.handle, "record", "record_time", value);  
    record->plan[0].record_time = atoi(value);

	/* record plan 2 */
    ak_ini_get_item_value(config.handle, "record", "record_start_run2", value);  
    record->plan[1].active = atoi(value);
    ak_ini_get_item_value(config.handle, "record", "record_start_time2", value);  
    record->plan[1].start_time = day_clock_to_seconds(value);
    ak_ini_get_item_value(config.handle, "record", "record_end_time2", value);  
    record->plan[1].end_time = day_clock_to_seconds(value);
    
    ak_ini_get_item_value(config.handle, "record", "record_week_active2", value);  
    memset(record->plan[1].week_enalbe, 0x00, sizeof(record->plan[1].week_enalbe));
    save_week_config_info(value, record->plan[1].week_enalbe);

	/* record plan 3 */
    ak_ini_get_item_value(config.handle, "record", "record_start_run3", value);  
    record->plan[2].active = atoi(value);
    ak_ini_get_item_value(config.handle, "record", "record_start_time3", value);  
    record->plan[2].start_time = day_clock_to_seconds(value);
    
    ak_ini_get_item_value(config.handle, "record", "record_end_time3", value);  
    record->plan[2].end_time = day_clock_to_seconds(value);
    
    ak_ini_get_item_value(config.handle, "record", "record_week_active3", value);  
   	memset(record->plan[2].week_enalbe, 0x00, sizeof(record->plan[2].week_enalbe));
   	save_week_config_info(value, record->plan[2].week_enalbe);

   	#endif
}

void ak_config_set_record_plan(struct video_record_config *record)
{
	char value[CONFIG_VALUE_BUF_SIZE] = {0};
	
    sprintf(value, "%d", record->video_index );     
    ak_ini_set_item_value(config.handle, "record", "video_index", value);  
    sprintf(value, "%d", record->plan[0].active );
    ak_ini_set_item_value(config.handle, "record", "record_start_run1", value);  
    sprintf(value, "%ld", record->plan[0].start_time);
    ak_ini_set_item_value(config.handle, "record", "record_start_time1", value);  
    sprintf(value, "%ld", record->plan[0].end_time);
    ak_ini_set_item_value(config.handle, "record", "record_end_time1", value);  
    //sprintf(value, "%d", record->plan[0].week_enalbe);
    ak_ini_set_item_value(config.handle, "record", "record_cyc1", value);   
    sprintf(value, "%d", record->plan[1].active);
    ak_ini_set_item_value(config.handle, "record", "record_start_run2", value);  
    sprintf(value, "%ld", record->plan[1].start_time);
    ak_ini_set_item_value(config.handle, "record", "record_start_time2", value);  
    sprintf(value, "%ld", record->plan[1].end_time);
    ak_ini_set_item_value(config.handle, "record", "record_end_time2", value);  
    //sprintf(value, "%d", record->plan[1].week_enalbe);
    ak_ini_set_item_value(config.handle, "record", "record_cyc2", value);  
    sprintf(value, "%d", record->plan[2].active);
    ak_ini_set_item_value(config.handle, "record", "record_start_run3", value);  
    sprintf(value, "%ld", record->plan[2].start_time);
    ak_ini_set_item_value(config.handle, "record", "record_start_time3", value);  
    sprintf(value, "%ld", record->plan[2].end_time);
    ak_ini_set_item_value(config.handle, "record", "record_end_time3", value);  
    //sprintf(value, "%d", record->plan[2].week_enalbe);
    ak_ini_set_item_value(config.handle, "record", "record_cyc3", value);  
}

static void init_camera_config(struct camera_disp_config * camera)
{
	char value[CONFIG_VALUE_BUF_SIZE] = {0};
	
    ak_ini_get_item_value(config.handle, "camera", "width", value);  
    camera->width = atoi(value); 
    ak_ini_get_item_value(config.handle, "camera", "height", value);  
    camera->height = atoi(value); 
    ak_ini_get_item_value(config.handle, "camera", "osd_position", value);  
    camera->osd_position = atoi(value);
    ak_ini_get_item_value(config.handle, "camera", "osd_switch", value);  
    camera->osd_switch = atoi(value);
    
    ak_ini_get_item_value(config.handle, "camera", "osd_name", value);  
    strcpy(camera->osd_name, value);
    
    /* TODO */
#if 0    
    camera->osd_unicode_name_len = font_utf8_to_unicode(
    	(char *)camera->osd_name, 
    	(char *)camera->osd_unicode_name);
    camera->osd_unicode_name_len >>= 1;
    font_lib_init(camera->osd_unicode_name, camera->osd_unicode_name_len);
#endif    

    ak_print_normal("%s: %d, %s\n", __func__,
    	camera->osd_unicode_name_len, camera->osd_name);
    	
    ak_ini_get_item_value(config.handle, "camera", "time_switch", value);  
    camera->time_switch = atoi(value);    
    ak_ini_get_item_value(config.handle, "camera", "date_format", value);  
    camera->date_format = atoi(value);
    ak_ini_get_item_value(config.handle, "camera", "hour_format", value);  
    camera->hour_format = atoi(value);
    ak_ini_get_item_value(config.handle, "camera", "week_format", value);  
    camera->week_format = atoi(value);    
	ak_ini_get_item_value(config.handle, "camera", "ircut_mode", value);  
    camera->ircut_mode = atoi(value);
}

void ak_config_set_camera_info(struct camera_disp_config *camera)
{
	ak_print_debug("osd_name len : %d\n", strlen(camera->osd_name));

	char value[CONFIG_VALUE_BUF_SIZE] = {0};

    sprintf(value, "%d", camera->width);    
    ak_ini_set_item_value(config.handle, "camera", "width", value);  
    sprintf(value, "%d", camera->height);
    ak_ini_set_item_value(config.handle, "camera", "height", value);
    sprintf(value, "%d", camera->osd_position);
    ak_ini_set_item_value(config.handle, "camera", "osd_position", value);
    sprintf(value, "%d", camera->osd_switch);
    ak_ini_set_item_value(config.handle, "camera", "osd_switch", value);
    sprintf(value, "%s", camera->osd_name);
    ak_ini_set_item_value(config.handle, "camera", "osd_name", value);  
    sprintf(value, "%d", camera->time_switch);
    ak_ini_set_item_value(config.handle, "camera", "time_switch", value);  
    sprintf(value, "%d", camera->date_format);
    ak_ini_set_item_value(config.handle, "camera", "date_format", value);  
    sprintf(value, "%d", camera->hour_format);
    ak_ini_set_item_value(config.handle, "camera", "hour_format", value);  
    sprintf(value, "%d", camera->week_format);
    ak_ini_set_item_value(config.handle, "camera", "week_format", value);  
	sprintf(value, "%d", camera->ircut_mode);
    ak_ini_set_item_value(config.handle, "camera", "ircut_mode", value);
}

struct camera_disp_config* ak_config_get_camera_info(void)
{
    return config.camera;
}

void ak_config_set_record_plan_by_net(struct record_plan_config *plan)
{
    int index = 0;
    int record_time;

    if(plan->active){
        for(index = 0; index < MAX_RECORD_PLAN_NUM; index ++){
            if(config.record_plan->plan[index].active == 0){
                break;
            }
        }
        if(index != MAX_RECORD_PLAN_NUM){
            record_time = config.record_plan->plan[index].record_time;
            memcpy(&config.record_plan->plan[index], plan, 
            	sizeof(struct record_plan_config));
            config.record_plan->plan[index].record_time = record_time;
        }
    }else{
        for(index = 0; index < MAX_RECORD_PLAN_NUM; index ++){
            if((config.record_plan->plan[index].start_time == plan->start_time) 
            	&& (config.record_plan->plan[index].end_time == plan->end_time)){
            	int i = 0;
            	for(i=0; i<DAY_PER_WEEK; ++i){
            		if(config.record_plan->plan[index].week_enalbe[i] 
            			!= plan->week_enalbe[i]){
            			break;
            		}	
            	}
            	if(i >= DAY_PER_WEEK){
            		config.record_plan->plan[index].active = 0;
                	break;
            	}
            }
        }
    }
    
    ak_config_set_record_plan(config.record_plan);
}

void ak_config_set_ptz(void *con, int para1)
{
	struct ptz_pos * ptz_contrl = (struct ptz_pos *)con;
	char *title = "ptz";
	char name[50];
	char value[CONFIG_VALUE_BUF_SIZE] = {0};
	
    sprintf(name, "p%dL", para1);
    sprintf(value, "%d", ptz_contrl[para1-1].left);
    ak_ini_set_item_value(config.handle, title, name, value);
	sprintf(name, "p%dU", para1);
    sprintf(value, "%d", ptz_contrl[para1-1].up);
    ak_ini_set_item_value(config.handle, title, name, value);   
}

void ak_config_get_ptz(void *con, int para1)
{
	struct ptz_pos * ptz_contrl = (struct ptz_pos *)con;
	
	char *title = "ptz";
	char name[50];
	char value[CONFIG_VALUE_BUF_SIZE] = {0};
	
    sprintf(name, "p%dL", para1);
    ak_ini_get_item_value(config.handle, title, name, value);
	ptz_contrl[para1-1].left = atoi(value);
    sprintf(name, "p%dU", para1);
    ak_ini_get_item_value(config.handle, title, name, value);
    ptz_contrl[para1-1].up = atoi(value);
}

void ak_config_set_ptz_unhit(void *con)
{
    struct ptz_pos * ptz_contrl = (struct ptz_pos *)con;
	char *title = "ptz";
    char value[CONFIG_VALUE_BUF_SIZE] = {0};

	if (access(CONFIG_PTZ_UNHIT_NAME, F_OK) != 0) {
		sprintf(value, "echo \"[%s]\" > %s", title, CONFIG_PTZ_UNHIT_NAME);
		system(value);
	}
	
    sprintf(value, "%d", ptz_contrl[0].left);
    ak_ini_set_item_value(config.handle, title, "hitL", value);
    sprintf(value, "%d", ptz_contrl[0].up);
    ak_ini_set_item_value(config.handle, title, "hitU", value); 
}

void ak_config_get_ptz_unhit(void *con)
{
	struct ptz_pos * ptz_contrl = (struct ptz_pos *)con;
	
	char *title = "ptz";
    char value[CONFIG_VALUE_BUF_SIZE] = {0};

	if (access(CONFIG_PTZ_UNHIT_NAME, F_OK) != 0) {
		sprintf(value, "echo \"[%s]\" > %s", title, CONFIG_PTZ_UNHIT_NAME);
		system(value);
	}
	
    ak_ini_get_item_value(config.handle, title, "hitL", value);
	ptz_contrl[0].left = atoi(value);
    ak_ini_get_item_value(config.handle, title, "hitU", value);
    ptz_contrl[0].up = atoi(value); 
}

static void init_system_user(struct sys_user_config *user)
{
	char value[CONFIG_VALUE_BUF_SIZE] = {0};

    ak_ini_get_item_value(config.handle, "global", "user", value);  
    strcpy(user->name, value);
    ak_ini_get_item_value(config.handle, "global", "secret", value);  
    strcpy(user->secret, value);
    ak_ini_get_item_value(config.handle, "global", "debuglog", value);  
    user->debuglog =  atoi(value);
    ak_ini_get_item_value(config.handle, "global", "rtsp_support", value);  
    user->rtsp_support =  atoi(value);
    ak_ini_get_item_value(config.handle, "global", "firmware_update", value);  
    user->firmware_update =  atoi(value);
    ak_ini_get_item_value(config.handle, "global", "dev_name", value);
	strcpy(user->dev_name, value);
	ak_ini_get_item_value(config.handle, "global", "soft_version", value);
	user->soft_version = atoi(value);
}

struct sys_user_config* ak_config_get_system_user(void)
{
	return config.user;
}

void ak_config_set_system_user(struct sys_user_config *user)
{
	char *title = "global";
    char value[100];
    
    sprintf(value, "%s", user->name);
	ak_ini_set_item_value(config.handle, title, "user", value);
    sprintf(value, "%s", user->secret);
	ak_ini_set_item_value(config.handle, title, "secret", value);
    sprintf(value, "%s", user->dev_name);
	ak_ini_set_item_value(config.handle, title, "dev_name", value);
    sprintf(value, "%d", user->firmware_update);
	ak_ini_set_item_value(config.handle, title, "firmware_update", value);
    sprintf(value, "%d", user->rtsp_support);
	ak_ini_set_item_value(config.handle, title, "rtsp_support", value);
    sprintf(value, "%u", user->soft_version);
	ak_ini_set_item_value(config.handle, title, "soft_version", value);
}

void ak_config_init_ini(void)
{
	config.handle = ak_ini_init("APPINI");
    if(NULL == config.handle) {
        ak_print_error_ex("open config file failed.\n");
		return;
    } else {
		ak_print_normal_ex("anyka config file check ok.\n\n");	
	}

    config.video = (struct video_config *)malloc(sizeof(struct video_config));
    config.record_plan = (struct video_record_config *)malloc(sizeof(struct video_record_config)); 
    config.alarm = (struct sys_alarm_config *)malloc(sizeof(struct sys_alarm_config));
    config.camera = (struct camera_disp_config *)malloc(sizeof(struct camera_disp_config));
	
	init_video_param(config.video);
	
#if CONFIG_DUMP_INFO
	ak_ini_dump_config(config.handle, "ethernet");
#endif

    init_record_plan(config.record_plan);
    init_sys_alarm(config.alarm);
    init_camera_config(config.camera);
    
}

void ak_config_release_ini(void)
{
	ak_ini_destroy(config.handle);
}
