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

class ReporterTest
{
    <<Test>>
    public async function testRun1(Assert $assert): Awaitable<void>
    {
        $logger = M::mock(\Psr\Log\LoggerInterface::class);
        $logger->shouldReceive('log')->andReturnUsing(function ($a, $b, $c) use ($assert) {
            $assert->mixed($a)->identicalTo(\Psr\Log\LogLevel::ERROR);
            $assert->string($b)->is('foobar');
            $assert->mixed($c['exception'])->isTypeOf(\RuntimeException::class);
        });
        $errorLogger = new \Labrys\ErrorLogger($logger);
        $reporter = new Reporter($errorLogger);
        $next = function ($req, $res) use ($assert) {
            throw new \RuntimeException("foobar");
        };
        $request = new \Zend\Diactoros\ServerRequest();
        $response = new \Zend\Diactoros\Response();
        $assert->whenCalled(function () use ($reporter, $request, $response, $next){
            $reporter->__invoke($request, $response, $next);
        })->willThrowClass(\RuntimeException::class);
        M::close();
    }
}
