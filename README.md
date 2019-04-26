# Soal Shift Modul 4 E07
Baru sukses nomor 1
## Nomor 1
Pertama, sandinya adalah [_Caesar cipher_](https://en.wikipedia.org/wiki/Caesar_cipher), 
dengan daftar karakter yang dimodifikasi.
Berikut daftar karakternya.
```
qE1~ YMUR2"`hNIdPzi%^t@(Ao:=CQ,nx4S[7mHFye#aT6+v)DfKL$r?bkOGB>}!9_wV']jcp5JZ&Xl|\8s;g<{3.u*W-0
```
Untuk enkripsi-dekripsi pun mirip, hanya perlu ditambah (enkripsi) atau dikurang (dekripsi)
### Implementasi _Caesar cipher_
```C
#define ENCRYPT 0
#define DECRYPT 1
#define KEY 17

void caesar(char inp[], int len, int flag)
{
    int i, j, index;
    if (flag == DECRYPT)
    {
        for (i = 0; i < len; i++)
        {
            if (inp[i] == '/')
                continue;
            else
            {
                for (j = 0; j < strlen(charlist); j++)
                {
                    if (charlist[j] == inp[i])
                        index = j;
                }
                index -= KEY;
                while (index < 0)
                    index += strlen(charlist);
                index %= strlen(charlist);
                inp[i] = charlist[index];
            }
        }
    }
    else if (flag == ENCRYPT)
    {
        for (i = 0; i < len; i++)
        {
            if (inp[i] == '/')
                continue;
            else
            {
                for (j = 0; j < strlen(charlist); j++)
                {
                    if (charlist[j] == inp[i])
                        index = j;
                }
                index += KEY;
                index %= strlen(charlist);
                inp[i] = charlist[index];
            }
        }
    }
}
```
### Enkripsi-dekripsi di _filesystem_
Enkripsi-dekripsi bisa dilakukan dengan mengimplementasikan 3 fungsi, `getattr()`, `readdir()`, dan `read()`.  
Pertama-tama definisikan `struct fuse_operations` terlebih dahulu.
```C
static struct fuse_operations xmp_oper =
{
    .getattr = xmp_getattr,
    .readdir = xmp_readdir,
    .read = xmp_read
};
```
Kemudian definisikan fungsinya.
#### 1. `xmp_getattr()`
```C
static int xmp_getattr(const char *path, struct stat *stbuf)
{
    int res;
    char name[MAX_PATH], fname[MAX_PATH];
    memset(fname, '\0', MAX_PATH);
    strcpy(name, path);
    caesar(name, strlen(name), ENCRYPT);
    strcat(fname, dirpath);
    strcat(fname, name);
    res = lstat(fname, stbuf);
    if (res == -1)
        return -errno;
    return 0;
}
```
#### 2. `xmp_readdir()`
```C
static int xmp_readdir(const char *path,
                       void *buf,
                       fuse_fill_dir_t filler,
                       off_t offset,
                       struct fuse_file_info *fi)
{
    char name[MAX_PATH], fname[MAX_PATH], dec[MAX_PATH];
    memset(fname, '\0', MAX_PATH);
    if(strcmp(path,"/") == 0)
    {
        path = dirpath;
        sprintf(fname, "%s", path);
    }
    else
    {
        strcpy(name, path);
        caesar(name, strlen(name), ENCRYPT);
        memset(fname, '\0', MAX_PATH);
        strcat(fname, dirpath);
        strcat(fname, name);
    }
    int res = 0;

    DIR *dp;
    struct dirent *de;

    (void) offset;
    (void) fi;

    dp = opendir(fname);
    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL)
    {
        if(strcmp(de->d_name,".")==0||strcmp(de->d_name,"..")==0)
            continue;
        struct stat st;
        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;
        memset(dec, '\0', MAX_PATH);
        strcpy(dec, de->d_name);
        caesar(dec, strlen(dec), DECRYPT);
        res = (filler(buf, dec, &st, 0));
        if(res != 0)
            break;
    }

    closedir(dp);
    return 0;
}
```
#### 3. `xmp_read()`
```C
static int xmp_read(const char *path,
                    char *buf,
                    size_t size,
                    off_t offset,
                    struct fuse_file_info *fi)
{
    char name[MAX_PATH], fname[MAX_PATH];
    memset(fname, '\0', MAX_PATH);
    if(strcmp(path,"/") == 0)
    {
        path = dirpath;
        sprintf(fname, "%s", path);
    }
    else
    {
        strcpy(name, path);
        caesar(name, strlen(name), ENCRYPT);
        memset(fname, '\0', MAX_PATH);
        strcat(fname, dirpath);
        strcat(fname, name);
    }
    int res = 0;
    int fd = 0 ;

    (void) fi;
    fd = open(fname, O_RDONLY);
    if (fd == -1)
        return -errno;

    res = pread(fd, buf, size, offset);
    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}
```
### Penggunaan
1. _Compile_
```bash
gcc `pkg-config fuse --cflags` AFSHiaAP_E07.c -o AFSHiaAP_E07 `pkg-config fuse --libs`
```
2. Jalankan
```bash
./AFSHiaAP_E07 "/home/$USER/shift4"
```
3. ????

4. PROFIT(?)
# -FIN-
