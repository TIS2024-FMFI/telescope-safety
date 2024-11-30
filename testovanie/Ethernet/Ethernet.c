#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "wizchip_conf.h"
#include "socket.h"
#include "w5500.h"
#include "dhcp.h"
#include "httpServer.h"
#include "hardware/clocks.h"
#include "port_common.h"
#include "w5x00_spi.h"
#include "www/websites.h"
#include "timer.h"

/* Clock */
#define PLL_SYS_KHZ (133 * 1000)

/* Buffer */
#define ETHERNET_BUF_MAX_SIZE (1024 * 2)

/* Socket */
#define HTTP_SOCKET_MAX_NUM 4

// Network configuration
#define LOCAL_IP {10, 10, 10, 0}
#define SUBNET_MASK {255, 255, 255, 0}
#define GATEWAY {10, 10, 10, 1}
#define SERVER_IP {10, 10, 10, 0}
#define SERVER_PORT 9005

static wiz_NetInfo g_net_info = {
    .mac = {0x02, 0x01, 0x00, 0x11, 0x11, 0x11},
    .ip = {0, 0, 0, 0},
    .sn = {0, 0, 0, 0},
    .gw = {0, 0, 0, 0},
    .dns = {0, 0, 0, 0},
    .dhcp = NETINFO_DHCP
};




/* HTTP */
static uint8_t g_http_send_buf[ETHERNET_BUF_MAX_SIZE] = {
    0,
};
static uint8_t g_http_recv_buf[ETHERNET_BUF_MAX_SIZE] = {
    0,
};

/* Socket */
#define SOCKET_DHCP 0
#define SOCKET_DNS 1
static uint8_t g_http_socket_num_list[HTTP_SOCKET_MAX_NUM] = {2, 3, 4, 5};

/* Retry count */
#define DHCP_RETRY_COUNT 5
#define DNS_RETRY_COUNT 5

static uint8_t g_ethernet_buf[ETHERNET_BUF_MAX_SIZE] = {
    0,
}; // common buffer

/* DHCP */
static uint8_t g_dhcp_get_ip_flag = 0;

/* Timer */
static volatile uint16_t g_msec_cnt = 0;

/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
/* Clock */
static void set_clock_khz(void);

/* DHCP */
static void wizchip_dhcp_init(void);
static void wizchip_dhcp_assign(void);
static void wizchip_dhcp_conflict(void);

/* Timer */
static void repeating_timer_callback(void);

char* load_file(const char* filename);






int generate_mac(wiz_NetInfo *net_info);




int main()
{

    /* Initialize */
    uint8_t retval = 0;
    uint8_t dhcp_retry = 0;
    uint8_t dns_retry = 0;

    sleep_ms(3000);
    set_clock_khz();
    stdio_init_all();
    sleep_ms(3000);

    


    for (int i = 0; i < 6; i++){
        printf("Waiting for You: %d\n", i);
        sleep_ms(1000);
    }
    // socket(0, Sn_MR_TCP, 3000, 0);

    uint8_t i = 0;
    // char* index_page = load_file("www/index.html");
    // char* zone_setup_page = load_file("www/zone_setup.html");
    // char* config_setup_page = load_file("www/config_setup.html");


    printf("Initializing Wizchip\n");
    wizchip_spi_initialize();
    wizchip_cris_initialize();

    wizchip_reset();
    wizchip_initialize();
    wizchip_check();


    printf("Network Initialized\n");
    network_initialize(g_net_info);
    printf("Network Wiznet Initialized\n");
    

    httpServer_init(g_http_send_buf, g_http_recv_buf, HTTP_SOCKET_MAX_NUM, g_http_socket_num_list);

    /* Get network information */
    print_network_information(g_net_info);

    /* Register web page */
    reg_httpServer_webContent((uint8_t *)"index.html", (uint8_t*) index_page);
    reg_httpServer_webContent((uint8_t *)"zone_setup.html", (uint8_t*) zone_setup_page);
    reg_httpServer_webContent((uint8_t *)"config_setup.html", (uint8_t*) config_setup_page);

    
    wizchip_1ms_timer_initialize(repeating_timer_callback);

    if (g_net_info.dhcp == NETINFO_DHCP) // DHCP
    {
        wizchip_dhcp_init();
    }
    else // static
    {
        network_initialize(g_net_info);

        /* Get network information */
        print_network_information(g_net_info);
    }


    /* Infinite loop */
    while (1)
    {
        /* Assigned IP through DHCP */
        if (g_net_info.dhcp == NETINFO_DHCP)
        {
            retval = DHCP_run();

            if (retval == DHCP_IP_LEASED)
            {
                if (g_dhcp_get_ip_flag == 0)
                {
                    printf(" DHCP success\n");

                    g_dhcp_get_ip_flag = 1;
                }
            }
            else if (retval == DHCP_FAILED)
            {
                g_dhcp_get_ip_flag = 0;
                dhcp_retry++;

                if (dhcp_retry <= DHCP_RETRY_COUNT)
                {
                    printf(" DHCP timeout occurred and retry %d\n", dhcp_retry);
                }
            }

            if (dhcp_retry > DHCP_RETRY_COUNT)
            {
                printf(" DHCP failed\n");

                DHCP_stop();

                while (1)
                    ;
            }

            wizchip_delay_ms(1000); // wait for 1 second
        }


        /* Run HTTP server */
        for (i = 0; i < HTTP_SOCKET_MAX_NUM; i++){
            httpServer_run(i);

        }

    }
   

    // free(index_page);
    // free(zone_setup_page);
    // free(config_setup_page);
}





/**
 * ----------------------------------------------------------------------------------------------------
 * Functions
 * ----------------------------------------------------------------------------------------------------
 */
/* Clock */
static void set_clock_khz(void)
{
    // set a system clock frequency in khz
    set_sys_clock_khz(PLL_SYS_KHZ, true);

    // configure the specified clock
    clock_configure(
        clk_peri,
        0,                                                // No glitchless mux
        CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS, // System PLL on AUX mux
        PLL_SYS_KHZ * 1000,                               // Input frequency
        PLL_SYS_KHZ * 1000                                // Output (must be same as no divider)
    );
}

/* DHCP */
static void wizchip_dhcp_init(void)
{
    printf(" DHCP client running\n");

    DHCP_init(SOCKET_DHCP, g_ethernet_buf);

    reg_dhcp_cbfunc(wizchip_dhcp_assign, wizchip_dhcp_assign, wizchip_dhcp_conflict);
}

static void wizchip_dhcp_assign(void)
{
    getIPfromDHCP(g_net_info.ip);
    getGWfromDHCP(g_net_info.gw);
    getSNfromDHCP(g_net_info.sn);
    getDNSfromDHCP(g_net_info.dns);

    g_net_info.dhcp = NETINFO_DHCP;

    /* Network initialize */
    network_initialize(g_net_info); // apply from DHCP

    print_network_information(g_net_info);
    printf(" DHCP leased time : %ld seconds\n", getDHCPLeasetime());
}

static void wizchip_dhcp_conflict(void)
{
    printf(" Conflict IP from DHCP\n");

    // halt or reset or any...
    while (1)
        ; // this example is halt.
}

/* Timer */
static void repeating_timer_callback(void)
{
    g_msec_cnt++;

    if (g_msec_cnt >= 1000 - 1)
    {
        g_msec_cnt = 0;

        DHCP_time_handler();
    }
}


// Function to load a file into a buffer
char* load_file(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Failed to open file: %s\n", filename);
        return NULL;
    }

    // Get the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate buffer for the file contents
    char* buffer = (char*)malloc(file_size + 1);
    if (!buffer) {
        printf("Failed to allocate memory\n");
        fclose(file);
        return NULL;
    }

    // Read the file into the buffer
    fread(buffer, 1, file_size, file);
    buffer[file_size] = '\0'; // Null-terminate the buffer

    // Close the file
    fclose(file);

    return buffer;
}

int generate_mac(wiz_NetInfo *net_info){
    // least significant bit of the first byte of the MAC address must be 0
    // indicates unicast address
    // second least significant bit of the first byte of the MAC address must be 1
    // indicates locally administered address
    net_info->mac[0] = 0x02;
    // the rest of the MAC address can be any value
    net_info->mac[1] = 0x01;

    // second half of the RP2040 ID (Serial Number)
    // Has to be checked if it is unique and if its ID
    net_info->mac[2] = (uint8_t) 0x10000064;
    net_info->mac[3] = (uint8_t) 0x10000065;
    net_info->mac[4] = (uint8_t) 0x10000066;
    net_info->mac[5] = (uint8_t) 0x10000067;
    return 0;

}

