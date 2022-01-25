
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define VERSION "1.0"
#define DEBUG_ALL
#ifdef DEBUG_ALL
    #define DEBUG_E
    #define DEBUG_D
#endif

#define LOGI(msg,...) printf(msg, ##__VA_ARGS__)

#ifdef DEBUG_E
	#define LOGE(msg,...) {printf("error:");printf(msg, ##__VA_ARGS__);}
#else
    #define LOGE(msg,...)
#endif

#ifdef DEBUG_D
    #define LOGD(msg,...) {printf(msg, ##__VA_ARGS__);}
#else
    #define LOGD(msg,...)
#endif

typedef struct {
	unsigned short 		file_type;
	unsigned int 		file_size;
	unsigned short 		reserved1;
	unsigned short 		reserved2;
	unsigned int 		offset;
} __attribute__((packed)) bmp_file_header;

typedef struct  {
	unsigned int 		bmp_info_size;
	int 				bmp_width;
	int 				bmp_height;
	unsigned short 		planes;
	unsigned short 		image_depth;
	unsigned int 		compression;
	unsigned int 		image_size;
	int 				x_pels_permeter;
	int 				y_pels_permeter;
	unsigned int 		color_used;
	unsigned int 		color_important;
} __attribute__((packed)) bmp_info_header;

typedef struct  {
	unsigned char 		blue;
	unsigned char 		green;
	unsigned char 		red;
	unsigned char 		reserved;
} __attribute__((packed)) bmp_palette;

void version()
{
    printf("bmp2array version %s\n",VERSION);
#ifdef __DATE__
          printf(("compiled %s"), __DATE__);
#endif
    printf("\n");
    printf("Copyright (C) Guo Yaoxin\n\n");
    printf("Welcome to my channel 科G栈\n");
    printf("bilibili:https://space.bilibili.com/476114390\n"
            "github:https://github.com/gcrisis\n"
            "youtube:https://www.youtube.com/channel/UCrn3-hdeEaM_4v1Xyn85cag\n");
    printf("\nThis program is free software; you can redistribute it and/or\n"
               "modify it under the terms of the GNU General Public License v3.0 \n"
               "as published by the Free Software Foundation.\n\n");
   
}
void help()
{
    printf(
        "Usage: bmp2array [OPTION]...[-i] filename\n"
        "Option:\n"
        "   -i :use when your filename isn't at last position\n"
        "   -B :output array element size in bytes,default 1\n"
        "   -M :change byte order to MSB,default LSB\n"
        "   -o :output file name\n"
        "   -r :reverse line order\n" 
        "   -h :show help\n"
        "   -v :output version information and exit\n\n"
        );
    version();
}
int main(int argc, char **argv)
{		
//图片
	bmp_file_header file_head;
	bmp_info_header info_head;
	size_t bytes_num;
	char * filepath = argv[argc-1]; //图片路径默认最后
	char * filename = NULL;
    unsigned char *bmpBuffer;
    uint8_t reverse_line_order = 0;  //0:keep,1:reverse
	
//数组
	uint8_t array_element_bytes = 1;
	uint8_t byte_order = 1;  //1:lsb,0:msb
//解析输入参数
    int c;
    for (int i=0;i<argc;i++)
        LOGD("%s\n",argv[i]);
    while((c = getopt(argc,argv,"iB:Mo:rhv"))!=-1){
        LOGD("%c %s\n",c,argv[optind]);
        switch (c)
        {
            case 'B':
                array_element_bytes = atoi(optarg);
                break;
            case 'm':
                byte_order = 0;
                break;
            case 'o':
                filename = malloc(strlen(optarg));
                memcpy(filename,optarg,strlen(optarg));
                break;
            case 'r':
                reverse_line_order = 1;
                break;
            case 'i':
                filepath = optarg;
                break;
            case 'h':
                help();
                exit(0);
                break;
            case 'v':
                version();
                exit(0);
            default:
                LOGD("%c",c);
                break;
        }
    }
//打开图片
	FILE *pFile = fopen(filepath, "r");
	if (pFile == NULL) {
		LOGE("file not exist\n");
		return 0;
	}
//读取头信息
	fread(&file_head, 1, sizeof(file_head), pFile);
	fread(&info_head, 1, sizeof(info_head), pFile);
	
	if(info_head.bmp_height<0)
	{
        LOGI("The picture's line order is reversed\n");
		info_head.bmp_height=-info_head.bmp_height;
	}
		
	LOGD("filehead:%x %d %d\n",file_head.file_type,file_head.file_size,file_head.offset);
	LOGD("infohead:%d\n",info_head.bmp_info_size);
	LOGD("%d %d %d %d %d %d %d %d %d %d\n",info_head.bmp_width,info_head.bmp_height,info_head.planes
										  ,info_head.image_depth,info_head.compression,info_head.image_size
										  ,info_head.x_pels_permeter,info_head.y_pels_permeter,info_head.color_used
										  ,info_head.color_important);
//读取rgb数据	
	bytes_num = info_head.bmp_width * info_head.bmp_height * info_head.image_depth/8;
	bmpBuffer = malloc(bytes_num);
	fseek(pFile, file_head.offset, 0);
	fread(bmpBuffer, 1, bytes_num, pFile);
	fclose(pFile);
    if(reverse_line_order) //翻转行序
    {
        int linebytes = info_head.bmp_width * info_head.image_depth/8;
        char * linebuffer = malloc(linebytes);
        char (*p)[linebytes] = (char (*)[linebytes])bmpBuffer;
        for(int i=0;i<info_head.bmp_height/2;i++)
        {
            memcpy(linebuffer,p[i],linebytes);
            memcpy(p[i],p[info_head.bmp_height-1-i],linebytes);
            memcpy(p[info_head.bmp_height-1-i],linebuffer,linebytes);
        }
    }
//输出数组
	if(!filename)
    {
        filepath = strtok(filepath,".");
        if(strrchr(filepath,'/'))
        {
          	filepath = strrchr(filepath,'/')+1;
        }
        filename = malloc(strlen(filepath)+1);
        memcpy(filename,filepath,strlen(filepath)+1);
    }	
    LOGD("%s %s",filename,filepath);
	FILE *out = fopen(strcat(filepath,".h"),"w+");
	fprintf(out,"const uint%d_t %s[%ld] PROGMEM ={\n\n",array_element_bytes*8,filename,bytes_num/array_element_bytes);
    
    for(int i=0,n=1,divisor = array_element_bytes<4?16:8;i<bytes_num;i+=array_element_bytes,n++)
    {
        fprintf(out,"0x");
        if(byte_order)
        {
            for(int j=array_element_bytes-1;j>=0;j--)
                fprintf(out,"%02x",bmpBuffer[i+j]);
        }
        else
        {
            for(int j=0;j<array_element_bytes;j++)
                fprintf(out,"%02x",bmpBuffer[i+j]);
        }
        
        fprintf(out,",");
        if(n%divisor == 0)	// 8/16位16个换一行，32/64位8个换一行
            fprintf(out,"\n");
    }

	
	fprintf(out,"\n};");
    fclose(out);
    free(filename);
	free(bmpBuffer);
    return 0;
}

