vtrick
======

*vtrick* is a program to trick behavior of Viessmann central heating
kettles.

Requirements
------------

Viessmann kettles show sometimes a specific misbehavior. The kettle
while starting its burner in central heating mode switches it off after
couple of seconds. Burner starts are repeated several times with period
of seconds with the same effect. This leads to an excessive burner and
fuel usage. This is observed when the system reaches near-target
temperature. The kettle then detects heat consumption is too slow by
slow declining return water temperature. This is most likely interpreted
as chocked circuit and the burner gets switched off to avoid damage.

To circumvent this misbehavior it is enough to set the system target
temperature to some high value for a couple of seconds. This is what
*vtrick* is doing, when it detects burner start in central heating mode.

The computer is communicating with the kettle via Optolink Interface.
Device example can be seen
[here](https://github.com/openv/openv/wiki/Bauanleitung-USB) .

Usage
-----

*vtrick* is started with an argument indicating Optolink Interface
serial port.

    $ ./vtrick /dev/ttyUSB0

Build dependencies
------------------

-   make

-   gtest v1.10.0 -- for unit tests

-   lcov -- for unit test coverage report

Known issues
------------

Initialization of the serial port is not always working properly. This
will be solved with coming commits. In the meantime the serial port can
be bootstrapped with the below credited software.

Credits
-------

Credit goes to Peter Schulze, author of the *v-control -- 1.2.5*
software. I used it in debug mode to check parameter codes my kettle is
using. If you want to add support for new kettles, it'd advise to use
Peter's program.

There is also lot of information on the subject on the
[openv](https://github.com/openv/openv) wiki.

End note
--------

File `doc/readme.html` is automatically converted to `README.md` with
git commit using *pandoc* tool.
