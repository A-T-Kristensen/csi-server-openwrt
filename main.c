#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "logger.h"
#include "csi_service.h"
#include "server.h"

int exitRequested;

void sigHandler(int sig) {
    if (sig == SIGINT) {
        exitRequested = 1;
    }
}

int main(int argc, char* argv[]) {
    exitRequested = 0;
    signal(SIGINT, sigHandler);
    
    setLogLevel(LEVEL_TRACE);
    log(LEVEL_INFO, "Starting CSI server...");
    
    int status;
    status = initCSI();
    if(status) {
        log(LEVEL_ERROR, "Could not initialize CSI service");
        exit(status);
    }

    status = initServer();
    if(status) {
        log(LEVEL_ERROR, "Could not start server");
        exit(status);
    }

    int received;
    while(! exitRequested) {
        received = 0;
        received = readCSI() | received;
        received = readServer() | received;

        if(! received) {
            usleep(10);
        }
    }

    log(LEVEL_INFO, "Stopping CSI server...");

    closeServer();
    closeCSI();
}
