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
namespace Labrys\Net\Exception;

/**
 * Exception for translation problems (e.g. json_decode on non-JSON).
 */
class Illegible extends \InvalidArgumentException implements \Labrys\Net\Exception
{
    /**
     * Creates a new Illegible.
     */
    public function __construct(
        private mixed $argument = "",
        string $message = "",
        int $code = 0,
        ?\Exception $cause = null
    ) {
        parent::__construct($message, $code, $cause);
    }

    /**
     * Gets the illegible argument
     *
     * @return - the illegible argument
     */
    public function getArgument() : mixed
    {
        return $this->argument;
    }
}
