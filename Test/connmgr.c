#define _GNU_SOURCE
#include "connmgr.h"


typedef struct pollfd poll_fd_t;

typedef struct
{
    time_t last_record;
    sensor_id_t sensor_id;
    tcpsock_t *socked_id;
    poll_fd_t poll_fd;
} sensor_nd;



void *sensor_copy(void *element)
{
    sensor_nd *copy = malloc(sizeof(sensor_nd));
    copy->last_record = ((sensor_nd *)element)->last_record;
    copy->sensor_id = ((sensor_nd *)element)->sensor_id;
    copy->socked_id = ((sensor_nd *)element)->socked_id;
    copy->poll_fd = ((sensor_nd *)element)->poll_fd;
    return (void *)copy;
}

void sensor_free(void **element)
{
    free(*element);
    *element = NULL;
}

int sensor_compare(void *x, void *y)
{
    return ((((sensor_nd *)x)->sensor_id < ((sensor_nd *)y)->sensor_id) ? -1 : (((sensor_nd *)x)->sensor_id == ((sensor_nd *)y)->sensor_id) ? 0 : 1);
}

int get_poll_array(struct pollfd **ar, dplist_t *connections)  //updates array with all the element from the list
{
    free(*ar);
    (*ar) = malloc(sizeof(struct pollfd) * dpl_size(connections));
    for (int i = 0; i < dpl_size(connections); i++)
    {
        sensor_nd *dumy = dpl_get_element_at_index(connections, i);
        dumy->poll_fd.events = POLLIN;
        (*ar)[i] = dumy->poll_fd;
    }
    return -1;
}


void connmgr_listen(int port_number/*, sbuffer_t *sbuffer, FILE *fifo*/)
{
    struct pollfd *array = malloc(sizeof(struct pollfd));
    dplist_t *connections = dpl_create(sensor_copy, sensor_free, sensor_compare);

    tcpsock_t *server, *client;
    sensor_data_t data;
    int bytes, result, polltimeout;
    printf("Test server is started\n");
    if (tcp_passive_open(&server, port_number) != TCP_NO_ERROR)
        exit(EXIT_FAILURE);
    sensor_nd server_node;
    struct pollfd poll_fd_server;
    if (tcp_get_sd(server, &(poll_fd_server.fd)) != TCP_NO_ERROR)
    {
        printf("Server cannot be made");
    }
    server_node.poll_fd = poll_fd_server;
    server_node.socked_id = server;
    server_node.poll_fd.events = POLLIN;
    connections = dpl_insert_at_index(connections, &server_node, 0, true); //insert the server_node at index 0 in list
   

    while (1)
    {
        get_poll_array(&array, connections); //poll
        polltimeout = poll(array, dpl_size(connections), TIMEOUT * 1000); 

        if (dpl_size(connections) == 1 && polltimeout == 0)  //if only server node in list and time out -> break while loop
        {
            printf("Server timed out\n");
            connections = dpl_remove_at_index(connections, 0, true);
            free(array);
            printf("closing file\n");
            tcp_close(&server);
            connmgr_free(connections);
            break;
        }

        for (int i = 1; i < dpl_size(connections); i++)
        {
            sensor_nd *sensor = dpl_get_element_at_index(connections, i);
            tcpsock_t *socket = sensor->socked_id;
            if ((sensor->last_record + TIMEOUT) < time(NULL))  //check sensor time out
            {
                printf("Sensor timed out\n");
                tcp_close(&(socket));
                connections = dpl_remove_at_index(connections, i, true);
                array[i].revents = 0;
            }
            if (array[i].revents == POLLIN)  //if this sensor polled
            {
                sensor->last_record = time(NULL);
                // read sensor ID
                bytes = sizeof(data.id);
                result = tcp_receive(socket, (void *)&data.id, &bytes);
                // read temperature
                bytes = sizeof(data.value);
                result = tcp_receive(socket, (void *)&data.value, &bytes);
                // read timestamp
                bytes = sizeof(data.ts);
                result = tcp_receive(socket, (void *)&data.ts, &bytes);
                if ((result == TCP_NO_ERROR) && bytes)
                {
                    //sbuffer_insert(sbuffer, &data, 0); //insert into shared memory
                    printf("sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n", data.id, data.value, (long int)data.ts);
                }
                if (sensor->sensor_id == 0)
                {
                    sensor->sensor_id = data.id;
                    char *log;  //log
                    asprintf(&log, "A sensor node with id %d has opened a new connection", sensor->sensor_id);
                    //write_to_fifo(log, time(NULL), fifo);
                    free(log);
                }
                if (result == TCP_CONNECTION_CLOSED)
                {
                    printf("Peer has closed connection\n");
                    char *log;  //log
                    asprintf(&log, "A sensor node with id %d has closed the connection", data.id);
                    //write_to_fifo(log, time(NULL), fifo);
                    free(log);
                    tcp_close(&(socket));
                    connections = dpl_remove_at_index(connections, i, true);
                }
            }
        }
        if (array[0].revents == POLLIN)  //the server node got polled -> new connection incomming
        {
            if (tcp_wait_for_connection(server, &client) != TCP_NO_ERROR)
                exit(EXIT_FAILURE);
            printf("Incoming client connection\n");
            sensor_nd sensor_node;
            struct pollfd poll_fd_sensor;
            tcp_get_sd(client, &(poll_fd_sensor.fd));
            sensor_node.poll_fd = poll_fd_sensor;
            sensor_node.socked_id = client;
            sensor_node.poll_fd.events = POLLIN;
            sensor_node.last_record = time(NULL);
            int size = dpl_size(connections);
            connections = dpl_insert_at_index(connections, &sensor_node, size, true);
        }
    }
}

void connmgr_free(dplist_t *connections)
{
    dpl_free(&connections, true);
}