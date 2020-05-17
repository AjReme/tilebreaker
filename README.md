# TileBreaker

A console competitive game based on [tileman.io](http://tileman.io/).

## Building

Simply run the following script:

```
$ meson build
$ ninja -C build
$ ninja -C build install
```

After that, in folder `bin` you'll find game launcher called TileBreaker.

## Using the launcher

* For help, type
`$ ./TileBreaker help`

* For creating server, type
`$ ./TileBreaker create [Port] [Players number]`.
For example, command
`$ ./TileBreaker create 53645 6`.
Will create game in port 53645 and for 6 players.

* For connecting, type
`$ ./TileBreaker create [IP] [Port]`.
Example:
`$ ./TileBreaker create localhost 53645`.