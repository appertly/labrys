# Contributing

A few notes about development for this project. 

## Compliance

Our code is intended to comply with the formatting standards [PSR-1](http://www.php-fig.org/psr/psr-1/), [PSR-2](http://www.php-fig.org/psr/psr-2/).
Hack syntax will naturally differ from PHP a little bit, but keep at least these two standards in mind.

Classes must conform to the [PSR-4](http://www.php-fig.org/psr/psr-4/) autoloading standard.

## Debug

Make sure your HHVM `php.ini` file contains the following to enable Xdebug support. This will also enable code coverage under unit tests.

```
xdebug.enable=1
xdebug.remote_enable=1
xdebug.remote_host="localhost"
xdebug.remote_port=9089
```
