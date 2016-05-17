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
namespace Labrys\Web;

use Psr\Http\Message\ServerRequestInterface as Request;
use Psr\Http\Message\ResponseInterface as Response;

/**
 * Authentication plugin for the front controller.
 */
class AuthPlugin implements Plugin
{
    /**
     * Creates a new AuthPlugin.
     *
     * @param $service - The auth service
     */
    public function __construct(private \Caridea\Auth\Service $service)
    {
    }

    /**
     * Allows a plugin to configure the request before any route matching.
     *
     * Implementations can return the original request if no modifications need
     * to take place.
     *
     * @param $request - The server request
     * @return - The request
     */
    public function advise(Request $request): Request
    {
        return $request->withAttribute('principal', $this->service->getPrincipal());
    }

    /**
     * Allows a plugin to issue a response before the request is dispatched.
     *
     * @param $request - The server request
     * @param $response - The response
     * @return - The response
     */
    public function intercept(Request $request, Response $response) : Response
    {
        return $response;
    }
}
