/* The neopets Scratchcard Autobuyer (ScAb) is designed to automatically buy a scratchcard when available */

#define USERNAME "milkbreadperson"
#define PASSWORD

#define COOKIE "cookie"

#define MINUTE 60
#define HOUR (60*MINUTE)
#define WAIT_TIME 6
#define RANDOM_WAIT_MAX 20


#define _GNU_SOURCE
#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

/* used to make curl not write output to stdout */
size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
   return size * nmemb;
}

//////////////////////////
//neopets login
//////////////////////////
void neopets_login(char* username, char* password){
    CURL* curl = curl_easy_init();
    char* post;
    //set options for login
    curl_easy_setopt(curl, CURLOPT_URL, "http://www.neopets.com/login.phtml");
    curl_easy_setopt(curl, CURLOPT_COOKIEJAR, COOKIE);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);

    asprintf(&post, "username=%s&password=%s", username, password);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post);

    //perform login
    curl_easy_perform(curl);

    //cleanup
    curl_easy_cleanup(curl);
    free(post);
}

///////////////////////
//HTTP GET
///////////////////////
CURLcode http_get(char* url, char* ref){
    CURL* curl = curl_easy_init();
    CURLcode res;
	
    //set options for get
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, COOKIE);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_REFERER, ref);

    //perform get
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    return res;
}


///////////////////////
//HTTP POST
///////////////////////
CURLcode http_post(char* url, char* post, char* ref){
    CURL* curl = curl_easy_init();
    CURLcode res;
	
    //set options for get
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post);
    curl_easy_setopt(curl, CURLOPT_COOKIEFILE, COOKIE);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
    curl_easy_setopt(curl, CURLOPT_REFERER, ref);

    //perform post
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    return res;
}

//////////////////////
//buy scratchcard
//////////////////////
void buy_sc(){
    /* login to neopets */
    syslog(LOG_INFO, "Loging in to neopets");
    neopets_login(USERNAME, PASSWORD);
    syslog(LOG_INFO, "Login complete");

    /* scratchcard request */
    http_get("http://www.neopets.com/winter/kiosk.phtml", "http://www.neopets.com/winter/");
    http_post("http://www.neopets.com/winter/process_kiosk.phtml", "", "http://www.neopets.com/winter/kiosk.phtml");
    syslog(LOG_INFO, "Bought a winter scratchcard");

    /* logout */
    http_get("http://www.neopets.com/logout.phtml", "");
    syslog(LOG_INFO, "Logged out");
}


//////////////////////
//main
//////////////////////
int main(){
   
//    CURLcode response;

    //init syslog
    openlog("ScAb", LOG_PERROR | LOG_PID | LOG_NDELAY, LOG_USER);
    
    //set random seed for waiting
    srand(time(NULL));

    while(1){
	buy_sc();
	int wait_time = (WAIT_TIME * HOUR) + ((rand()%RANDOM_WAIT_MAX) * MINUTE);
	syslog(LOG_INFO, "Waiting for %d minutes", wait_time/MINUTE);
	sleep(wait_time);


    }

    //clean up
    closelog();


    exit(EXIT_SUCCESS);
}
