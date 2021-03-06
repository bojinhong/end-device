#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <time.h>

#define	MODEMDEVICE	"/dev/ttyUSB0"
#define DEBUG 0


void crc_chk(unsigned char* data, unsigned char* crc1, unsigned char* crc2, unsigned char length)
{
	int j;
	unsigned short reg_crc=0xFFFF;
	while(length--)
	{
		reg_crc ^= *data++;
		for(j=0;j<8;j++)
		{
			if(reg_crc & 0x01)
				reg_crc=(reg_crc>>1) ^ 0xA001;
			else
				reg_crc=reg_crc >>1;
		}
	}
	*crc1 = (unsigned char)(reg_crc & 0xff);
	*crc2 = (unsigned char)((reg_crc & 0xff00) >> 8);
}

void set_command(int uart_fd, unsigned char buff1[])
{
	unsigned char crc1, crc2;
	unsigned char buff2[10];
	int ret, ii;
	int	get_len;
	
	crc_chk(buff1, &crc1, &crc2, 6); 

	buff1[6] = crc1;		//0x85
	buff1[7] = crc2;		//0xB2

#if DEBUG == 1
	printf("Send Data :[ %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x ]\n", 
						buff1[0], 
						buff1[1], 
						buff1[2], 
						buff1[3], 
						buff1[4], 
						buff1[5], 
						buff1[6], 
						buff1[7]);
#endif

	ret = write(uart_fd, buff1, 8);
	if (ret != 8) 
	{
		printf(" Error!\n");
	}

	read(uart_fd, &buff2[0], 1);
	read(uart_fd, &buff2[1], 1);
	read(uart_fd, &buff2[2], 1);

	get_len = buff2[2];

#if DEBUG == 1
	printf("Recv Data :[ %02x %02x %02x ", 
						buff2[0],
						buff2[1],
						buff2[2]
					);
#endif

	for (ii=0; ii < get_len + 2; ii++) 
	{       
		read(uart_fd, &buff2[ii+3], 1);

#if DEBUG == 1
		printf("%02x ", buff2 [ii+3]);
#endif
	}

#if DEBUG == 1
	printf("] \n");
#endif

}

void get_command(int uart_fd, unsigned char buff1[])
{
	unsigned char crc1, crc2;
	unsigned char buff2[100];
        char tmp[20];
	int ret, ii;
	int	get_len;
	
	crc_chk(buff1, &crc1, &crc2, 6); 

	buff1[6] = crc1;		//0x85
	buff1[7] = crc2;		//0xB2

#if DEBUG == 1
	printf("Send Data :[ %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x ]\n", 
						buff1[0], 
						buff1[1], 
						buff1[2], 
						buff1[3], 
						buff1[4], 
						buff1[5], 
						buff1[6], 
						buff1[7]);
#endif

	ret = write(uart_fd, buff1, 8);
	if (ret != 8) 
	{
		printf(" Error!\n");
	}

	read(uart_fd, &buff2[0], 1);
	read(uart_fd, &buff2[1], 1);
	read(uart_fd, &buff2[2], 1);

	get_len = buff2[2];

#if DEBUG == 1
	printf("Recv Data :[ %02x %02x %02x \n", 
						buff2[0],
						buff2[1],
						buff2[2]
					);
#endif

	for(ii=0; ii < get_len + 2; ii++) 
	{
		read(uart_fd, &buff2[ii+3], 1);
#if DEBUG == 1
		printf("%02x ", buff2 [ii+3]);
#endif
	}

#if DEBUG == 1
	printf("] \n");
#endif
        sprintf(tmp,"%d,%.1f,%d,%d",(int)buff2[4],((float)buff2[6])/2,(int)buff2[8],(int)buff2[10]);
        printf("%s\n",tmp);
}

void get_command1(int uart_fd, unsigned char buff1[])
{
	unsigned char crc1, crc2;
	unsigned char buff2[100];
        char tmp[20];
	int ret, ii;
	int	get_len;
	
	crc_chk(buff1, &crc1, &crc2, 6); 

	buff1[6] = crc1;		//0x85
	buff1[7] = crc2;		//0xB2

#if DEBUG == 1
	printf("Send Data :[ %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x ]\n", 
						buff1[0], 
						buff1[1], 
						buff1[2], 
						buff1[3], 
						buff1[4], 
						buff1[5], 
						buff1[6], 
						buff1[7]);
#endif

	ret = write(uart_fd, buff1, 8);
	if (ret != 8) 
	{
		printf(" Error!\n");
	}

	read(uart_fd, &buff2[0], 1);
	read(uart_fd, &buff2[1], 1);
	read(uart_fd, &buff2[2], 1);

	get_len = buff2[2];

#if DEBUG == 1
	printf("Recv Data :[ %02x %02x %02x \n", 
						buff2[0],
						buff2[1],
						buff2[2]
					);
#endif

	for(ii=0; ii < get_len + 2; ii++) 
	{
		read(uart_fd, &buff2[ii+3], 1);
#if DEBUG == 1
		printf("%02x ", buff2 [ii+3]);
#endif
	}

#if DEBUG == 1
	printf("] \n");
#endif
        printf("%d\n",(int)buff2[4]);
}

int main(int argc,char *argv[])
{
        unsigned char command[8]={0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
	struct termios	tio;
	int 			tty_fd;
	float			value;

	memset(&tio,0,sizeof(tio));

	tio.c_iflag=0;
	tio.c_oflag=0;
	tio.c_cflag=CS8|CREAD|CLOCAL|PARENB|PARODD;           // 8n1, see termios.h for more information

	tio.c_lflag=0;
	tio.c_cc[VMIN]=1;
	tio.c_cc[VTIME]=5;

	tty_fd=open(MODEMDEVICE, O_RDWR);      
	if (tty_fd > 0) 
 	{
#if DEBUG == 1
		printf("message :device '%s' is open, fd = %d \n\n", MODEMDEVICE, tty_fd);
#endif
	}
	else 
	{
		printf("Error :device '%s' can't be open, fd = %d \n\n", MODEMDEVICE, tty_fd);
	}
	cfsetospeed(&tio,B9600);
	cfsetispeed(&tio,B9600);

	tcsetattr(tty_fd,TCSANOW,&tio);

        if(strcmp(argv[1],"set")==0)
        {
                command[0]=0x01;
         	command[1]=0x06;
                command[2]=(unsigned char)atoi(argv[2]);
                command[4]=0x00;

                if(strcmp(argv[3],"power")==0)
                {	
                	if(strcmp(argv[2],"0")==0)
       			{
                		command[3]=0x07;
                    	}
                    	else
                    	{
                        	command[3]=0x10;
			}

                        command[5]=(unsigned char)atoi(argv[4]);
		}

                if(strcmp(argv[3],"temp")==0)
                {
                        if(strcmp(argv[2],"0")==0)
       			{
                		command[3]=0x08;
                        }
                        else
                        {
                                command[3]=0x1B;
			}

	        	command[5]=(unsigned char)atoi(argv[4]); 
		}
                
                if(strcmp(argv[3],"mode")==0)
                {
                        if(strcmp(argv[2],"0")==0)
       			{
                		command[3]=0x09;
                        }
                        else
                        {
                                command[3]=0x12;
			}
              
	                command[5]=(unsigned char)atoi(argv[4]);
		}

                if(strcmp(argv[3],"fan")==0)
                {
                        if(strcmp(argv[2],"0")==0)
       			{
                		command[3]=0x0A;
                        }
                        else
                        {
                                command[3]=0x13;
			}
                	
			command[5]=(unsigned char)atoi(argv[4]);
		}

                if(strcmp(argv[3],"lock")==0)
                {
                        if(strcmp(argv[2],"0")==0)
       			{
                		command[3]=0x0B;
                        }
                        else
                        {
                                command[3]=0x11;
			}
                	
			command[5]=(unsigned char)atoi(argv[4]);
		}
                
                set_command(tty_fd, command);
        }

        if(strcmp(argv[1],"get")==0)
        {
                command[0]=0x01;
         	command[1]=0x03;
                command[2]=(unsigned char)atoi(argv[2]); 
                command[4]=0x00;

                if(strcmp(argv[2],"0")==0)
                {
			command[3]=0x07;
                        command[5]=0x05;
                }

                get_command(tty_fd, command);
        }

        if(strcmp(argv[1],"cset")==0)
 	{
    		command[0]=(unsigned char)atoi(argv[2]); 
    		command[1]=(unsigned char)atoi(argv[3]); 
    		command[2]=(unsigned char)atoi(argv[4]); 
    		command[3]=(unsigned char)atoi(argv[5]); 
    		command[4]=(unsigned char)atoi(argv[6]); 
    		command[5]=(unsigned char)atoi(argv[7]); 
		set_command(tty_fd, command);
 	}

        if(strcmp(argv[1],"cget")==0)
 	{
    		command[0]=(unsigned char)atoi(argv[2]); 
    		command[1]=(unsigned char)atoi(argv[3]); 
    		command[2]=(unsigned char)atoi(argv[4]); 
    		command[3]=(unsigned char)atoi(argv[5]); 
    		command[4]=(unsigned char)atoi(argv[6]); 
    		command[5]=(unsigned char)atoi(argv[7]); 
		get_command1(tty_fd, command);
 	}        

	close(tty_fd);

	return EXIT_SUCCESS;
}


