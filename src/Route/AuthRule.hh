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

use Psr\Http\Message\ServerRequestInterface as Request;
use Psr\Http\Message\ResponseInterface as Response;
use Caridea\Auth\Principal;

/**
 * Aura Router rule to test user is authenticated.
 */
class AuthRule implements \Aura\Router\Rule\RuleInterface
{
    /**
     * Check if the Request matches the Route.
     *
     * @param $request - The HTTP request
     * @param $route - The route.
     * @return - True on success, false on failure
     */
    public function __invoke(Request $request, \Aura\Router\Route $route): bool
    {
        return !$route->auth || !$this->getPrincipal($request)->isAnonymous();
    }

    /**
     * Gets the stored principal, or the anonymous user if none was found.
     *
     * @param $request - The HTTP request
     * @return - The authenticated principal
     */
    protected function getPrincipal(Request $request) : Principal
    {
        $principal = $request->getAttribute('principal', Principal::getAnonymous());
        invariant($principal instanceof Principal, "Type mismatch: principal");
        return $principal;
    }
}
