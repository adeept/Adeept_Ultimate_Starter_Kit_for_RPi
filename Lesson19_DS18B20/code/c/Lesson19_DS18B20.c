#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <lgpio.h>

#define BUFSIZE 128
#define DEVICE_ROOT "/sys/bus/w1/devices/"
#define SENSOR_PREFIX "28-"
#define TEMP_FILE "w1_slave"

int get_ds18b20_path(char *sensor_path)
{
    DIR *dir;
    struct dirent *dir_entry;

    dir = opendir(DEVICE_ROOT);
    if (dir == NULL) {
        perror("Failed to open directory " DEVICE_ROOT);
        return -2;
    }

    while ((dir_entry = readdir(dir)) != NULL) {
        if (strncmp(dir_entry->d_name, SENSOR_PREFIX, strlen(SENSOR_PREFIX)) == 0) {
            snprintf(sensor_path, BUFSIZE, "%s%s/%s", DEVICE_ROOT, dir_entry->d_name, TEMP_FILE);
            closedir(dir);
            return 0;
        }
    }

    closedir(dir);
    fprintf(stderr, "No DS18B20 sensor detected\n");
    return -1;
}

int main(void)
{
    float temp;
    int fd;
    int ret;
    int i, j;
    char buf[BUFSIZE];
    char tempBuf[16] = {0};
    char sensor_path[BUFSIZE];

    printf("DS18B20 Temperature Test\n");

    ret = get_ds18b20_path(sensor_path);
    if (ret != 0) {
        fprintf(stderr, "Failed to get DS18B20 path\n");
        return 1;
    }

    printf("Detected DS18B20 path: %s\n", sensor_path);

    while (1) {
        memset(buf, 0, sizeof(buf));
        memset(tempBuf, 0, sizeof(tempBuf));

        fd = open(sensor_path, O_RDONLY);
        if (fd == -1) {
            perror("Failed to open temperature data file");
            lguSleep(1);
            continue;
        }

        while (1) {
            ret = read(fd, buf, sizeof(buf) - 1);

            if (ret == 0)
                break;

            if (ret == -1) {
                if (errno == EINTR)
                    continue;

                perror("read()");
                close(fd);
                return 1;
            }

            buf[ret] = '\0';
            break;
        }

        close(fd);

        for (i = 0; i < ret; i++) {
            if (buf[i] == 't' && buf[i + 1] == '=') {
                for (j = 0; j < (int)sizeof(tempBuf) - 1 && i + 2 + j < ret; j++) {
                    if (buf[i + 2 + j] < '0' || buf[i + 2 + j] > '9')
                        break;
                    tempBuf[j] = buf[i + 2 + j];
                }
                break;
            }
        }

        if (tempBuf[0] != '\0') {
            temp = (float)atoi(tempBuf) / 1000.0;
            printf("Temperature: %.3f C\n", temp);
        } else {
            printf("Failed to read temperature\n");
        }

        lguSleep(0.5);
    }

    return 0;
}