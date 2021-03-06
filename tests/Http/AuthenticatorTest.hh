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

class AuthenticatorTest
{
    <<Test>>
    public async function testRun1(Assert $assert): Awaitable<void>
    {
        $session = M::mock(\Caridea\Session\Session::class);
        $session->shouldReceive('getValues')->andReturn(new \Caridea\Session\NullMap());
        $service = new \Caridea\Auth\Service($session);
        $object = new Authenticator($service, '/auth/login');
        $next = function ($req, $res) use ($assert) {
            $assert->mixed($req->getAttribute('principal'))->identicalTo(\Caridea\Auth\Principal::getAnonymous());
            return $res;
        };
        $request = new \Zend\Diactoros\ServerRequest();
        $response = new \Zend\Diactoros\Response();
        $out = $object->__invoke($request, $response, $next);
        $assert->mixed($out)->identicalTo($response);
        M::close();
    }

    <<Test>>
    public async function testRun2(Assert $assert): Awaitable<void>
    {
        $session = M::mock(\Caridea\Session\Session::class);
        $session->shouldReceive('canResume')->andReturn(true);
        $session->shouldReceive('getValues')->andReturn(new \Caridea\Session\NullMap());
        $service = new \Caridea\Auth\Service($session);
        $object = new Authenticator($service, '/auth/login');
        $next = function ($req, $res) use ($assert) {
            throw new \Labrys\Route\Exception\Unroutable("Something happened", 403);
        };
        $request = new \Zend\Diactoros\ServerRequest();
        $request = $request->withRequestTarget("/my/place");
        $response = new \Zend\Diactoros\Response();
        $out = $object->__invoke($request, $response, $next);
        $assert->string($out->getHeaderLine('Location'))->is('/auth/login?then=/my/place');
        $assert->int($out->getStatusCode())->eq(303);
        M::close();
    }
}
