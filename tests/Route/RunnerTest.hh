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
use Caridea\Container\Builder;
use Psr\Http\Message\ServerRequestInterface as Request;
use Psr\Http\Message\ResponseInterface as Response;

class RunnerTest implements Plugin
{
    public function __construct(private int $priority = 0)
    {
    }

    public function getPriority(): int
    {
        return $this->priority;
    }

    public function __invoke(Request $request, Response $response, (function (Request,Response): Response) $next): Response
    {
        return $response->withHeader('X-Priority', (string)$this->priority);
    }

    <<Test>>
    public async function testIntegration(Assert $assert): Awaitable<void>
    {
        $builder = new Builder();
        $builder->lazy('plugin1', self::class, function ($c) {
            return new self(300);
        })->lazy('plugin2', self::class, function ($c) {
            return new self(500);
        });
        $container = $builder->build(null);
        $runner = new Runner($container);

        $uri = new \Zend\Diactoros\Uri('https://example.com/foo/bar');
        $request = new \Zend\Diactoros\ServerRequest([], [], $uri, 'POST');
        $response = new \Zend\Diactoros\Response();
        $out = $runner->run($request, $response);
        $assert->string($out->getHeaderLine('X-Priority'))->is('500');
    }
}
