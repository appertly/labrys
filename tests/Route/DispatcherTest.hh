<?hh
/**
 * Labrys
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 *
 * @copyright 2015-2016 Appertly
 * @license   Apache-2.0
 */
namespace Labrys\Route;

use HackPack\HackUnit\Contract\Assert;
use Aura\Router\Matcher;
use Aura\Router\Route;
use Aura\Router\Rule\Accepts;
use Aura\Router\Rule\Allows;
use Aura\Router\Rule\Path;
use Aura\Router\Rule\RuleIterator;
use Caridea\Container\Builder;
use Psr\Http\Message\ServerRequestInterface as Request;
use Psr\Http\Message\ResponseInterface as Response;
use Psr\Log\NullLogger;

class DispatcherTest
{
    public function __construct(private string $header = 'foobar')
    {
    }

    public function __invoke(Request $request, Response $response): Response
    {
        return $response->withHeader('X-Unit-Test', $this->header);
    }

    <<Test>>
    public async function testUnroutable(Assert $assert): Awaitable<void>
    {
        $routeRules = new RuleIterator([new Path(), new Allows(), new Accepts()]);
        $map = new \Aura\Router\Map(new Route());
        $map->get('only.get', '/foo/bar', [self::class, 'postMethod']);
        $matcher = new Matcher($map, new NullLogger(), $routeRules);

        $builder = new Builder();
        $container = $builder->build(null);

        $object = new Dispatcher($matcher, $container);

        $uri = new \Zend\Diactoros\Uri('https://example.com/foo/bar');
        $request = new \Zend\Diactoros\ServerRequest([], [], $uri, 'POST');
        $response = new \Zend\Diactoros\Response();

        $assert->whenCalled(function () use ($object, $request, $response) {
            $object->__invoke($request, $response, ($req, $res) ==> $res);
        })->willThrowClass(Exception\Unroutable::class);
    }

    <<Test>>
    public async function testNormal(Assert $assert): Awaitable<void>
    {
        $routeRules = new RuleIterator([new Path(), new Allows(), new Accepts()]);
        $map = new \Aura\Router\Map(new Route());
        $map->get('only.get', '/foo/bar', function ($req, $res) {
            return $res->withHeader('X-Unit-Test', 'foo');
        });
        $matcher = new Matcher($map, new NullLogger(), $routeRules);

        $builder = new Builder();
        $container = $builder->build(null);

        $object = new Dispatcher($matcher, $container);

        $uri = new \Zend\Diactoros\Uri('https://example.com/foo/bar');
        $request = new \Zend\Diactoros\ServerRequest([], [], $uri, 'GET');
        $response = new \Zend\Diactoros\Response();

        $res = $object->__invoke($request, $response, ($req, $res) ==> $res);
        $assert->string($res->getHeaderLine('X-Unit-Test'))->is('foo');
    }

    <<Test>>
    public async function testContainer1(Assert $assert): Awaitable<void>
    {
        $routeRules = new RuleIterator([new Path(), new Allows(), new Accepts()]);
        $map = new \Aura\Router\Map(new Route());
        $map->get('only.get', '/foo/bar', [self::class, '__invoke']);
        $matcher = new Matcher($map, new NullLogger(), $routeRules);

        $builder = new Builder();
        $builder->lazy('dispatcherController', self::class, function ($c) {
            return new DispatcherTest('herpderp');
        });
        $container = $builder->build(null);

        $object = new Dispatcher($matcher, $container);

        $uri = new \Zend\Diactoros\Uri('https://example.com/foo/bar');
        $request = new \Zend\Diactoros\ServerRequest([], [], $uri, 'GET');
        $response = new \Zend\Diactoros\Response();

        $res = $object->__invoke($request, $response, ($req, $res) ==> $res);
        $assert->string($res->getHeaderLine('X-Unit-Test'))->is('herpderp');
    }

    <<Test>>
    public async function testContainer2(Assert $assert): Awaitable<void>
    {
        $routeRules = new RuleIterator([new Path(), new Allows(), new Accepts()]);
        $map = new \Aura\Router\Map(new Route());
        $map->get('only.get', '/foo/bar', 'dispatcherController');
        $matcher = new Matcher($map, new NullLogger(), $routeRules);

        $builder = new Builder();
        $builder->lazy('dispatcherController', self::class, function ($c) {
            return new DispatcherTest();
        });
        $container = $builder->build(null);

        $object = new Dispatcher($matcher, $container);

        $uri = new \Zend\Diactoros\Uri('https://example.com/foo/bar');
        $request = new \Zend\Diactoros\ServerRequest([], [], $uri, 'GET');
        $response = new \Zend\Diactoros\Response();

        $res = $object->__invoke($request, $response, ($req, $res) ==> $res);
        $assert->string($res->getHeaderLine('X-Unit-Test'))->is('foobar');
    }

    <<Test>>
    public async function testContainer3(Assert $assert): Awaitable<void>
    {
        $routeRules = new RuleIterator([new Path(), new Allows(), new Accepts()]);
        $map = new \Aura\Router\Map(new Route());
        $map->get('only.get', '/foo/bar', 'dispatcherController');
        $matcher = new Matcher($map, new NullLogger(), $routeRules);

        $builder = new Builder();
        $container = $builder->build(null);

        $object = new Dispatcher($matcher, $container);

        $uri = new \Zend\Diactoros\Uri('https://example.com/foo/bar');
        $request = new \Zend\Diactoros\ServerRequest([], [], $uri, 'GET');
        $response = new \Zend\Diactoros\Response();

        $assert->whenCalled(function () use ($object, $request, $response) {
            $object->__invoke($request, $response, ($req, $res) ==> $res);
        })->willThrowClassWithMessage(
            Exception\Uncallable::class,
            "The container has no object with the name 'dispatcherController'"
        );
    }

    <<Test>>
    public async function testContainer4(Assert $assert): Awaitable<void>
    {
        $routeRules = new RuleIterator([new Path(), new Allows(), new Accepts()]);
        $map = new \Aura\Router\Map(new Route());
        $map->get('only.get', '/foo/bar', 'dispatcherController');
        $matcher = new Matcher($map, new NullLogger(), $routeRules);

        $builder = new Builder();
        $builder->lazy('dispatcherController', \SplObjectStorage::class, function ($c) {
            return new \SplObjectStorage();
        });
        $container = $builder->build(null);

        $object = new Dispatcher($matcher, $container);

        $uri = new \Zend\Diactoros\Uri('https://example.com/foo/bar');
        $request = new \Zend\Diactoros\ServerRequest([], [], $uri, 'GET');
        $response = new \Zend\Diactoros\Response();

        $assert->whenCalled(function () use ($object, $request, $response) {
            $object->__invoke($request, $response, ($req, $res) ==> $res);
        })->willThrowClassWithMessage(
            Exception\Uncallable::class,
            "The object 'dispatcherController' cannot be invoked as a function"
        );
    }

    <<Test>>
    public async function testContainer5(Assert $assert): Awaitable<void>
    {
        $routeRules = new RuleIterator([new Path(), new Allows(), new Accepts()]);
        $map = new \Aura\Router\Map(new Route());
        $map->get('only.get', '/foo/bar', [self::class, 'foobar']);
        $matcher = new Matcher($map, new NullLogger(), $routeRules);

        $builder = new Builder();
        $builder->lazy('dispatcherController', self::class, function ($c) {
            return new DispatcherTest();
        });
        $container = $builder->build(null);

        $object = new Dispatcher($matcher, $container);

        $uri = new \Zend\Diactoros\Uri('https://example.com/foo/bar');
        $request = new \Zend\Diactoros\ServerRequest([], [], $uri, 'GET');
        $response = new \Zend\Diactoros\Response();

        $assert->whenCalled(function () use ($object, $request, $response) {
            $object->__invoke($request, $response, ($req, $res) ==> $res);
        })->willThrowClassWithMessage(
            Exception\Uncallable::class,
            "Controller class '" . __CLASS__ . "' doesn't have method 'foobar'"
        );
    }

    <<Test>>
    public async function testContainer6(Assert $assert): Awaitable<void>
    {
        $routeRules = new RuleIterator([new Path(), new Allows(), new Accepts()]);
        $map = new \Aura\Router\Map(new Route());
        $map->get('only.get', '/foo/bar', [self::class, '__invoke']);
        $matcher = new Matcher($map, new NullLogger(), $routeRules);

        $builder = new Builder();
        $container = $builder->build(null);

        $object = new Dispatcher($matcher, $container);

        $uri = new \Zend\Diactoros\Uri('https://example.com/foo/bar');
        $request = new \Zend\Diactoros\ServerRequest([], [], $uri, 'GET');
        $response = new \Zend\Diactoros\Response();

        $assert->whenCalled(function () use ($object, $request, $response) {
            $object->__invoke($request, $response, ($req, $res) ==> $res);
        })->willThrowClassWithMessage(
            Exception\Uncallable::class,
            "Controller instance not found: '" . __CLASS__ . "'"
        );
    }

    <<Test>>
    public async function testContainer7(Assert $assert): Awaitable<void>
    {
        $routeRules = new RuleIterator([new Path(), new Allows(), new Accepts()]);
        $map = new \Aura\Router\Map(new Route());
        $map->get('only.get', '/foo/bar', [1, 2, 3]);
        $matcher = new Matcher($map, new NullLogger(), $routeRules);

        $builder = new Builder();
        $container = $builder->build(null);

        $object = new Dispatcher($matcher, $container);

        $uri = new \Zend\Diactoros\Uri('https://example.com/foo/bar');
        $request = new \Zend\Diactoros\ServerRequest([], [], $uri, 'GET');
        $response = new \Zend\Diactoros\Response();

        $assert->whenCalled(function () use ($object, $request, $response) {
            $object->__invoke($request, $response, ($req, $res) ==> $res);
        })->willThrowClassWithMessageContaining(
            Exception\Uncallable::class,
            "Could not invoke the handler: "
        );
    }
}
