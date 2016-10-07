## max-auth-attempts

Test how many authentication attempts are possible.
All options can set in /etc/honeyssh/config.json.
The passwords are random values because the only function of this programme
is to test how many looking attempts are possible

## Build

needs:
libssh2
jansson

build with:
make
make install
