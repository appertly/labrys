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
 * Logs exceptions that occur further inside the layer pipeline.
 */
class Reporter implements \Labrys\Route\Plugin
{
    /**
     * Creates a new error Reporter.
     *
     * @param $errorLogger - The error logger
     */
    public function __construct(private \Labrys\ErrorLogger $errorLogger)
    {
    }

    /**
     * Gets the plugin priority; larger means first.
     *
     * @return - The plugin priority
     */
    public function getPriority(): int
    {
        return PHP_INT_MAX - 1;
    }

    /**
     * Middleware request–response handling.
     *
     * Performs a typical passthru (i.e. `return $next($req, $res);`), but in
     * the event an `Exception` occurs, the `$errorLogger` is called, then the
     * `Exception` is rethrown (most likely to be caught higher in the queue).
     *
     * @param $request - The server request
     * @param $response - The response
     * @return - The response
     * @throws \Exception rethrows any exception that occurs after logging.
     */
    public function __invoke(Request $request, Response $response, (function (Request,Response): Response) $next): Response
    {
        try {
            return $next($request, $response);
        } catch (\Exception $e) {
            $this->errorLogger->log($e);
            throw $e;
        }
    }
}
