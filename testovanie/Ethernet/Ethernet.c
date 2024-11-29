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

wiz_NetInfo net_info = {
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
static uint8_t g_http_socket_num_list[HTTP_SOCKET_MAX_NUM] = {0, 1, 2, 3};

/* Functions */
/* Clock */
static void set_clock_khz(void);
char* load_file(const char* filename);





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

void network_init(){
    generate_mac(&net_info);

    wizchip_setnetinfo(&net_info);

    DHCP_init(0, &net_info.mac[0]);
    uint8_t dhcp_status = DHCP_run();
    while (dhcp_status != DHCP_IP_LEASED){
        sleep_ms(1000);
        dhcp_status = DHCP_run();
    }

    wizchip_getnetinfo(&net_info);
    printf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\n", net_info.mac[0], net_info.mac[1], net_info.mac[2], net_info.mac[3], net_info.mac[4], net_info.mac[5]);
    printf("IP Address: %d.%d.%d.%d\n", net_info.ip[0], net_info.ip[1], net_info.ip[2], net_info.ip[3]);
    printf("Subnet Mask: %d.%d.%d.%d\n", net_info.sn[0], net_info.sn[1], net_info.sn[2], net_info.sn[3]);
    printf("Gateway: %d.%d.%d.%d\n", net_info.gw[0], net_info.gw[1], net_info.gw[2], net_info.gw[3]);
    printf("DNS: %d.%d.%d.%d\n", net_info.dns[0], net_info.dns[1], net_info.dns[2], net_info.dns[3]);
}


int main()
{

    sleep_ms(3000);
    set_clock_khz();
    stdio_init_all();

    printf("Initializing Network\n");
    network_init();
    printf("Network Initialized\n");

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

    network_initialize(net_info);
    printf("Network Wiznet Initialized\n");

    httpServer_init(g_http_send_buf, g_http_recv_buf, HTTP_SOCKET_MAX_NUM, g_http_socket_num_list);

    /* Get network information */
    print_network_information(net_info);

    /* Register web page */
    reg_httpServer_webContent((uint8_t *)"index.html", (uint8_t*) index_page);
    reg_httpServer_webContent((uint8_t *)"zone_setup.htlm", (uint8_t*) zone_setup_page);
    reg_httpServer_webContent((uint8_t *)"config_setup.html", (uint8_t*) config_setup_page);

    /* Infinite loop */
    printf("Starting HTTP Server\n");
    while (1)
    {
        /* Run HTTP server */
        for (i = 0; i < HTTP_SOCKET_MAX_NUM; i++)
        {
            
            httpServer_run(i);
        }
    }

    // free(index_page);
    // free(zone_setup_page);
    // free(config_setup_page);
}





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