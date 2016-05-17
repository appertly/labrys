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

use Psr\Http\Message\ResponseInterface;
use Psr\Http\Message\ServerRequestInterface;

/**
 * Handles errors that arise from the front controller.
 */
interface Contingency
{
    /**
     * Handles an exception.
     *
     * This is your chance for logging, changing the HTTP status header, and
     * rendering some kind of message for the client.
     *
     * @param $request - The request
     * @param $response - The response
     * @param $e - The exception to process
     * @return - The new response
     */
    public function process(ServerRequestInterface $request, ResponseInterface $response, \Exception $e): ResponseInterface;
}
