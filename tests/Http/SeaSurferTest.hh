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
namespace Labrys\Http;

use HackPack\HackUnit\Contract\Assert;
use Mockery as M;

class SeaSurferTest
{
    <<Test>>
    public async function testRun0(Assert $assert): Awaitable<void>
    {
        $plugin = M::mock(\Caridea\Session\CsrfPlugin::class);
        $plugin->shouldNotReceive('isValid');
        $errorLogger = M::mock(\Labrys\ErrorLogger::class);
        $errorLogger->shouldNotReceive('log');
        $object = new SeaSurfer($plugin, $errorLogger);
        $next = function ($req, $res) use ($assert) {
            return $res;
        };
        $request = new \Zend\Diactoros\ServerRequest();
        $request = $request->withMethod('GET');
        $response = new \Zend\Diactoros\Response();
        $out = $object->__invoke($request, $response, $next);
        $assert->mixed($out)->identicalTo($response);
        M::close();
    }

    <<Test>>
    public async function testRun1(Assert $assert): Awaitable<void>
    {
        $token = 'foobarbazbiz';
        $plugin = M::mock(\Caridea\Session\CsrfPlugin::class);
        $plugin->shouldReceive('isValid')->withArgs([$token])->andReturn(false);
        $errorLogger = M::mock(\Labrys\ErrorLogger::class);
        $errorLogger->shouldReceive('log');
        $object = new SeaSurfer($plugin, $errorLogger, 'csrfToken', 'example.com');
        $next = function ($req, $res) use ($assert) {
            return $res;
        };
        $request = new \Zend\Diactoros\ServerRequest();
        $request = $request->withMethod('POST')
            ->withAttribute('principal', \Caridea\Auth\Principal::get('foobar', []))
            ->withHeader('Referer', 'https://example.com/test')
            ->withParsedBody(['csrfToken' => $token]);
        $response = new \Zend\Diactoros\Response();
        $out = $object->__invoke($request, $response, $next);
        $assert->mixed($out)->isTypeOf(\Zend\Diactoros\Response::class);
        $assert->int($out->getStatusCode())->eq(440);
        $assert->string($out->getReasonPhrase())->is('Login Timeout');
        M::close();
    }

    <<Test>>
    public async function testRun10(Assert $assert): Awaitable<void>
    {
        $token = 'foobarbazbiz';
        $plugin = M::mock(\Caridea\Session\CsrfPlugin::class);
        $plugin->shouldNotReceive('isValid');
        $errorLogger = M::mock(\Labrys\ErrorLogger::class);
        $errorLogger->shouldNotReceive('log');
        $object = new SeaSurfer($plugin, $errorLogger, 'csrfToken', 'example.com');
        $next = function ($req, $res) use ($assert) {
            return $res;
        };
        $request = new \Zend\Diactoros\ServerRequest();
        $request = $request->withMethod('POST')
            ->withAttribute('principal', \Caridea\Auth\Principal::get('foobar', []))
            ->withHeader('Referer', 'https://example.com/test')
            ->withHeader('X-Requested-With', 'XMLHttpRequest');
        $response = new \Zend\Diactoros\Response();
        $out = $object->__invoke($request, $response, $next);
        $assert->mixed($out)->isTypeOf(\Zend\Diactoros\Response::class);
        $assert->int($out->getStatusCode())->eq(200);
        $assert->string($out->getReasonPhrase())->is('OK');
        M::close();
    }

    <<Test>>
    public async function testRun11(Assert $assert): Awaitable<void>
    {
        $token = 'foobarbazbiz';
        $plugin = M::mock(\Caridea\Session\CsrfPlugin::class);
        $plugin->shouldNotReceive('isValid');
        $errorLogger = M::mock(\Labrys\ErrorLogger::class);
        $errorLogger->shouldNotReceive('log');
        $object = new SeaSurfer($plugin, $errorLogger, 'csrfToken', 'example.com');
        $next = function ($req, $res) use ($assert) {
            return $res;
        };
        $request = new \Zend\Diactoros\ServerRequest();
        $request = $request->withMethod('POST')
            ->withAttribute('principal', \Caridea\Auth\Principal::getAnonymous())
            ->withHeader('Referer', 'https://example.com/test');
        $response = new \Zend\Diactoros\Response();
        $out = $object->__invoke($request, $response, $next);
        $assert->mixed($out)->isTypeOf(\Zend\Diactoros\Response::class);
        $assert->int($out->getStatusCode())->eq(200);
        $assert->string($out->getReasonPhrase())->is('OK');
        M::close();
    }

    <<Test>>
    public async function testRun2(Assert $assert): Awaitable<void>
    {
        $token = 'foobarbazbiz';
        $plugin = M::mock(\Caridea\Session\CsrfPlugin::class);
        $plugin->shouldReceive('isValid')->withArgs([$token])->andReturn(false);
        $errorLogger = M::mock(\Labrys\ErrorLogger::class);
        $errorLogger->shouldReceive('log');
        $object = new SeaSurfer($plugin, $errorLogger, 'csrfToken', 'example.com');
        $next = function ($req, $res) use ($assert) {
            return $res;
        };
        $request = new \Zend\Diactoros\ServerRequest();
        $request = $request->withMethod('POST')
            ->withAttribute('principal', \Caridea\Auth\Principal::get('foobar', []))
            ->withHeader('Referer', 'https://example.com/test')
            ->withParsedBody([]);
        $response = new \Zend\Diactoros\Response();
        $out = $object->__invoke($request, $response, $next);
        $assert->mixed($out)->isTypeOf(\Zend\Diactoros\Response::class);
        $assert->int($out->getStatusCode())->eq(440);
        $assert->string($out->getReasonPhrase())->is('Login Timeout');
        M::close();
    }

    <<Test>>
    public async function testRun3(Assert $assert): Awaitable<void>
    {
        $token = 'foobarbazbiz';
        $plugin = M::mock(\Caridea\Session\CsrfPlugin::class);
        $errorLogger = M::mock(\Labrys\ErrorLogger::class);
        $errorLogger->shouldReceive('log');
        $object = new SeaSurfer($plugin, $errorLogger, 'csrfToken', 'example.com');
        $next = function ($req, $res) use ($assert) {
            return $res;
        };
        $request = new \Zend\Diactoros\ServerRequest();
        $request = $request->withMethod('PUT')
            ->withAttribute('principal', \Caridea\Auth\Principal::get('foobar', []))
            ->withHeader('Referer', 'https://example.net/test');
        $response = new \Zend\Diactoros\Response();
        $out = $object->__invoke($request, $response, $next);
        $assert->mixed($out)->isTypeOf(\Zend\Diactoros\Response::class);
        $assert->int($out->getStatusCode())->eq(440);
        $assert->string($out->getReasonPhrase())->is('Login Timeout');
        M::close();
    }

    <<Test>>
    public async function testRun7(Assert $assert): Awaitable<void>
    {
        $token = 'foobarbazbiz';
        $plugin = M::mock(\Caridea\Session\CsrfPlugin::class);
        $errorLogger = M::mock(\Labrys\ErrorLogger::class);
        $errorLogger->shouldReceive('log');
        $object = new SeaSurfer($plugin, $errorLogger);
        $next = function ($req, $res) use ($assert) {
            return $res;
        };
        $request = new \Zend\Diactoros\ServerRequest();
        $request = $request->withMethod('PUT')
            ->withAttribute('principal', \Caridea\Auth\Principal::get('foobar', []))
            ->withHeader('Host', 'example.com')
            ->withHeader('Referer', 'https://example.net/test');
        $response = new \Zend\Diactoros\Response();
        $out = $object->__invoke($request, $response, $next);
        $assert->mixed($out)->isTypeOf(\Zend\Diactoros\Response::class);
        $assert->int($out->getStatusCode())->eq(440);
        $assert->string($out->getReasonPhrase())->is('Login Timeout');
        M::close();
    }

    <<Test>>
    public async function testRun8(Assert $assert): Awaitable<void>
    {
        $token = 'foobarbazbiz';
        $plugin = M::mock(\Caridea\Session\CsrfPlugin::class);
        $errorLogger = M::mock(\Labrys\ErrorLogger::class);
        $errorLogger->shouldReceive('log');
        $object = new SeaSurfer($plugin, $errorLogger);
        $next = function ($req, $res) use ($assert) {
            return $res;
        };
        $request = new \Zend\Diactoros\ServerRequest();
        $request = $request->withMethod('PUT')
            ->withAttribute('principal', \Caridea\Auth\Principal::get('foobar', []))
            ->withHeader('Host', 'example.org')
            ->withHeader('X-Forwarded-Host', 'example.com')
            ->withHeader('Referer', 'https://example.net/test');
        $response = new \Zend\Diactoros\Response();
        $out = $object->__invoke($request, $response, $next);
        $assert->mixed($out)->isTypeOf(\Zend\Diactoros\Response::class);
        $assert->int($out->getStatusCode())->eq(440);
        $assert->string($out->getReasonPhrase())->is('Login Timeout');
        M::close();
    }

    <<Test>>
    public async function testRun9(Assert $assert): Awaitable<void>
    {
        $token = 'foobarbazbiz';
        $plugin = M::mock(\Caridea\Session\CsrfPlugin::class);
        $plugin->shouldReceive('isValid')->withArgs([$token])->andReturn(true);
        $errorLogger = M::mock(\Labrys\ErrorLogger::class);
        $errorLogger->shouldReceive('log');
        $object = new SeaSurfer($plugin, $errorLogger);
        $next = function ($req, $res) use ($assert) {
            return $res;
        };
        $request = new \Zend\Diactoros\ServerRequest();
        $request = $request->withMethod('PUT')
            ->withAttribute('principal', \Caridea\Auth\Principal::get('foobar', []))
            ->withHeader('Host', 'example.org')
            ->withHeader('X-Forwarded-Host', 'example.com')
            ->withHeader('Referer', 'https://example.com/test')
            ->withParsedBody(['csrfToken' => $token]);
        $response = new \Zend\Diactoros\Response();
        $out = $object->__invoke($request, $response, $next);
        $assert->mixed($out)->isTypeOf(\Zend\Diactoros\Response::class);
        $assert->int($out->getStatusCode())->eq(200);
        $assert->string($out->getReasonPhrase())->is('OK');
        M::close();
    }

    <<Test>>
    public async function testRun4(Assert $assert): Awaitable<void>
    {
        $token = 'foobarbazbiz';
        $plugin = M::mock(\Caridea\Session\CsrfPlugin::class);
        $errorLogger = M::mock(\Labrys\ErrorLogger::class);
        $errorLogger->shouldReceive('log');
        $object = new SeaSurfer($plugin, $errorLogger, 'csrfToken', 'example.com');
        $next = function ($req, $res) use ($assert) {
            return $res;
        };
        $request = new \Zend\Diactoros\ServerRequest();
        $request = $request->withMethod('DELETE')
            ->withAttribute('principal', \Caridea\Auth\Principal::get('foobar', []));
        $response = new \Zend\Diactoros\Response();
        $out = $object->__invoke($request, $response, $next);
        $assert->mixed($out)->isTypeOf(\Zend\Diactoros\Response::class);
        $assert->int($out->getStatusCode())->eq(449);
        $assert->string($out->getReasonPhrase())->is('Retry With');
        M::close();
    }

    <<Test>>
    public async function testRun5(Assert $assert): Awaitable<void>
    {
        $token = 'foobarbazbiz';
        $plugin = M::mock(\Caridea\Session\CsrfPlugin::class);
        $plugin->shouldReceive('isValid')->withArgs([$token])->andReturn(true);
        $errorLogger = M::mock(\Labrys\ErrorLogger::class);
        $errorLogger->shouldNotReceive('log');
        $object = new SeaSurfer($plugin, $errorLogger, 'csrfToken', 'example.com', false);
        $next = function ($req, $res) use ($assert) {
            return $res;
        };
        $request = new \Zend\Diactoros\ServerRequest();
        $request = $request->withMethod('DELETE')
            ->withAttribute('principal', \Caridea\Auth\Principal::get('foobar', []))
            ->withParsedBody(['csrfToken' => $token]);
        $response = new \Zend\Diactoros\Response();
        $out = $object->__invoke($request, $response, $next);
        $assert->mixed($out)->isTypeOf(\Zend\Diactoros\Response::class);
        $assert->int($out->getStatusCode())->eq(200);
        $assert->string($out->getReasonPhrase())->is('OK');
        M::close();
    }

    <<Test>>
    public async function testRun6(Assert $assert): Awaitable<void>
    {
        $token = 'foobarbazbiz';
        $plugin = M::mock(\Caridea\Session\CsrfPlugin::class);
        $plugin->shouldReceive('isValid')->withArgs([$token])->andReturn(true);
        $errorLogger = M::mock(\Labrys\ErrorLogger::class);
        $errorLogger->shouldNotReceive('log');
        $object = new SeaSurfer($plugin, $errorLogger, 'csrfToken', 'example.com');
        $next = function ($req, $res) use ($assert) {
            return $res;
        };
        $request = new \Zend\Diactoros\ServerRequest();
        $request = $request->withMethod('POST')
            ->withAttribute('principal', \Caridea\Auth\Principal::get('foobar', []))
            ->withHeader('Referer', 'https://example.com/test')
            ->withParsedBody(['csrfToken' => $token]);
        $response = new \Zend\Diactoros\Response();
        $out = $object->__invoke($request, $response, $next);
        $assert->mixed($out)->isTypeOf(\Zend\Diactoros\Response::class);
        $assert->int($out->getStatusCode())->eq(200);
        $assert->string($out->getReasonPhrase())->is('OK');
        M::close();
    }
}
