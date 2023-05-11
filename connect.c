#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "MQTTClient.h"

#define MQTT_Uri    "tcp://172.16.21.16:1883"
#define ClientId    "rasp"
#define UserName    "admin"
#define PassWord    "xiangyun11"

//connectlost
void connect_lost(void *context, char *cause)
{
      printf("Connection lost,The reason is: %s \n",cause);
}
//receive topic
int message_arrived(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
	 printf("Receive topic: %s, mesage data: \n", topicName);
	 printf("%.*s\n", message->payloadlen, (char*)message->payload);
         MQTTClient_freeMessage(&message);
	 MQTTClient_free(topicName);
	 return 1;
 }



//publish topic success
void delivery_complete(void *context, MQTTClient_deliveryToken dt)
{
	printf("publish topic success,token = %d \n", dt);
}

int main(int argc,char* argv[])
{

	MQTTClient client;

	int rc;
	if ((rc = MQTTClient_create(&client, MQTT_Uri, ClientId, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
{	
	printf("Failed to create client, return code %d\n", rc);
		 exit(EXIT_FAILURE);
           	 goto exit;
}

//define MQTTClient connect struct
      MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	conn_opts.username =UserName;
	conn_opts.password = PassWord;
	conn_opts.keepAliveInterval = 60; 
	conn_opts.cleansession = 1; 

if ((rc = MQTTClient_setCallbacks(client, NULL, connect_lost, message_arrived, delivery_complete)) != MQTTCLIENT_SUCCESS)
{
        printf("Failed to set callback,return code %d\n",rc);
        rc=EXIT_FAILURE;
	goto destroy_exit;
}

//define a topic struct
       MQTTClient_message pubmsg=MQTTClient_message_initializer;
       char mag_data[]="I am rasp";
       pubmsg.payload=mag_data;
       pubmsg.payloadlen=(int)strlen(mag_data);
       pubmsg.qos=1;
       pubmsg.retained=0;
       

 //publish topic failure
 if((rc=MQTTClient_publishMessage(client,"rasp_publish",&pubmsg,&token))!=MQTTCLIENT_SUCCESS)
{
	printf("Fail to publish message,return code %d\n",rc);
	exit(EXIT_FAILURE);
}

//subscribe topic
if((rc=MQTTClient_subscribe(client,"rasp_suscribe",1))!=MQTTCLIENT_SUCCESS)
{
	printf("Failed to subscribe.return code %d\n",rc);
	rc=EXIT_FAILURE;
}

//wait for quit
printf("Press Q or q +<enter> to quit\n\n");
int ch;
do
{
 ch=getchar();
}
while(ch!='Q'&&ch!='q');
if ((rc = MQTTClient_disconnect(client, 10000)) != MQTTCLIENT_SUCCESS)
{
 printf("Failed to disconnect, return code %d\n",rc);
rc=EXIT_FAILURE;
}

destroy_exit;
MQTTClient_destroy(&client);
exit:
return rc;
}
