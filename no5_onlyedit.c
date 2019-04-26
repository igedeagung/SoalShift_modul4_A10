#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <string.h>
#include <sys/types.h>

static const char *dirpath = "/home/gede/Downloads";
char lama[1000];
char baru[1000];
FILE *fd1;
FILE *fd2;
FILE *isi;

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
	}

	closedir(dp);
	return 0;
}

char name1[100];
char name2[100];

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
	

	fd1 = fopen (fpath, "r");
	fgets(lama, 1000, fd1);	

	int res = 0;
 	int fd = 0 ;

	(void) fi;



	fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	fclose(fd1);
	close(fd);
	// printf("%s--%s--%s--%d\n", fpath, path, dalem, n);
	return res;
}

static int xmp_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	char fpath[1000];
    char path2[1000];
	char *ext;
	char waktu[100];
	char newdir[100];
	char dalem[1000];
	int n=0;
    time_t rawtime;
    struct tm * timeinfo;

	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	
	sprintf(dalem, "%s", fpath);
	for(int h=0; h<strlen(dalem); h++)
   	{
		if(dalem[h]=='/')
		{
			n++;
		}
   	}
	for(int h=0; h<strlen(dalem); h++)
   	{
		if(n==0)
		{
			dalem[h]='\0';
			break;
		}
		else
		{
			if(dalem[h]=='/')
			{
				n--;
			}
		}
   	}
	int fd;
	int res;

	(void) fi;
	
	fd = open(fpath, O_WRONLY);
	
	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;


	fd2 = fopen (fpath, "r");

	fgets(baru, 1000, fd2);
	
	if(strcmp(lama,baru)!=0){
		DIR *drc;
        
		sprintf(newdir, "%sBackup/", dalem);
        
        drc = opendir(newdir);
        if (drc==NULL) {
        char *mekadir[]={"mkdir", newdir, NULL};
        execv("/bin/mkdir", mekadir);
        closedir(drc);
        }

        time ( &rawtime );
		timeinfo = localtime ( &rawtime );
		sprintf(waktu, "%04d-%02d-%02d_%02d:%02d:%02d", timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
		ext = strchr(fpath,'.');
		sprintf(path2, "%s", path);


		for(int e; e<strlen(path2); e++)
		{
			if(path2[e]=='.')
			{
				path2[e]='\0';
			}
		}

		char *path3;
		
		path3=strrchr(path2, '/');
		printf("--%s--%s--\n", path3, path2);
	    sprintf(name1,"%s",fpath);
        sprintf(name2, "%s%s_%s%s", newdir, path3, waktu, ext);
		
		isi = fopen(name2,"w+");
		fprintf(isi, "%s", baru);
		fclose(isi);
	}

	fclose(fd2);
	close(fd);
	
	return res;
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

// static int xmp_unlink(const char *path)
// {
// 	char fpath[1000];
// 	char dalem[1000];
// 	char newdir[1000];
// 	char newdir2[1000];
// 	// char newfile[1000];
// 	int n=0;
// 	char *filenya;
// 	char *ext;
// 	char isi[1000];
// 	char filenyaBaru[1000];
// 	// char *ext;

// 	if(strcmp(path,"/") == 0)
// 	{
// 		path=dirpath;
// 		sprintf(fpath,"%s",path);
// 	}
// 	else sprintf(fpath, "%s%s",dirpath,path);

// 	sprintf(dalem, "%s", fpath);
// 	filenya = strrchr(fpath,'/');
// 	ext = strchr(filenya, '.');
// 	sprintf(filenyaBaru, "%s", filenya);
// 	for(int b=0; b<strlen(filenyaBaru)-1; b++)
// 	{
// 		filenyaBaru[b]=filenyaBaru[b+1];
// 	}
// 	char filefix[1000];
// 	sprintf(filefix, "%s", filenyaBaru);
// 	for(int a=0; a<strlen(filenyaBaru); a++)
// 	{
// 		if(filenyaBaru[a]=='.')
// 		{
// 			filenyaBaru[a]='\0';
// 			break;
// 		}
// 	}

// 	FILE *bakup;
// 	bakup=fopen(fpath, "r");
// 	fgets(isi, 1000, bakup);
// 	fclose(bakup);

// 	for(int h=0; h<strlen(dalem); h++)
//    	{
// 		if(dalem[h]=='/')
// 		{
// 			n++;
// 		}
//    	}
// 	for(int h=0; h<strlen(dalem); h++)
//    	{
// 		if(n==0)
// 		{
// 			dalem[h]='\0';
// 			break;
// 		}
// 		else
// 		{
// 			if(dalem[h]=='/')
// 			{
// 				n--;
// 			}
// 		}
//    	}
	

// 	printf("----%s----%s----%s----\n", filenyaBaru, ext, dalem);
// 	int res;

// 	res = unlink(fpath);
// 	if (res == -1)
// 		return -errno;

// 	DIR *drc;
        
// 	sprintf(newdir, "%sRecycleBin/", dalem);
        
//     drc = opendir(newdir);
//     if (drc==NULL) {
//         char *mekadir[]={"mkdir", newdir, NULL};
//         execv("/bin/mkdir", mekadir);
//         closedir(drc);
//     }

// 	DIR *drcc;
        
// 	sprintf(newdir2, "%ssementara", dalem);
    
//     drcc = opendir(newdir2);
//     if (drcc==NULL) {
//         char *mekadir2[]={"mkdir", newdir2, NULL};
//         execv("/bin/mkdir", mekadir2);
//         closedir(drcc);
//     }

// 	DIR *dp;
// 	struct dirent *de;
// 	char backkup[1000];
// 	sprintf(backkup, "%sBackup", dalem);
// 	dp = opendir(backkup);
// 	if (dp == NULL)
// 		return -errno;

// 	while ((de = readdir(dp)) != NULL) {
// 		struct stat st;
// 		memset(&st, 0, sizeof(st));
// 		char wew[1000];
// 		char lama[1000];
// 		char baruu[1000];

// 		sprintf(wew, "%s", de->d_name);
// 		sprintf(lama, "%s/%s",backkup, de->d_name);
// 		sprintf(baruu, "%s/%s",newdir2, de->d_name);
// 		wew[strlen(filenyaBaru)]='\0';
// 		if(strcmp(wew, filenyaBaru)==0)
// 		{
// 			printf("----%s------\n", de->d_name);
// 			char *emve[]={"mv", lama, baru, NULL};
//         	execv("/bin/mv", emve);

// 			char filenew[1000];
// 			sprintf(filenew. "%s/%s", newdir2, filefix);
// 			FILE *isikan

// 			isikan=fopen(filenew, "w+");
// 			fprintf(isikan, "%s", isi);
// 			fclose(isikan);

// 		}
// 		printf("--------%s----------%ld\n", wew, strlen(filenyaBaru));
		
// 	}
// 	closedir(dp);



// 	return 0;
// }

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
	.write 		= xmp_write,
	.truncate  	= xmp_truncate,
	.unlink		= xmp_unlink
};

int main(int argc, char *argv[])
{
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}