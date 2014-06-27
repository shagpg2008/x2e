
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
//#include <sys/time.h>
struct timeval{ 
	long tv_sec; 
	long tv_usec; 
};

#define PCAP_VERSION_MAJOR 2
#define PCAP_VERSION_MINOR 4
#define NEED_READ_YES 1
#define NEED_READ_NO  0

#define PCAP_HEADER_DATA(linktype, snaplen)   \
char __pcap_header_buff[] = {                 \
    (unsigned char)0xd4, (unsigned char)0xc3, (unsigned char)0xb2, (unsigned char)0xa1, /*magic*/           \
    (char)PCAP_VERSION_MAJOR, 0, (char)PCAP_VERSION_MINOR, 0, /*major, minor*/ \
    0,0,0,0,                /*gmt to local correction*/            \
    0,0,0,0,                /*accuracy of timestamps*/             \
    (char)((snaplen)&0xff), (char)((snaplen>>8)&0xff), (char)((snaplen>>16)&0xff), (char)((snaplen>>24)&0xff),  /*data link type*/  \
    (char)((linktype)&0xff), (char)((linktype>>8)&0xff), (char)((linktype>>16)&0xff), (char)((linktype>>24)&0xff)       /*max length saved portion of each pkt*/ \
}

struct pcap_pkt_hdr {  
    struct timeval ts;      /* time stamp */  
    unsigned int caplen;    /* length of portion present */  
    unsigned int len;       /* length this packet (off wire) */  
};  

static char buff[1<<10] = {0};
static int linktype = 101;
static time_t now = 0;
static int needread = NEED_READ_YES;

static void pcap_strip_chars(char *buff)
{
	char *tmp = buff;

	while(*tmp != 0) {
		if(((*tmp >= '0' && *tmp <= '9') 
			|| (*tmp >= 'a' && *tmp <= 'f')
			|| (*tmp >= 'A' && *tmp <= 'F')
			|| (*tmp == 'x' || *tmp == 'X')
			|| *tmp == ' ' || *tmp == ':')) {

			*buff = *tmp;
			buff++;
		}

		//strip "^C, ^M", etc.
		if(*tmp == '^') {
			tmp ++;
		}

		tmp++;
	}

	*buff = 0;
}

static FILE *pcap_file_open(const char *filename, unsigned int linktype, unsigned int snaplen )
{
    PCAP_HEADER_DATA(linktype, snaplen);
    FILE *fp =fopen(filename, "wb");
    if(fp == NULL) {
        return NULL;
    }
    
    fwrite(__pcap_header_buff, 1, sizeof(__pcap_header_buff), fp);
    return fp;
}

static unsigned int pcap_packet_write(FILE* fp, struct timeval tm, const char *pkt, unsigned int len)
{
    struct pcap_pkt_hdr hdr = {0};
	int neednthdr = linktype == 101 && (len > 14 && ((pkt[12] == 0x8 && (pkt[13] == 0x0 || pkt[13] == 0x6))));

	if(neednthdr) {
		pkt += 14;
		len -= 14;
	}

	hdr.ts = tm;
	hdr.caplen = len;
	hdr.len    = len;

    fwrite((const char*)&hdr, 1, sizeof(hdr), fp);
    len = fwrite(pkt, 1, len, fp);
    return len+sizeof(hdr);
}

static void pcap_check_lnkhdr(FILE *fp)
{
	char *tmp = NULL;
	while(!feof(fp) && (fgets(buff, sizeof(buff), fp))) {
		if(buff[0] == '\r' || buff[0] == '\n' || buff[1] == '\r' || buff[1] == '\n') {
            continue;
        }
        
        if(strstr(buff, " > ") != NULL) {
			needread = NEED_READ_NO;
            break;
        }

		if(strstr(buff, "tcpdump") == NULL
			&& strstr(buff, "T:") == NULL) {
			continue;
		}

		//tcpdump -Xls 0  -n
		tmp = strtok(buff, " ");
		if(tmp != NULL) {
			while((tmp = strtok(NULL, " ")) != NULL) {
				if(strstr(tmp, "xx") != NULL
					|| strstr(tmp, "XX") != NULL) {
					linktype = 1;
					return;
				}
			}
		} 

		//this is for DX campatiable.
		//Z1T:-i,el0,-xx,-s,0,udp
		tmp = strtok(buff, ",");
		if(tmp != NULL) {
			while((tmp = strtok(NULL, ",")) != NULL) {
				if(strstr(tmp, "xx") != NULL
					|| strstr(tmp, "XX") != NULL) {
					linktype = 1;
					return;
				}
			}
		}
	}
}

static int pcap_find_pkthdr(FILE *fp, struct timeval *tm)
{
    char *lptmp = NULL;
	int num = 0;
    
    while(!feof(fp) && (needread == NEED_READ_NO || fgets(buff, sizeof(buff), fp))) {
		needread = NEED_READ_YES;
        if(buff[0] == '\r' || buff[0] == '\n' || buff[1] == '\r' || buff[1] == '\n') {
            continue;
        }
        
        //hh:mm:ss.uuuuuu format
        if(strchr(buff, ':') != NULL) {
            unsigned int hour = 0, min = 0, sec = 0, us = 0;
            num = sscanf(buff, "%d:%d:%d.%d", &hour, &min, &sec, &us);
            tm->tv_sec  = now + hour*3600+min*60+sec;
            tm->tv_usec = us;
            //printf("now=%d,buf=%s,h:%d,m=%d,s:%d,us:%d\n", now, buff, hour, min, sec, us);
        } else {
            //ss.uuuuuu format
            num = sscanf(buff, "%d.%d", &tm->tv_sec, &tm->tv_usec);
        }

		if(num < 2) {
			continue;
		}
        
        return 1;
    }
    
    return 0;
}

static int pcap_find_pktdat(FILE *fp, unsigned char *data, unsigned int max)
{
    unsigned char *lptmp = NULL;
    int num = 0, len = 0, line = 0;
    
    while(!feof(fp) && fgets(buff, sizeof(buff), fp) && max > 0) {
        if(buff[0] == '\r' || buff[0] == '\n' || buff[1] == '\r' || buff[1] == '\n') {
            continue;
        }

		lptmp = buff;

        if((lptmp = (unsigned char *)strstr(buff, "0x")) == NULL
			|| (lptmp = (unsigned char *)strstr(buff, ": ")) == NULL) {
			if(len != 0) {
				needread = NEED_READ_NO;
				break;
			}
			continue;
        }

        lptmp++;
		pcap_strip_chars(lptmp);

        num = sscanf(lptmp, " %02X%02X %02X%02X %02X%02X %02X%02X %02X%02X %02X%02X %02X%02X %02X%02X", 
            data+0, data+1, data+2, data+3, data+4, data+5, data+6, data+7, 
            data+8, data+9, data+10, data+11, data+12, data+13, data+14, data+15);

        data += num;
        len += num;
        max -= num;

		if(num <= 0) {
			needread = NEED_READ_NO; //the next pkt head read, so doesn't read another line.
			break;
		}

        if(num < 16) {
             break;
        }
    }
    
    return len;
}

static void pcap_file_close(FILE *fp)
{
    if(fp != NULL) {
        fclose(fp);
    }
}

int convert_pcap_file(const char *fromf, const char *target)
{
    struct timeval tm = {0};
    char data[64<<10] = {0};
    int size = 0, num = 0;
	FILE *fp = NULL;
	FILE *from = fopen(fromf, "rb");

    if(from == NULL) {
        return -1;
    }
    
	memset(buff, 0, sizeof(buff));
	linktype = 101;
	needread = NEED_READ_YES;
	
	if(now == 0) {
        time_t timep = time(NULL);
        struct tm* p = localtime(&timep);
        struct tm tmp = *p;
        tmp.tm_sec  = 0;
        tmp.tm_min  = 0;
        tmp.tm_hour = 0;
        now = mktime(&tmp);
    }

	pcap_check_lnkhdr(from);

    fp = pcap_file_open(target, linktype, 0xffff);
	if(fp == NULL) {
		fclose(from);
		return -1;
	}

    while(pcap_find_pkthdr(from, &tm)) {
        size = pcap_find_pktdat(from, data, sizeof(data));
        if(size > 0) {
            pcap_packet_write(fp, tm, data, size);
			num++;
        }/* else {
			printf("Skip:%s\n", buff);
		}*/
    }
    
    pcap_file_close(fp);
	fclose(from);
    return num;
}


#ifdef __SHAGPG

int main(int argc, char *argv[])
{
	if(argv[1] == NULL || argv[2] == NULL) {
		printf("Usage: command <src file> <dst file>\n");
		return -1;
	}

	convert_pcap_file(argv[1], argv[2]);
}

#endif
