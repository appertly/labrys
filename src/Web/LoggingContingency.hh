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
 * Logs exceptions and then delegate real work to another `Contingency`.
 */
class LoggingContingency implements Contingency
{
    /**
     * Creates a new LoggingContingency.
     *
     * @param $errorLogger - The error logger
     * @param $delegate - The actual Contingency
     */
    public function __construct(private \Labrys\ErrorLogger $errorLogger, private Contingency $delegate)
    {
    }

    /**
     * Handles an exception.
     *
     * @param $request - The request
     * @param $response - The response
     * @param $e - The exception to process
     * @return - The new response
     */
    public function process(Request $request, Response $response, \Exception $e) : Response
    {
        $this->errorLogger->log($e);
        return $this->delegate->process($request, $response, $e);
    }
}