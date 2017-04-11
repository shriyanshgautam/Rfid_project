//============================================================================
// Name        : rfid4.cpp
// Author      : shriyansh
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


/**
 * https://www3.ntu.edu.sg/home/ehchua/programming/howto/EclipseCpp_HowTo.html
 * http://kosmisch.net/Blog/DotNetEssential/Archive/2016/3/16/create-and-debug-cc-programs-with-eclipse-and-cygwin-in-windows.html
 * use 32 bit cygwin and command setup-x86.exe -q -P wget -P gcc-g++ -P make -P diffutils -P libmpfr-devel -P libgmp-devel -P libmpc-devel –P gdb
 * set environment path C//cygwin/bin
 * copy RFIDAPI.dll and RFIDAPI.lib in lib folder and RFIDAPIEXPORT to includes folder and put both in project folder
 * specify paths and symbols - project right click > properties > c/c++ general > paths and symbols > gnu c++ >include select includes folder,>library paths select libs folder, libraries write RFIDAPI only
 * put RFIDAPI.dll besides the generated exe file to access it at run time
 * download libcurl.dll and libcurldll.a and libcurl.a 32 bit from https://curl.haxx.se/download.html#Win32
 * place libcurl.dll in debug folder for access to executable file
 * and do not link libcurl.a in libraries, link libcurldll.a as curldll in libraries, place libcurl.dll and libcurldll.a at same place
 * include header from curl
 * http post example from https://curl.haxx.se/libcurl/c/http-post.html
 * get zlib1.dll for 32 bit from https://www.opendll.com/index.php?file-download=zlib1.dll&arch=32bit&version=1.2.3.0&dsc=zlib%20data%20compression%20library#
 * and put it in debug folder for access to executable file
 *
 *
 * */



#include <iostream>
#include <RFIDAPIEXPORT.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sstream>
#include <string.h>
#include <curl/curl.h>

#include <typeinfo.h>


#define RFID_IP "192.168.0.239"
#define RFID_PORT 7086
#define SERVER "http://localhost:3000/read"

using namespace std;

string ips[] = {"192.168.0.238","192.168.0.239"};

 void *hp[100];

//void *hp;
CURL *curl;
CURLcode res;

struct thread_params {
    void* hp;
    int device_id;
    char* device_ip;
};


char * string_to_char(string str){
	char *cstr = new char[str.length() + 1];
	strcpy(cstr, str.c_str());
	return cstr;
}


void make_request(int id,string device_ip,int device_id){

	/* get a curl handle */
	  curl = curl_easy_init();
	  if(curl) {
	    /* First set the URL that is about to receive our POST. This URL can
	       just as well be a https:// URL if that is what should receive the
	       data. */
		  stringstream url;
		  url << SERVER << "/" <<  device_id ;
		  const char *URL = url.str().c_str();

	    curl_easy_setopt(curl, CURLOPT_URL, URL);
	    /* Now specify the POST data */
	    stringstream postString; // declare the string
	    postString << "device_ip=" << device_ip << "&device_id=" << device_id <<"&id=" << id ;
	    const char *data = postString.str().c_str();
	    //std::string s = std::to_string(id);
	    curl_easy_setopt(curl, CURLOPT_POSTFIELDS,data);
	    //dcurl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, 10L);
	    //get request
	    //curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

	    /* Perform the request, res will get the return code */
	    res = curl_easy_perform(curl);
	    /* Check for errors */
	    if(res != CURLE_OK)
	      fprintf(stderr, "curl_easy_perform() failed: %s\n",curl_easy_strerror(res));

	    /* always cleanup */
	    curl_easy_cleanup(curl);
	  }


}

void *read_card(void *context){

	int device_count = (int)context;

	unsigned char nBit=0;
	unsigned int iIdCode[device_count];
	int nRet[device_count];

    while(true){
    			int index;
    			for(index=0;index<device_count;index++){
    				         nRet[index] = SAAT_YRevIDMsgDec(hp[index], &iIdCode[index], &nBit);
    				         if(1 == nRet[index]) {
    				                printf("%s-%d ID Code is read as:%d \n",ips[index].c_str(),index ,iIdCode[index]);
    				                make_request(iIdCode[index],ips[index],index);
    				                //make_network_call(iIdCode);

    				          } else{
    				                //printf("No card in proximity %d\n",nRet);
    				          }

    			}
    }
    pthread_exit(NULL);
}

void close_connection(void *hp,char *ip,int device_id){
                    if(!SAAT_YPowerOff(hp)){
                        printf("ERROR -  Stop command failed %s-%d\n",ip,device_id);
                    }else{
                        printf("SUCCESS -  Stop command successful %s-%d\n",ip,device_id);

                        if(!SAAT_Close(hp)){
                            printf("ERROR - Closing connection failed %s-%d\n",ip,device_id);
                        }else{
                            printf("SUCCESS - Connection closed successfully %s-%d\nBYE!\n",ip,device_id);
                        }
                    }

}

bool initialise_connection(char *ip,int port,int device_id){
	//passing pointer to pointer as address is required
	if(!SAAT_TCPInit(&hp[device_id],ip,port) )
		{
			printf("ERROR - Reader initialization failed %s-%d!\n",ip,device_id);
			return false;
		}else{
			printf("SUCCESS - Reader initialised %s-%d\n",ip,device_id);
			return true;
		}
}

bool open_connection(char *ip,int device_id){
	if(!SAAT_Open(hp[device_id])) {
	        printf("ERROR - Connection opening failed %s-%d!\n",ip,device_id);
	        return false;
	 }else{
	        printf("SUCCESS - Connection opened %s-%d!\n",ip,device_id);
	        return true;
	 }
}


bool start_reading(char *ip,int device_id){
	 unsigned char optype = 0x01;
	 unsigned char idtype = 0x01;
	 if(!SAAT_YMakeTagUpLoadIDCode(hp[device_id], optype, idtype)) {
	        printf("ERROR - Reading command failed %s-%d!\n",ip,device_id);
	        return false;
	 }else{
		 	printf("SUCCESS - Started reading %s-%d!\n",ip,device_id);
		 	return true;
	 }
}

void read_in_thread(string *ip,int device_count){


	int rc;
	pthread_t thread;
	pthread_attr_t attr;
	void *status;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);


				rc = pthread_create(&thread, &attr, read_card,(void*)device_count );

				if (rc){
					  cout << "Error:unable to create thread," << rc << endl;
					  //exit(-1);
				}




	        pthread_attr_destroy(&attr);


	        	rc = pthread_join(thread, &status);

	        	if (rc){
	        		cout << "Error:unable to join," << rc << endl;
	        		//exit(-1);
	        	}


	pthread_exit(NULL);
}

void init_network(){
    curl_global_init(CURL_GLOBAL_ALL);
}

void close_network(){
    curl_global_cleanup();
}


bool start_process(char *ip,int port,int reader_id){
	if(initialise_connection(ip,port,reader_id)){
	        if(open_connection(ip,reader_id)) {
	            if(start_reading(ip,reader_id)) {
	            	return true;

	            }
	        }
	}
	return false;
}




int main()
{


    //string ips[] = {"192.168.0.238","192.168.0.239"};

    int device_count = sizeof( ips ) / sizeof(string);

    //void *hp[device_count];

    init_network();

    int index;
    for(index=0;index<device_count;index++){
    	start_process(string_to_char(ips[index]),RFID_PORT,index);
    }

    read_in_thread(ips,device_count);


  	for(index=0;index<device_count;index++){
  	   close_connection(hp[index],string_to_char(ips[index]),index);
  	}



  	close_network();


	/*if(initialise_connection(&hp,string_to_char(ips[1]),RFID_PORT) ){
        if(open_connection(hp)) {
            if(start_reading(hp)) {
                    init_network();
                    read_in_thread(hp);
                    close_connection(hp);
                    close_network();
            }
        }
	}*/
	cin.get();
    return 0;
}




