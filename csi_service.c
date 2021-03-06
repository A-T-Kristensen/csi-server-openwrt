#include <stdio.h>
#include <stdlib.h>

#include "logger.h"
#include "core.h"

#define BUFSIZE 4096

// https://wands.sg/research/wifi/AtherosCSI/
// M x N x subcarriers   where M is transmitting antenna, N is receiving antenna
COMPLEX       csiMatrix[3][3][114];
csi_struct*   csiStatus;
unsigned char buffer[BUFSIZE];
unsigned char dataBuffer[1500];
int           csiDevice;// device file descriptor


int initCSI() {
    csiStatus = (csi_struct*) malloc(sizeof(csi_struct));
    
    csiDevice = open_csi_device();
    if(csiDevice < 0) {
        log(LEVEL_ERROR, "Could not open csi device, are you running as root? is /dev/CSI_dev present?");
        return 1;
    }
    log(LEVEL_DEBUG, "Opened csi device, fd: %d", csiDevice);



    return 0;
}

int closeCSI() {
    close_csi_device(csiDevice);
    free(csiStatus);
    log(LEVEL_DEBUG, "Closing csi device");
    return 0;
}

void processCSI(unsigned char *data_buf, csi_struct* csi_status, COMPLEX csi_matrix[3][3][114]) {

    onCSI(data_buf, csi_status, csi_matrix);

    // debugging code
    /* if(1 || csi_status->payload_len == 140) {
        printf("Recv msg with rate: 0x%02x | payload len: %d\n",csi_status->rate,csi_status->payload_len);
        printf("\n");
        printf("RSSI: %d\n", csi_status->rssi);
        printf("Subcarriers: %d\n", csi_status->num_tones);
        printf("RX antenna: %d\n", csi_status->nr);
        printf("TX antenna: %d\n", csi_status->nc);
        
        //COMPLEX* mat = csi_matrix[csi_status->nc][csi_status->nr];

        if(csi_status->csi_len == 0) {
            printf("No CSI in message (probably valid but no hw_upload/type)\n\n\n");
            return;
        }

        for(int tx = 0;tx < 3;tx++) {
            for(int rx = 0;rx < 3;rx++) {
                printf("\nTX: %d, RX: %d\n", tx, rx);
                for(int i = 0;i < csi_status->num_tones;i++) {
                   printf("R%dI%d ", csi_matrix[tx][rx][i].real, csi_matrix[csi_status->nc][csi_status->nr][i].imag);
                }
            }
        }
        printf("\n\n");

        printf("Above: msg with rate: 0x%02x | payload len: %d\n",csi_status->rate,csi_status->payload_len);

        //printf("\n");
        //for(int i = 0;i < csi_status->buf_len;i++) {
        //    printf("0x%02x\t", data_buf[i]);
        //}
        //printf("\n\n");
    } */

    
}

/**
 * Attempts to read csi from the csi device
 * @returns 1 if csi was read and processed, 0 otherwise
 */
int readCSI() {
    int length = read_csi_buf(buffer, csiDevice, BUFSIZE);
    if(length > 0) {
        log(LEVEL_TRACE, "Received %d bytes (a message) from csi device", length);

        record_status(buffer, length, csiStatus);
        record_csi_payload(buffer, csiStatus, dataBuffer, csiMatrix);

        processCSI(dataBuffer, csiStatus, csiMatrix);
        return 1;
    } else {
        return 0;
    }
}
