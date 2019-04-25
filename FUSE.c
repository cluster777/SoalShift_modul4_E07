#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#define MAX_PATH 260
#define CYPHER 0
#define DECYPHER 1

const char a[]= {"qE1~ YMUR2\"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV\']jcp5JZ&Xl|\\8s;g<{3.u*W-0"};
const char root[MAX_PATH] = "/home/arino/shift4";

int find(char f)
{
    for (int i = 0; i < strlen(a); i++)
    {
        if(f == a[i])
            return i;
    }
    return -1;
}
int caesar(char input[], int flag)
{
    //ini diganti sama nama folder
    //char input[]="INI_FOLDER/halo";
    //char input[]="n,nsbZ]wio/QBE#";
    //sampai sini hehe...

    //ini untuk tipe cypher atau decypher
    //int flag=DECYPHER;
    //sampai sini
    char hasil[100]="";
    char txp[2];
    int mod = strlen(a);
    for(int i=0; i < strlen(input); i++)
    {
        char hs;
        if(input[i] != '/')
        {
            int temp = find(input[i]);
            if(temp == -1)
            {
                //input tidak dikenal
                printf("error word not found\n");
                return -1;
                //error return -1
            }
            if(flag == CYPHER)
                temp = temp + 17;
            else
                temp = temp - 17 + strlen(a);
            temp = temp % mod;
            //printf("%c",a[temp]);
            hs = (char)a[temp];
        }
        else
        {
            //printf("\\");
            hs='/';
        }
        txp[0]=hs;
        sprintf(hasil,"%s%c",hasil,hs);
    }
    printf("%s\n",hasil);
    //printf("\n");
}

static int my_getattr(const char *path, struct stat *buf)
{
    char fullpath[MAX_PATH], anyar[MAX_PATH], lawas[MAX_PATH];
    strcpy(anyar, path);
    strcpy(lawas, path);
    caesar(anyar, DECYPHER);
    sprintf(anyar, "%s%s", root, anyar);
    sprintf(lawas, "%s%s", root, lawas);
    //rename(lawas, anyar);
    sprintf(fullpath, "%s%s", root, path);
    int res;
    if ((res = lstat(anyar, buf)) == -1)
        return -errno;
    return 0;
}

static int my_readdir(const char *path,
                      void *buf,
                      fuse_fill_dir_t filler,
                      off_t offset,
                      struct fuse_file_info *fi)
{
    DIR *dp;
    struct dirent *dir;
    (void)offset;
    (void)fi;

    char fullpath[MAX_PATH], anyar[MAX_PATH], lawas[MAX_PATH], fpath[MAX_PATH];
    //strcpy(anyar, path);
    //strcpy(lawas, path);
    //caesar(anyar, DECYPHER);
    //sprintf(anyar, "%s%s", root, anyar);
    //sprintf(lawas, "%s%s", root, lawas);
    //rename(lawas, anyar);
    sprintf(fullpath, "%s%s", root, path);

    if ((dp = opendir(lawas)) == NULL)
        return -errno;

    while ((dir = readdir(dp)) != NULL)
    {
        struct stat st;
        memset(&st, 0, sizeof st);
        st.st_ino = dir->d_ino;
        st.st_mode = dir->d_type << 12;
        strcpy(fpath, dir->d_name);
        strcpy(anyar, fpath);
        strcpy(lawas, fpath);
        caesar(anyar, DECYPHER);
        sprintf(anyar, "%s%s", root, anyar);
        sprintf(lawas, "%s%s", root, lawas);
        if (filler(buf, anyar, &st, 0))
            break;
    }
    closedir(dp);
    return 0;
}

static int my_read(const char *path,
                   char *buf,
                   size_t size,
                   off_t offset,
                   struct fuse_file_info *fi)
{
    int fd;
    int res;
    (void)fi;

    char fullpath[MAX_PATH], anyar[MAX_PATH], lawas[MAX_PATH];
    strcpy(anyar, path);
    strcpy(lawas, path);
    caesar(anyar, DECYPHER);
    sprintf(anyar, "%s%s", root, anyar);
    sprintf(lawas, "%s%s", root, lawas);
    rename(lawas, anyar);
    sprintf(fullpath, "%s%s", root, path);

    if ((fd = open(anyar, O_RDONLY)) == -1)
        return -errno;

    if ((res = pread(fd, buf, size, offset)) == -1)
        return -errno;

    close(fd);
    return res;
}

struct fuse_operations op =
{
    .getattr = my_getattr,
    .readdir = my_readdir,
    .read = my_read
};

int main(int argc, char *argv[])
{
    umask(0);
    argc = 2;
    argv[0] = "./FUSE";
    argv[1] = "/home/arino/shift4_mount";
    argv[2] = NULL;
    fuse_main(argc, argv, &op, NULL);
    return 0;
}
