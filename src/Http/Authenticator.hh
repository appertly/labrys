<?hh // strict
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

use Psr\Http\Message\ServerRequestInterface as Request;
use Psr\Http\Message\ResponseInterface as Response;

/**
 * Authentication plugin for the middleware dispatcher.
 *
 * @property \Caridea\Auth\Service $service The auth service
 * @property string $authUrl The URL for the standalone login form
 */
class Authenticator implements \Labrys\Route\Plugin
{
    /**
     * Creates a new AuthPlugin.
     *
     * @param $service - The auth service
     * @param $authUrl - The URL for the standalone login form
     */
    public function __construct(protected \Caridea\Auth\Service $service, protected string $authUrl)
    {
    }

    /**
     * Gets the plugin priority; larger means first.
     *
     * @return - The plugin priority
     */
    public function getPriority(): int
    {
        return 2000000;
    }

    /**
     * Allows a plugin to issue a response before the request is dispatched.
     *
     * @param $request - The server request
     * @param $response - The response
     * @return - The response
     */
    public function __invoke(Request $request, Response $response, (function (Request,Response): Response) $next) : Response
    {
        $request = $request->withAttribute('principal', $this->service->getPrincipal());
        try {
            return $next($request, $response);
        } catch (\Labrys\Route\Exception\Unroutable $e) {
            if ($e->getCode() === 403) {
                return $response->withStatus(303)->withHeader(
                    'Location', $this->authUrl . '?then=' . (string)$request->getRequestTarget()
                );
            }
            throw $e;
        }
    }
}
