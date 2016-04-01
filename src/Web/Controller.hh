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
 * @license   http://opensource.org/licenses/Apache-2.0 Apache 2.0 License
 */
namespace Labrys\Web;

use Psr\Http\Message\ServerRequestInterface as Request;
use Psr\Http\Message\ResponseInterface as Response;
use Caridea\Auth\Principal;
use Caridea\Http\PaginationFactory;

/**
 * Controller trait with some handy methods.
 */
trait Controller
{
    /**
     * Gets a Map of the request body content.
     *
     * @param \Psr\Http\Message\ServerRequestInterface $request The request
     * @return The Map of request body content
     */
    protected function getParsedBodyMap(Request $request) : Map<string,mixed>
    {
        $body = $request->getParsedBody();
        return is_array($body) ? new Map($body) : Map{};
    }

    /**
     * Cleanly writes the body to the response.
     *
     * @param \Psr\Http\Message\ResponseInterface $response The HTTP response
     * @param \Stringish $body The body to write
     * @return \Psr\Http\Message\ResponseInterface The same or new response
     */
    protected function write(Response $response, mixed $body) : Response
    {
        $response->getBody()->write((string) $body);
        return $response;
    }

    /**
     * Checks the If-Modified-Since header, maybe sending 304 Not Modified.
     *
     * @param \Psr\Http\Message\ServerRequestInterface $request The HTTP request
     * @param \Psr\Http\Message\ResponseInterface $response The HTTP response
     * @param int $timestamp The timestamp for comparison
     * @return \Psr\Http\Message\ResponseInterface The same or new response
     */
    protected function ifModSince(Request $request, Response $response, int $timestamp) : Response
    {
        $ifModSince = $request->getHeaderLine('If-Modified-Since');
        if ($ifModSince && $timestamp <= strtotime($ifModSince)) {
            return $response->withStatus(304, "Not Modified");
        }
        return $response;
    }

    /**
     * Redirects the user to another URL.
     *
     * @param \Psr\Http\Message\ResponseInterface $response The HTTP response
     */
    protected function redirect(Response $response, int $code, string $url) : Response
    {
        return $response->withStatus($code)->withHeader('Location', $url);
    }

    /**
     * Gets the stored principal, or the anonymous user if none was found.
     *
     * @param $request - The HTTP request
     * @return The authenticated principal
     */
    protected function getPrincipal(Request $request) : Principal
    {
        $principal = $request->getAttribute('principal', Principal::getAnonymous());
        invariant($principal instanceof Principal, "Type mismatch: principal");
        return $principal;
    }

    /**
     * Gets a pagination factory
     *
     * @return The pagination factory
     */
    protected function paginationFactory() : PaginationFactory
    {
        return new PaginationFactory();
    }
}
