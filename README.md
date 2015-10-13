# What is this?

``Websybil`` is a proof of concept in order to show how ``user-agent`` faking is useless when you need
to hide this piece of information from smarter people.

In fact, the idea behind this application is pretty straightforward. I had decided express it in ``C``
because this Programming Language for Computing is as Math Notation for Mathematics. Anyone really
interested about real Computer Programming can understand it in a few seconds and then express and
adapt this idea for other Programming Languages.

# How to clone this repo?

``git clone https://github.com/rafael-santiago/websybil websybil``

``cd websybil``

``git submodule update --init``

# How to build it?

``Websybil`` uses ``Hefesto`` for building issues. So you need to install [Hefesto](https://github.com/rafael-santiago/hefesto).

With ``Hefesto`` working on your system inside ``Websybil`` src directory just type:

``hefesto``

An ``ELF`` under src/bin will be created. Congrats, now you have "websybil" ready for browser predictions on your system.

# How to use this?

There are three basic operation modes:

1. User buffer parsing.

2. Network sniffing.

3. Pipe.

## Buffer parsing

``./websybil --request-buffer="<http-request-string>"``

## Network sniffing

``./websybil --from-wire=eth0``

Of course it requires ``CAP_SYS_ADMIN`` privilegies.

## Pipe

Just put the ``websybil`` at the end of your pipe chain.

``(...) | ./websybil``

Send a ``SIGINT`` to stop the application (``CTRL+c``).
