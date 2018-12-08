#include "liboss2/OSSClient.h"
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <fstream>
using namespace std;

char* osstype;
char* accesskey;
char* secretkey;
char* bucket;
char* objectPrefix;
char* zone;

#define DBG (0)

liboss2::client::OSSClient* client;

void Download();
ossObjectResult* ListObject();
void GetFile(char* bucketname, char* object, int filesize);
void ListBucket();



int main(int argc, char* argv[])
{
  osstype = strdup("Amazon");
  accesskey = strdup("AKIAIH7UJTSAWOAHE5FQ");
  secretkey = strdup("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");
  bucket = strdup("hwtoss");
  objectPrefix = strdup("netcdf");
  zone = strdup("ap-northeast-1");

  liboss2::client::ClientConf* conf = new ClientConf();
  conf->setOSSProvider(osstype);
  conf->setAccessKeyID(accesskey);
  conf->setSecretKey(secretkey);
  conf->setZone(zone);
  conf->setUseVirtualHosted(true);
  conf->setLogLevel(OSS_INFO);
  conf->setUseWriteCache(true);
  client = new OSSClient(*conf);
  //Download();
  ListBucket();
  return 0;
}

void ListBucket(){
  printf("\nList Bucket...\n");
  std::vector<ossBucketInfo> listBucket = client->listBucket();
  for(auto item:listBucket){
    printf("Name: %s\n", item.name);
    printf("Created: %s\n", item.created);
  }
  printf("List Bucket Done.\n");
}

ossObjectResult* ListObject()
{
  ossObjectResult* ossObject = client->listObject(bucket, objectPrefix);
#if DBG
  for (int i = 0; i < ossObject->nObjects; i++){
    printf("object %d: (%s, %lu)\n", i, ossObject->objects[i].key,
           ossObject->objects[i].size);
  }
#endif
  return ossObject;
}

void GetFile(char* bucketname, char* object, int filesize)
{
  char filename[1024] = {0};
  sprintf(filename, "./%s", object);
  FILE *fp = fopen(filename, "w");
  liboss2::client::GetObject* obj = client->getReadObject(bucketname, object, -1, -1);
  char responseBuffer[512];
  int rnum = 0;
  long totalread = 0;
  int bytes_left = filesize;
  while (bytes_left > 0){
    rnum = obj->read(responseBuffer, 512);
    if (rnum < 0){
      printf("QCloud read failed \n");
      break;
    }
    if (rnum == 0){
      break;
    }
    bytes_left -= rnum;
    totalread += rnum;
    char temp[1025];
    memset(temp, 0, sizeof(temp));
    strncpy(temp, responseBuffer, rnum);
    fputs(temp, fp);
  }
  printf("read data finished, total size:%ld\n",totalread);
  fclose( fp );
}

void Download()
{
  ossObjectResult* result = ListObject();
  for(int i = 0; i < result->nObjects; i++)
  {
    GetFile(bucket, result->objects[i].key, result->objects[i].size);
  }
}
