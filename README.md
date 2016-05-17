# labrys

This is a library that helps you write web applications in Hack.

## Installation

You can install this library using Composer:

```console
$ composer require appertly/labrys
```

* The master branch (version 0.x) of this project requires HHVM 3.12 and has a few dependencies.

## Compliance

Releases of this library will conform to [Semantic Versioning](http://semver.org).

Our code is intended to comply with [PSR-1](http://www.php-fig.org/psr/psr-1/), [PSR-2](http://www.php-fig.org/psr/psr-2/), and [PSR-4](http://www.php-fig.org/psr/psr-4/). If you find any issues related to standards compliance, please send a pull request!

## The Big Idea

Really, Labrys is the glue between several micro libraries.

In addition to several helper classes, the Big Deal here is a mechanism to declare modules.

The `Labrys\System` class has three dependency containers: one for configuration properties, one for *back-end* objects and one for *front-end* objects. Modules can register objects in these containers.

More details coming soon! In the meantime, please browse the code or check out the [API documentation](https://appertly.github.io/labrys/api/).
