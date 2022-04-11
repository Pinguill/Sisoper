import socket
import sys
from tracemalloc import stop


def get_constants(prefix):
    """Create a dictionary mapping socket module
    constants to their names.
    """
    return {
        getattr(socket, n): n
        for n in dir(socket)
        if n.startswith(prefix)
    }



def createContainer(name):
    families = get_constants('AF_')
    types = get_constants('SOCK_')
    protocols = get_constants('IPPROTO_')
    
    # Create a TCP/IP socket
    sock = socket.create_connection(('localhost', 7070))
    
    print('Family  :', families[sock.family])
    print('Type    :', types[sock.type])
    print('Protocol:', protocols[sock.proto])
    print()

    try:

        # Send data
        message = "1 " + name;  
        print('sending {!r}'.format(message))
        sock.sendall(message.encode())

        amount_received = 0
        amount_expected = len(message)

        while amount_received < amount_expected:
            data = sock.recv(16)
            amount_received += len(data)
            print('received {!r}'.format(data))

    finally:
        print('closing socket')
        sock.close()


def stopContainer(name):
    families = get_constants('AF_')
    types = get_constants('SOCK_')
    protocols = get_constants('IPPROTO_')
    
    # Create a TCP/IP socket
    sock = socket.create_connection(('localhost', 7070))
    
    print('Family  :', families[sock.family])
    print('Type    :', types[sock.type])
    print('Protocol:', protocols[sock.proto])
    print()

    try:

        # Send data
        message = "2 " + name;  
        print('sending {!r}'.format(message))
        sock.sendall(message.encode())

        amount_received = 0
        amount_expected = len(message)

        while amount_received < amount_expected:
            data = sock.recv(16)
            amount_received += len(data)
            print('received {!r}'.format(data))

    finally:
        print('closing socket')
        sock.close()

def removeContainer(name):
    families = get_constants('AF_')
    types = get_constants('SOCK_')
    protocols = get_constants('IPPROTO_')
    
    # Create a TCP/IP socket
    sock = socket.create_connection(('localhost', 7070))
    
    print('Family  :', families[sock.family])
    print('Type    :', types[sock.type])
    print('Protocol:', protocols[sock.proto])
    print()

    try:

        # Send data
        message = "3 " + name;  
        print('sending {!r}'.format(message))
        sock.sendall(message.encode())

        amount_received = 0
        amount_expected = len(message)

        while amount_received < amount_expected:
            data = sock.recv(16)
            amount_received += len(data)
            print('received {!r}'.format(data))

    finally:
        print('closing socket')
        sock.close()

#createContainer("hola1")
#createContainer("hola2")
#createContainer("hola3")

stopContainer("hola1")
stopContainer("hola2")
stopContainer("hola3")