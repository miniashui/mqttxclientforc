#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "MQTTClient.h"

#define MQTT_Uri    "tcp://172.16.21.16:1883"   
#define ClientId    "rasp"                  
#define UserName    "admin"                 
#define PassWord    "xiangyun11"                    

//disconnect getback
void connect_lost(void *context, char *cause)
{
    printf("Connection lost,The reason: %s \n",cause);
}

// receive topic getback
int message_arrived(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    printf("Receive topic: %s, message data: \n", topicName);
    printf("%.*s\n", message->payloadlen, (char*)message->payload);
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

// topic success getback
void delivery_complete(void *context, MQTTClient_deliveryToken dt)
{
    printf("publish topic success,token = %d \n", dt);
}

int main(int argc, char* argv[])
{
    // 1、define mqtt client struct
    MQTTClient client;

    // 2、MQTT client
    int rc;
    if ((rc = MQTTClient_create(&client, MQTT_Uri, ClientId, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to create client, return code %d\n", rc);
        exit(EXIT_FAILURE);
        goto exit;
    }

    // 3、define mqtt connect struct
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.username = UserName;         
    conn_opts.password = PassWord;          
    conn_opts.keepAliveInterval = 60;       
    conn_opts.cleansession = 1;             
    
    // 4、connect getback
    if ((rc = MQTTClient_setCallbacks(client, NULL, connect_lost, message_arrived, delivery_complete)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to set callbacks, return code %d\n", rc);
        rc = EXIT_FAILURE;
        goto destroy_exit;
    }

    // 5、connect to mqtt server
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
        goto destroy_exit;
    }

    // 6、define struct
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    char mag_data[] = "I am Ubuntu16.";
    pubmsg.payload = mag_data;
    pubmsg.payloadlen = (int)strlen(mag_data);
    pubmsg.qos = 1;                
    pubmsg.retained = 0;           
    MQTTClient_deliveryToken token; 
    // 7、publish topic
    if ((rc = MQTTClient_publishMessage(client, "ubuntu16_publish", &pubmsg, &token)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to publish message, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

    // 8、subscribe
    if ((rc = MQTTClient_subscribe(client, "ubuntu16_subscribe", 1)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to subscribe, return code %d\n", rc);
        rc = EXIT_FAILURE;
    }

    // 9、wait quit
    printf("Press Q or q + <Enter> to quit\n\n");
    int ch;
    do
    {
            ch = getchar();
    } while (ch!='Q' && ch != 'q');

    if ((rc = MQTTClient_unsubscribe(client, "ubuntu16_subscribe")) != MQTTCLIENT_SUCCESS)
    {
            printf("Failed to unsubscribe, return code %d\n", rc);
            rc = EXIT_FAILURE;
    }

    // 10、disconnect
    if ((rc = MQTTClient_disconnect(client, 10000)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to disconnect, return code %d\n", rc);
        rc = EXIT_FAILURE;
    }

destroy_exit:
    MQTTClient_destroy(&client);
exit:
    return rc;
}

