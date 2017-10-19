#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <elf.h>

typedef struct virus virus;
typedef struct linked linked;

struct virus {
    unsigned short length;
    char name[16];
    char signature[];
};
 
struct linked {
    virus *v;
    linked *next;
};

void PrintHex(unsigned char *buffer, long length);

unsigned char* getFileData(char * fileName);

int getFileSize(char * fileName);

int readVirus(int index, unsigned char * buffer, struct virus ** viruses, int virusesIndex);

void printVirus(virus *v);

/* Print the data of every linked in list. Each item followed by a newline character. */
void list_print(linked *virus_list); 

/* Add a new linked with the given data to the list 
  (either at the end or the beginning, depending on what your TA tells you),
  and return a pointer to the list (i.e., the first linked in the list).
  If the list is null - create a new entry and return a pointer to the entry. */
linked* list_append(linked* virus_list, virus* data); 
 
/* Free the memory allocated by the list. */
void list_free(linked *virus_list);

/* The detect_virus function compares the content of the buffer byte-by-byte with the 
 * virus signatures stored in the virus_list linkeded list. */
void detect_virus(char *buffer, linked *virus_list, unsigned int size);

/* compare if 'contained' is contained in text */
int isVirusThere(char* text, virus* v, unsigned int size);

int main(int argc, char **argv) {
  int index = 0;
  int virusesIndex = 0;
  unsigned char * buffer = getFileData("signatures");
  struct virus * viruses[11];
  linked * virus_list = NULL;
  
  /* the core of the algorithm */
  /* iteration over viruses - saves them in a linkededlist */
  while(virusesIndex < 11)
  {
    index = readVirus(index, buffer, viruses, virusesIndex);
    virusesIndex++;
  }
  virusesIndex =  0;
  
  while(virusesIndex < 11)
  {
    virus_list = list_append(virus_list, viruses[virusesIndex]);
    virusesIndex++;
  }
  
  /* loads the checked file data */
  char * checkedFileBuffer = getFileData("ELFexec");
  
  if(argc > 1 && strcmp(argv[1], "-f") == 0)
    detect_virus_f(checkedFileBuffer, virus_list, getFileSize("ELFexec"));
  else
    detect_virus(checkedFileBuffer, virus_list, getFileSize("ELFexec"));
  
  list_free(virus_list);
    
  /* terminate */
  free (buffer);
  return 0;
}
void PrintHex(unsigned char *buffer, long length)
{
  int i = 0;
  while(i != length)
  {
    printf("%02X ", buffer[i]);
    i++;
  }
}

int getFileSize(char * fileName)
{
  FILE *fp;
  long size = 0;
  if(!(fp = fopen(fileName, "r")))
  {
    fputs ("File error",stderr); 
    exit (1);
  }
  
  /* obtain file size: */
  fseek (fp , 0 , SEEK_END);
  size = ftell (fp);
  
  fclose (fp);
  return size;
}

unsigned char* getFileData(char * fileName)
{
  FILE *fp;
  long size = 0;
  unsigned char * buffer;
  size_t result = 0;
  if(!(fp = fopen(fileName, "r")))
  {
    fputs ("File error1", stderr); 
    exit (1);
  }
  
  /* obtain file size: */
  fseek (fp , 0 , SEEK_END);
  size = ftell (fp);
  rewind (fp);
  
  /* allocate memory to contain the whole file: */
  buffer = (unsigned char*) malloc (sizeof(unsigned char)*size);
  if (buffer == NULL) {
    fputs ("Memory error2",stderr);
    exit (2);
  }
  
  /* copy the file into the buffer: */
  result = fread (buffer,1,size,fp);
  if (result != size) {
    fputs ("Reading error3",stderr); 
    exit (3);
  }
  
  fclose (fp);
  return buffer;
}

int readVirus(int index, unsigned char * buffer, struct virus ** viruses, int virusesIndex)
{
  int size;
  size = 256 * buffer[index] + buffer[index + 1] - 18;
  index += 2;
  
  viruses[virusesIndex] = malloc(sizeof(struct virus) + size + 16);
  
  viruses[virusesIndex]->length = size;
  
  memcpy(viruses[virusesIndex]->name, buffer + index, 16);
  index += 16;
  
  memcpy(viruses[virusesIndex]->signature, buffer + index, viruses[virusesIndex]->length);
  index += viruses[virusesIndex]->length;
  
  return index;
}

void printVirus(virus *v)
{
  printf("Virus name: %s\n", v->name);
  printf("Virus size: %d\n", v->length);
  printf("signature:\n");
  
  PrintHex(v->signature, v->length);
  
  printf("\n\n");
}

void list_print(linked *virus_list)
{
  if(virus_list)
  {
    printVirus(virus_list->v);
    list_print(virus_list->next);
  }
}

linked* list_append(linked* virus_list, virus* data)
{
  linked* temp = malloc(sizeof *temp);
  temp->v = data;
  temp->next = NULL;
  if(!virus_list)
    return temp;
  else
  {
    linked * currentlinked = virus_list;
    while(currentlinked->next)
      currentlinked = currentlinked->next;
    
    currentlinked->next = temp;
  }
  return virus_list;
}

void list_free(linked *virus_list)
{
  free(virus_list->v);
  if(virus_list->next)
    list_free(virus_list->next);
  free(virus_list);
}

void detect_virus_f(char *buffer, linked *virus_list, unsigned int size)
{
  linked *currentlinked = virus_list;
  int virusIndex = 0;
  int chosenOne = -1;
  virus *chosenVirus;
  int virusStatingPoint[11];
  int min = size + 1;
    
  while(currentlinked)
  {
    virusStatingPoint[virusIndex] = isVirusThere(buffer, currentlinked->v, size);
    if(virusStatingPoint[virusIndex] < min && virusStatingPoint[virusIndex] != -1)
    {
      min = virusStatingPoint[virusIndex];
      chosenOne = virusIndex;
      chosenVirus = currentlinked->v;
    }
    
    virusIndex++;
    currentlinked = currentlinked->next;
  }
  if(chosenOne != -1)
  {
    printf("Virus Found:\n");
    printf("starting byte location: %d\n", virusStatingPoint[chosenOne]);
    printf("virus name: %s\n", chosenVirus->name);
    printf("size of the virus signature: %d\n", strlen(chosenVirus->signature));
    printf("\n");
  }
}
char * getShType(int type)
{
  switch(type)
  {
    case SHT_NULL:      return "NULL";
    case SHT_PROGBITS:    return "PROGBITS";
    case SHT_SYMTAB:    return "SYMTAB";
    case SHT_STRTAB:    return "STRTAB";
    case SHT_RELA:      return "RELA";
    case SHT_HASH:      return "HASH";
    case SHT_DYNAMIC:     return "DYNAMIC";
    case SHT_NOTE:      return "NOTE";
    case SHT_NOBITS:    return "NOBITS";
    case SHT_REL:       return "REL";
    case SHT_SHLIB:     return "SHLIB";
    case SHT_DYNSYM:    return "DYNSYM";
    case SHT_INIT_ARRAY:  return "INIT_ARRAY";
    case SHT_FINI_ARRAY:  return "FINI_ARRAY";
    case SHT_PREINIT_ARRAY: return "PREINIT_ARRAY";
    case SHT_GROUP:       return "GROUP";
    case SHT_SYMTAB_SHNDX:  return "SYMTAB_SHNDX";
    case SHT_NUM:       return "NUM";
    case SHT_LOOS:      return "LOOS";
    case SHT_GNU_LIBLIST:   return "GNU_LIBLIST";
    case SHT_CHECKSUM:    return "CHECKSUM";
    case SHT_SUNW_move:   return "MOVE";
    case SHT_SUNW_COMDAT:   return "COMDAT";
    case SHT_SUNW_syminfo:  return "SYMINFO";
    case SHT_GNU_verdef:  return "VERDEF";
    case SHT_GNU_verneed:   return "VERNEED";
    case SHT_GNU_versym:  return "VERSYM";
    case SHT_LOPROC:    return "LOPROC";
    case SHT_HIPROC:    return "HIPROC";
    case SHT_LOUSER:    return "LOUSER";
    case SHT_HIUSER:    return "HIUSER";

  }
  return "";
}

void detect_virus(char *buffer, linked *virus_list, unsigned int size)
{
  linked *currentlinked = virus_list;
  int virusIndex;
  int virusStatingPoint;
  int i;
  
  while(currentlinked)
  {
    if((virusStatingPoint = isVirusThere(buffer, currentlinked->v, size)) != -1)
    {
      printf("Virus Found:\n");
      printf("starting byte location: %d\n", virusStatingPoint);
      printf("virus name: %s\n", currentlinked->v->name);
      printf("\ncleaning virus!\n");

      int currentfd = -1;
      void *map_start;
      Elf32_Ehdr *header;
      struct stat fd_stat;
      if((currentfd = open("ELFexec", O_RDWR)) < 0 ) {
      perror("error in open");
        return;
      }

      if( fstat(currentfd, &fd_stat) != 0 ) {
        perror("stat failed");
        close(currentfd);
            return;
      }

      if ( (map_start = mmap(0, fd_stat.st_size, PROT_READ | PROT_WRITE , MAP_SHARED, currentfd, 0)) == MAP_FAILED ) {
        perror("mmap failed");
        close(currentfd);
        return;
      }
      header = (Elf32_Ehdr *) map_start;

      Elf32_Shdr * string_tbl_section = (Elf32_Shdr *)(map_start + header->e_shoff + (header->e_shstrndx * header->e_shentsize));

      Elf32_Shdr * sectionHeaderRow;

      for(i = 0; i < header->e_shnum; i++)
      {
        sectionHeaderRow = (Elf32_Shdr *) (map_start + header->e_shoff + header->e_shentsize * i);
        if(strcmp((char *)(map_start + string_tbl_section->sh_offset + sectionHeaderRow->sh_name), ".text") == 0)
        {
          int entryPoint = sectionHeaderRow->sh_addr;

          lseek(currentfd, 24, SEEK_SET);
          
          write(currentfd, &entryPoint, 4);
          return 0;
        }
      }

      close(currentfd);
    }
    
	
    currentlinked = currentlinked->next;
  }
}

int isVirusThere(char* text, virus* v, unsigned int size)
{
  int startingLetter = 0;
  while(strlen(v->signature) <= size - startingLetter )
  {
    if(memcmp(text + startingLetter, v->signature, v->length) == 0)
      return startingLetter;
    startingLetter++;
  }
  return -1;
}