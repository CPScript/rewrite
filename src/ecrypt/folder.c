#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <direct.h>
#define mkdir(name) _mkdir(name)
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

int main() {
    #ifdef _WIN32
        char* desktopPath = getenv("USERPROFILE");
        if (desktopPath == NULL) {
            fprintf(stderr, "Failed to get path.\n");
            return 1;
        }
        char folderPath[1024];
        snprintf(folderPath, sizeof(folderPath), "%s\\Desktop\\Photos", desktopPath);
    #endif

    if (mkdir(folderPath) == 0) {
        printf("Folder made\n");
    } else {
        perror("Failed to make folder");
        return 1;
    }

    return 0;
}
