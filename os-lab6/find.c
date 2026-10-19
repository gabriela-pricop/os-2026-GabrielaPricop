#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fs.h"
#include "user/user.h"

char*
fmtname(char *path)
{
  static char buf[DIRSIZ+1];
  char *p;

  for(p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;

  if(strlen(p) >= DIRSIZ)
    return p;

  memmove(buf, p, strlen(p));
  memset(buf + strlen(p), 0, DIRSIZ + 1 - strlen(p));

  return buf;
}

void
find(char *path, char *name)
{
  int fd;
  struct stat st;
  struct dirent de;
  char buf[512];
  char *p;

  if(stat(path, &st) < 0)
    return;

  if(strcmp(fmtname(path), name) == 0)
    printf("%s\n", path);

  if(st.type != T_DIR)
    return;

  fd = open(path, 0);
  if(fd < 0)
    return;

  strcpy(buf, path);
  p = buf + strlen(buf);
  *p++ = '/';

  while(read(fd, &de, sizeof(de)) == sizeof(de)){

    if(de.inum == 0)
      continue;

    if(strcmp(de.name, ".") == 0 ||
       strcmp(de.name, "..") == 0)
      continue;

    memmove(p, de.name, DIRSIZ);
    p[DIRSIZ] = 0;

    find(buf, name);
  }

  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc != 3){
    fprintf(2, "usage: find dir name\n");
    exit(1);
  }

  find(argv[1], argv[2]);
  exit(0);
}
