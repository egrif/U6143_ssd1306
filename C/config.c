#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"

DisplayConfig g_config = {
    .show_temperature = 1,
    .show_memory      = 1,
    .show_disk        = 1,
    .show_clock       = 0,
    .show_uptime      = 0,
    .show_cpu_freq    = 0,
    .show_gpu_temp    = 0,
    .show_network     = 0,
    .show_wifi        = 0,
    .show_docker      = 0,
    .show_hostname    = 1,
    .show_ip          = 0,
    .temp_fahrenheit      = 1,
    .load_cores           = 0,
    .top_line             = TOP_LINE_IP,
    .custom_text          = "UCTRONICS",
    .i2c_bus              = "/dev/i2c-1",
    .network_interfaces   = "eth0,wlan0",
    .screen_time          = 3,
};

static void trim(char *s)
{
    char *end = s + strlen(s) - 1;
    while (end >= s && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n'))
        *end-- = '\0';
    char *start = s;
    while (*start == ' ' || *start == '\t') start++;
    if (start != s)
        memmove(s, start, strlen(start) + 1);
}

void load_config(void)
{
    FILE *f = NULL;
    const char *env = getenv("SSD1306_CONF");
    if (env)
        f = fopen(env, "r");
    if (!f)
        f = fopen("./ssd1306.conf", "r");
    if (!f)
        f = fopen("/etc/ssd1306.conf", "r");
    if (!f)
        return;

    char line[128];
    while (fgets(line, sizeof(line), f)) {
        char *eq = strchr(line, '=');
        if (!eq)
            continue;
        *eq = '\0';
        char *key = line;
        char *val = eq + 1;
        trim(key);
        trim(val);
        if (key[0] == '#' || key[0] == '\0')
            continue;

        if (strcmp(key, "show_temperature") == 0)
            g_config.show_temperature = atoi(val);
        else if (strcmp(key, "show_memory") == 0)
            g_config.show_memory = atoi(val);
        else if (strcmp(key, "show_disk") == 0)
            g_config.show_disk = atoi(val);
        else if (strcmp(key, "show_clock") == 0)
            g_config.show_clock = atoi(val);
        else if (strcmp(key, "show_uptime") == 0)
            g_config.show_uptime = atoi(val);
        else if (strcmp(key, "show_cpu_freq") == 0)
            g_config.show_cpu_freq = atoi(val);
        else if (strcmp(key, "show_gpu_temp") == 0)
            g_config.show_gpu_temp = atoi(val);
        else if (strcmp(key, "show_network") == 0)
            g_config.show_network = atoi(val);
        else if (strcmp(key, "show_wifi") == 0)
            g_config.show_wifi = atoi(val);
        else if (strcmp(key, "show_docker") == 0)
            g_config.show_docker = atoi(val);
        else if (strcmp(key, "show_hostname") == 0)
            g_config.show_hostname = atoi(val);
        else if (strcmp(key, "show_ip") == 0)
            g_config.show_ip = atoi(val);
        else if (strcmp(key, "temp_unit") == 0)
            g_config.temp_fahrenheit = (strcmp(val, "fahrenheit") == 0);
        else if (strcmp(key, "load_display") == 0)
            g_config.load_cores = (strcmp(val, "cores") == 0);
        else if (strcmp(key, "network_interfaces") == 0)
            strncpy(g_config.network_interfaces, val, sizeof(g_config.network_interfaces) - 1);
        else if (strcmp(key, "ip_source") == 0) {
            /* backwards-compat alias: put the named interface first */
            if (strcmp(val, "wlan0") == 0)
                strncpy(g_config.network_interfaces, "wlan0,eth0", sizeof(g_config.network_interfaces) - 1);
            else
                strncpy(g_config.network_interfaces, "eth0,wlan0", sizeof(g_config.network_interfaces) - 1);
        }
        else if (strcmp(key, "screen_time") == 0)
            g_config.screen_time = atoi(val);
        else if (strcmp(key, "top_line") == 0) {
            if (strcmp(val, "hostname") == 0)
                g_config.top_line = TOP_LINE_HOSTNAME;
            else if (strcmp(val, "custom") == 0)
                g_config.top_line = TOP_LINE_CUSTOM;
            else if (strcmp(val, "none") == 0)
                g_config.top_line = TOP_LINE_NONE;
            else
                g_config.top_line = TOP_LINE_IP;
        }
        else if (strcmp(key, "custom_text") == 0)
            strncpy(g_config.custom_text, val, sizeof(g_config.custom_text) - 1);
        else if (strcmp(key, "i2c_bus") == 0)
            strncpy(g_config.i2c_bus, val, sizeof(g_config.i2c_bus) - 1);
    }
    fclose(f);
}
