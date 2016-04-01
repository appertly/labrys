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
 * @license   http://opensource.org/licenses/Apache-2.0 Apache 2.0 License
 */
namespace Labrys\Web;

use Psr\Http\Message\ServerRequestInterface as Request;
use Psr\Http\Message\ResponseInterface as Response;

/**
 * Web Front Controller
 */
class Front
{
    /**
     * Creates a new Front Controller
     *
     * @param $matcher - The route matcher
     * @param $emitter - The response emitter
     * @param $container - The dependency injection container
     * @param $errorLogger - The error logger
     * @param $contingency - The error handler
     * @param $authService - The authentication service
     */
    public function __construct(
        private \Aura\Router\Matcher $matcher,
        private \Zend\Diactoros\Response\EmitterInterface $emitter,
        private \Caridea\Container\Container $container,
        private \Labrys\ErrorLogger $errorLogger,
        private Contingency $contingency,
        private \Caridea\Auth\Service $authService
    ) {
    }

    /**
     * Route and dispatch a request, emitting the response, handling any errors.
     *
     * @param $request - The server request
     * @param $response - The response
     */
    public function execute(Request $request, Response $response): void
    {
        try {
            $this->emitter->emit($this->dispatch($request, $response));
        } catch (\Exception $e) {
            $this->errorLogger->log($e);
            $this->emitter->emit(
                $this->contingency->process($request, $response, $e)
            );
        }
    }

    /**
     * Perform the actual routing and dispatch, returning the Response
     *
     * @param $request - The server request
     * @param $response - The response
     * @return The new response
     * @throws Exception\Unroutable if route matching fails
     * @throws Exception\Dispatch if a controller method can't be invoked
     */
    protected function dispatch(Request $request, Response $response): Response
    {
        $request = $request->withAttribute('principal', $this->authService->getPrincipal());
        $route = $this->matcher->match($request);
        if (!$route) {
            $failedRoute = $this->matcher->getFailedRoute();
            throw Exception\Unroutable::fromRoute($failedRoute);
        } else {
            $handler = $route->handler;
            $controller = $this->container->getFirst($handler[0]);
            if ($controller === null) {
                throw new Exception\Dispatch("Controller instance not found: '{$handler[0]}'");
            }
            if (!method_exists($handler[0], $handler[1]) && !method_exists($handler[0], '__call')) {
                throw new Exception\Dispatch("Controller class '{$handler[0]}' doesn't have method '{$handler[1]}'");
            }
            foreach ($route->attributes as $k => $v) {
                $request = $request->withAttribute($k, $v);
            }
            $request = $request->withAttribute('route', $route);
            foreach ($this->container->getByType(Plugin::class) as $p) {
                $nr = $p->intercept($request, $response);
                if ($nr !== $response) {
                    return $nr;
                }
            }
            return call_user_method_array(
                $handler[1],
                $controller,
                [$request, $response]
            );
        }
    }
}
