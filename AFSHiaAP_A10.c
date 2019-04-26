#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <pwd.h>
#include <grp.h>

static const char *dirpath = "/home/gede/shift4";

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	res = lstat(fpath, stbuf);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
  char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		res = (filler(buf, de->d_name, &st, 0));
			if(res!=0) break;
        
        char newpath[1000];
        sprintf(newpath, "%s/%s", fpath, de->d_name);
        printf("-----%s------\n", newpath);

        struct stat sb;
		
		stat(newpath, &sb);

		struct passwd *pw = getpwuid(sb.st_uid);
		struct group  *gr = getgrgid(sb.st_gid);
        struct tm *jam = localtime(&sb.st_atime);

		int usr1=strcmp(pw->pw_name, "chipset");
        int usr2=strcmp(pw->pw_name, "ic_controller");
		int grp=strcmp(gr->gr_name, "rusak");
        char hasil[1000];

		if(((usr1==0 || usr2==0) && grp==0) || access(newpath, R_OK) != 0)
		{
            if(S_ISREG(sb.st_mode))
            {
                FILE *miris;
                char filenya[1000];
                sprintf(filenya, "%s/filemiris.txt", fpath);
                sprintf(hasil, "\nNama:%s, Owner:%d, Group:%d, Atime:%04d-%02d-%02d_%02d:%02d:%02d\n",de->d_name, sb.st_uid, sb.st_gid, jam->tm_year+1900, jam->tm_mon+1, jam->tm_mday, jam->tm_hour, jam->tm_min, jam->tm_sec);
                miris=fopen(filenya, "a");
                fputs(hasil, miris);
                fclose(miris);
                remove(newpath);    
            }
        }
	}

	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
  char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;
  int fd = 0 ;

	(void) fi;
	fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
    char fpath[1000];

	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	// sprintf(fpath, "%s.iz1", fpath);
	int res;
	/* On Linux this could just be 'mknod(path, mode, rdev)' but this
	   is more portable */
	char *p;
	p = strstr(fpath, "YOUTUBER");
	if(p)
	{
    	mode=33184;
	}
	if (S_ISREG(mode)) {
		res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(fpath, mode);
	else
		res = mknod(fpath, mode, rdev);
	if (res == -1)
		return -errno;
    
    // char newpath[1000];
    // sprintf(newpath, "%s.iz1", fpath);
    
	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
    char fpath[1000];

	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);

	char *p;
	p = strstr(fpath, "YOUTUBER");

	if(p)
	{
    	mode=488;
	}

    printf("---%d----\n", mode);
	int res;

	res = mkdir(fpath, mode);
	if (res == -1)
		return -errno;
    printf("---%d----\n", mode);
    
	return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
    char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);

	int res;

	res = truncate(fpath, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
    char fpath[1000];
	
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);

	int res;
	struct timeval tv[2];

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(fpath, tv);
	if (res == -1)
		return -errno;

	
	return 0;
}

static int xmp_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000];

    if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);

	int fd;
	int res;
	char newpath[1000];
	(void) fi;

	fd = open(fpath, O_WRONLY);
	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);

	char *p;
	p = strstr(fpath, "YOUTUBER");

	if(p)
	{
		pid_t pp;
		pp=fork();
		if(pp==0)
		{
			sprintf(newpath, "%s.iz1", fpath);
			char *emve[]={"mv", fpath, newpath, NULL};
			execv("/bin/mv", emve);
		}
	}
	return res;
}

static int xmp_chmod(const char *path, mode_t mode)
{
	char fpath[1000];

    if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	
	char *ext;
	ext=strrchr(fpath, '.');
	
	char *p;
	p = strstr(fpath, "/YOUTUBER/");

	if(p)
	{
		if(strcmp(ext, ".iz1")==0)
		{
			pid_t child;
			child = fork();

			if(child==0)
			{
				char *argv[4] = {"zenity", "--warning", "--text='File ekstensi iz1 tidak boleh diubah permissionnya.'", NULL};
				execv("/usr/bin/zenity", argv);
			}
			return 0;
		}
	}
	int res;

	res = chmod(fpath, mode);

	if (res == -1)
		return -errno;

	return 0;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.mknod		= xmp_mknod,
	.mkdir		= xmp_mkdir,
	.truncate	= xmp_truncate,
	.utimens	= xmp_utimens,
	.read		= xmp_read,
	.write		= xmp_write,
	.chmod		= xmp_chmod
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
