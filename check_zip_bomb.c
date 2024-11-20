#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "miniz.h"

#define MAX_TOTAL_UNCOMPRESSED_SIZE (100  1024  1024)
#define MAX_FILE_COUNT 1000

int isZipBomb(const char *zipFilePath) {
    mz_zip_archive zipArchive;
    memset(&zipArchive, 0, sizeof(zipArchive));

    if (!mz_zip_reader_init_file(&zipArchive, zipFilePath, 0)) {
        fprintf(stderr, "Ошибка при открытии zip-файла: %s\n", zipFilePath);
        return -1;
    }

    int fileCount = mz_zip_reader_get_num_files(&zipArchive);
    if (fileCount > MAX_FILE_COUNT) {
        printf("Обнаружена потенциальная ZIP-бомба: слишком много файлов (%d)\n", fileCount);
        mz_zip_reader_end(&zipArchive);
        return 1;
    }

    size_t totalUncompressedSize = 0;

    for (int i = 0; i < fileCount; i++) {
        mz_zip_archive_file_stat fileStat;
        if (mz_zip_reader_file_stat(&zipArchive, i, &fileStat)) {
            totalUncompressedSize += fileStat.m_uncomp_size;

            if (totalUncompressedSize > MAX_TOTAL_UNCOMPRESSED_SIZE) {
                printf("Обнаружена потенциальная ZIP-бомба: общий размер распакованных данных превышает лимит (%zu байт)\n", totalUncompressedSize);
                mz_zip_reader_end(&zipArchive);
                return 1;
            }
        }
    }

    mz_zip_reader_end(&zipArchive);
    printf("ZIP-бомба не обнаружена в файле: %s\n", zipFilePath);
    return 0;
}

int main() {
    char zipFilePath[256];

    printf("Введите путь к ZIP-файлу: ");
    scanf("%255s", zipFilePath);

    isZipBomb(zipFilePath);
    return 0;
}
