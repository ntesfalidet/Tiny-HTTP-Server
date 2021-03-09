/*
 * network_util.h
 *
 * Functions that implement network operations.
 *
 */

#ifndef NETWORK_UTIL_H_
#define NETWORK_UTIL_H_

#include <stdbool.h>
#include <limits.h>
#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256  // POSIX definition
#endif

// min and max IANA registered ports
#define MIN_REG_PORT 1024
#define MAX_REG_PORT 49151
#define MIN_PORT 0
#define MAX_PORT 65535

/**
 * Determines whether port is in range of
 * IANA registered port numbers.
 * MIN_PORT <= port < MIN_REG_PORT
 * See: https://en.wikipedia.org/wiki/Port_(computer_networking)
 * @param port the port number
 * @return true if port is well-known port, false otherwise.
 */
static bool isWellKnownPort(int port) {
    return (MIN_PORT <= port) && (port < MIN_REG_PORT);
}

/**
 * Determines whether port is in range of
 * IANA registered port numbers.
 * MIN_REG_PORT <= port <= MAX_REG_PORT
 * See: https://en.wikipedia.org/wiki/Port_(computer_networking)
 * @param port the port number
 * @return true if port is registered port, false otherwise.
 */
static bool isRegisteredPort(int port) {
    return (MIN_REG_PORT <= port) && (port <= MAX_REG_PORT);
}

/**
 * Determines whether port is in range of
 * private/dynamic port numbers.
 * MAX_REG_PORT < port <= MAX_PORT
 * See: https://en.wikipedia.org/wiki/Port_(computer_networking)
 * @param port the port number
 * @return true if port is dynamic port, false otherwise.
 */
static bool isDynamicPort(int port) {
    return (MAX_REG_PORT < port) && (port <= MAX_PORT);
}

/**
 * Connect to peer at host and port.
 *
 * @param host the host IP address as a string
 * @param port the host port
 * @return the peer socket or 0 if unavailable
 */
int get_peer_socket(const char *host, int port);

/**
 * Get listener socket
 *
 * @param port the port number
 * @return listener socket or 0 if unavailable
 */
int get_listener_socket(int port) ;

/**
 * Accept new peer connection on a listen socket.
 *
 * @param listen_sock_fd the listen socket
 * @return the peer socket fd
 */
int accept_peer_connection(int listen_sock_fd);

/**
 * Get the local host and port for a socket.
 *
 * @param sock_fd the socket
 * @param host buffer for host IP address string
 * @param port pointer for port value
 * @return 0 if successful
*/
int get_local_host_and_port(int sock_fd, char *addr_str, int *port);

/**
 * Get the peer host and port for a socket.
 *
 * @param sock_fd the socket
 * @param host buffer for host IP address string
 * @param port pointer for port value
 * @return 0 if successful
 */
int get_peer_host_and_port(int sock_fd, char *addr_str, int *port);

#endif /* NETWORK_UTIL_H_ */
