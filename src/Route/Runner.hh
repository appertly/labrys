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
namespace Labrys\Route;

use Psr\Http\Message\ServerRequestInterface as Request;
use Psr\Http\Message\ResponseInterface as Response;

/**
 * Collects any Route plugins and runs them, returning the response.
 */
class Runner
{
    /**
     * The nested Runner
     */
    private \Ducts\Runner $runner;

    /**
     * Creates a new Runner.
     *
     * @param $c - The container
     */
    public function __construct(\Caridea\Container\Container $c)
    {
        $plugins = new Vector(array_values($c->getByType(Plugin::class)));
        /* HH_IGNORE_ERROR[1002]: Hack typechecker doesn't like spaceship  */
        usort($plugins, ($a, $b) ==> $b->getPriority() <=> $a->getPriority());
        /* HH_IGNORE_ERROR[4110]: I'm sure this works */
        $this->runner = new \Ducts\Runner($plugins);
    }

    /**
     * Middleware request–response handling.
     *
     * @param $request - The server request
     * @param $response - The response
     * @return - The response
     */
    public function run(Request $request, Response $response): Response
    {
        return $this->runner->run($request, $response);
    }
}
