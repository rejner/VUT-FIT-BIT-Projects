""""
    IPP Project-1: HTTP resolver of domain names
    Faculty of Information Technology, Brno University of Technology (FIT VUT)
    Author: Michal Rein (xreinm00)
    e-mail: xreinm00@stud.fit.vutbr.cz

"""

import socket
from urllib import parse
import re
import sys
import time


# create socket for connection
def create_socket(argv):

    # global vars for host, port and socket object
    global host
    global port
    global s

    host = "localhost"
    if argv != '':
        port = int(argv[0])
    else:
        # default port
        port = 8080

    # create socket
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    except socket.error as msg:
        print("Socket error: " + str(msg))


# binding socket and waiting for incoming connection
def bind_socket():

    try:
        print("Binding the Port: " + str(port))
        s.bind((host, port))
        s.listen() # listening on port for incoming connection

    except socket.error as msg:
        print("Socket binding error: " + str(msg) + "\n" + "Retrying...")
        s.close()
        # bind_socket()


# establish connection with client (accepting socket)
def socket_accept():

    run = True
    while run:

        try:
            client_socket, address = s.accept()
            print("Connection has been established! | " + "IP " + address[0] + " | Port " + str(address[1]))

        except socket.error as msg:
            print("Server has experienced problems with client connection...")
            break

        # connection accepted, processing request
        run = process_request(client_socket)

        # after request is served, listen for another request
        client_socket.close()
        s.listen()

    s.close()


# processes HTTP request
def process_request(client_socket):

    # timeout for data receive
    timeout = 2
    client_request = ""

    client_socket.setblocking(0)

    # receive all data from client - controlled by timeout
    begin = time.time()
    while True:

        # if some data have been received and timeout reached, break
        if client_request and time.time()-begin > timeout:
            break

        # if no data have been received and 2*timeout was reached, break
        elif time.time()-begin > timeout*2:
            break

        # receive data chunk of 1024 bytes
        try:
            data = client_socket.recv(1024)
            if data:
                # decode and write data into client_request variable
                client_request += str(data, "utf-8")
                # reset timer
                begin = time.time()

        except socket.error:
            pass

    # print("Client's request: " + client_request)

    # test end case
    if client_request == "END":
        return False

    url = client_request.split(' ')[1]
    method = client_request.split(' ')[0]

    # testing which request is being served (we can process only GET and POST req.)
    if method == 'GET':
        do_GET(client_socket, url)

    elif method == 'POST':
        do_POST(client_socket, client_request)

    else:
        send_response(client_socket, "HTTP/1.1 405 Method Not Allowed\r\n\r\n")
        return True

    return True


# method for processing GET request
def do_GET(client_socket, url):

    # check url
    if not re.match(r'^/resolve\?.*$', url) or re.match(r'^/dns-query.*$', url):
        send_response(client_socket, "HTTP/1.1 400 Bad Request\r\n\r\n")
        print("Invalid url format!")
        return True

    o = parse.urlparse(url)
    parameters = o.query
    parsed_parameters = parse.parse_qs(parameters)

    # check parameter name
    if parsed_parameters.get('name'):
        t_name = str(parsed_parameters['name'])
        name = t_name.split('\'')[1]
    else:
        send_response(client_socket, "HTTP/1.1 400 Bad Request\r\n\r\n")
        print("Missing parameter: name!")
        return True

    # check parameter type
    if parsed_parameters.get('type'):
        t_type = str(parsed_parameters['type'])
        op_type = t_type.split('\'')[1]

        if op_type != 'A' and op_type != 'PTR':
            send_response(client_socket, "HTTP/1.1 400 Bad Request\r\n\r\n")
            print("Invalid type parameter!")
            return True

    else:
        send_response(client_socket, "HTTP/1.1 400 Bad Request\r\n\r\n")
        print("Missing parameter: type!")
        return True

    # get name from ip address
    if re.match(r'^[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+', name):
        try:
            # bad type
            if op_type == 'A':
                send_response(client_socket, "HTTP/1.1 400 Bad Request\r\n\r\n")

            else:
                host_name = socket.gethostbyaddr(name)
                send_response(client_socket, "HTTP/1.1 200 OK\r\n\r\n", "", name, op_type, host_name[0])

        except socket.error as msg:
            print("Couldn't find host!")
            send_response(client_socket, "HTTP/1.1 404 Not Found\r\n\r\n")

    # get ip address from name
    else:
        try:
            # bad type
            if op_type == 'PTR':
                send_response(client_socket, "HTTP/1.1 400 Bad Request\r\n\r\n")

            else:
                host_ip = socket.gethostbyname(name)
                send_response(client_socket, "HTTP/1.1 200 OK\r\n\r\n", "", name, op_type, host_ip)

        except socket.error as msg:
            print("Couldn't find host!")
            send_response(client_socket, "HTTP/1.1 404 Not Found\r\n\r\n")


# method for processing POST request
def do_POST(client_socket, data):

    parsing_header = True
    data_split = data.split('\n')
    response = ""

    num_found = 0 # number of found requests
    num_translated = 0 # number of successfully translated addresses

    # go through all lines, find header and start processing requests
    for line in data_split:

        # if header wasn't found yet or is being iterated, look further for requests
        if not parsing_header:
            if line != '':

                num_found += 1 # we found a valid request
                line = "".join(line.split()) # strip all tabulators, end of lines and white spaces
                address = line.split(':') # address[0] = name/ip-address, address[1] = type (A/PTR)

                # get name from ip address
                if re.match(r'^[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+', address[0]):
                    try:
                        # bad type
                        if address[1] != 'PTR':
                            continue

                        # try get name from ip address
                        host_name = socket.gethostbyaddr(address[0])
                        response += str(address[0]) + ":" + str(address[1]) + "=" + str(host_name[0]) + "\n"
                        num_translated += 1 # successfully translated

                    except socket.error as msg:
                        print("Couldn't find host!")

                # get ip address from name
                else:
                    try:
                        # bad type
                        if address[1] != 'A':
                            continue

                        # get ip from name
                        host_ip = socket.gethostbyname(address[0])
                        response += str(address[0]) + ":" + str(address[1]) + "=" + host_ip + "\n"
                        num_translated += 1

                    except socket.error as msg:
                        print("Couldn't find host!")

        # look further for query content
        else:
            if line == '\r':
                parsing_header = False

    # if at least some addresses were successfully translated, send final response with data
    if num_translated > 0:

        # structure HTML response
        final_response = ""
        final_response += "HTTP/1.1 200 OK\r\n\r\n"
        final_response += response

        send_response(client_socket, "", final_response, post_method=True)

    # there were no addresses to translate, or all of them failed in some way
    else:
        send_response(client_socket, "HTTP/1.1 404 Not Found\r\n\r\n")


# sends response to client
def send_response(client_socket, header, data="", name="default", op_type="default", ip="unknown", post_method=False):

    # sending response to GET request:
    if not post_method:

        # check if name translation was successful
        if re.match(r'.*200.*', header):
            data = str(header + str(name) + ":" + op_type + "=" + ip + "\n")

        # send only error code as header
        else:
            data = str(header)

    # send data (common for either GET or POST request)
    client_socket.send(str.encode(data))
    print("Data sent to client...")


# --------MAIN---------
def main(argv=''):
    create_socket(argv)
    bind_socket()
    socket_accept()


if __name__ == "__main__":

    if len(sys.argv) > 1:
        main(sys.argv[1:])
    else:
        main()


