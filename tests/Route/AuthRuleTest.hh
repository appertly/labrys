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
use Aura\Router\Route;
use Psr\Http\Message\ServerRequestInterface as Request;

class RunnerTest
{
    <<Test>>
    public async function testBasic(Assert $assert): Awaitable<void>
    {
        $route = new Route();
        $route->auth(true);
        $object = new AuthRule();
        $uri = new \Zend\Diactoros\Uri('https://example.com/foo/bar');
        $request = new \Zend\Diactoros\ServerRequest([], [], $uri, 'GET');
        $assert->bool($object->__invoke($request, $route))->is(false);
    }

    <<Test>>
    public async function testBasic2(Assert $assert): Awaitable<void>
    {
        $route = new Route();
        $route->auth(false);
        $object = new AuthRule();
        $uri = new \Zend\Diactoros\Uri('https://example.com/foo/bar');
        $request = new \Zend\Diactoros\ServerRequest([], [], $uri, 'GET');
        $assert->bool($object->__invoke($request, $route))->is(true);
    }

    <<Test>>
    public async function testBasic3(Assert $assert): Awaitable<void>
    {
        $route = new Route();
        $route->auth(true);
        $object = new AuthRule();
        $uri = new \Zend\Diactoros\Uri('https://example.com/foo/bar');
        $request = new \Zend\Diactoros\ServerRequest([], [], $uri, 'GET');
        $request = $request->withAttribute('principal', \Caridea\Auth\Principal::get('foobar', []));
        $assert->bool($object->__invoke($request, $route))->is(true);
    }
}
