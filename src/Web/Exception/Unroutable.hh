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
namespace Labrys\Web\Exception;

/**
 * Exceptions for requests that cannot be routed to a controller method.
 */
class Unroutable extends \RuntimeException implements \Labrys\Web\Exception
{
    /**
     * Creates a new Unroutable.
     */
    public function __construct(
        string $message = "",
        int $code = 0,
        ?\Exception $cause = null,
        private ImmMap<string,string> $headers = ImmMap{}
    ) {
        parent::__construct($message, $code, $cause);
    }

    /**
     * Gets the headers to set.
     *
     * @return associative array of header names to values
     */
    public function getHeaders() : ImmMap<string,string>
    {
        return $this->headers;
    }

    /**
     * Creates a new Unroutable based on a failed Aura Route.
     *
     * @param $failedRoute The failed Aura Route
     * @return A new Unroutable
     */
    public static function fromRoute(\Aura\Router\Route $failedRoute) : Unroutable
    {
        switch ($failedRoute->failedRule) {
            case 'Aura\Router\Rule\Allows':
                $allows = ImmMap{'Allow' => implode(',', $failedRoute->allows)};
                return new self("Method Not Allowed", 405, null, $allows);
            case 'Aura\Router\Rule\Accepts':
                return new self("Not Acceptable", 406);
            case 'Labrys\Web\AuthRule':
                return new self("You Must Be Authenticated", 403);
            default:
                return new self("Not Found", 404);
        }
    }
}