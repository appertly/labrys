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
 * Exception for network misconfiguration
 */
class Misconfigured extends Unreachable
{
    private static ImmSet<int> $errors = ImmSet{
        CURLE_SSL_ENGINE_NOTFOUND, CURLE_OUT_OF_MEMORY,
        CURLE_SSL_ENGINE_SETFAILED, CURLE_READ_ERROR, CURLE_BAD_FUNCTION_ARGUMENT,
        CURLE_BAD_PASSWORD_ENTERED, CURLE_UNSUPPORTED_PROTOCOL, CURLE_LIBRARY_NOT_FOUND,
        CURLE_URL_MALFORMAT, CURLE_MALFORMAT_USER, CURLE_URL_MALFORMAT_USER,
        CURLE_FAILED_INIT, CURLE_FUNCTION_NOT_FOUND, CURLE_SSL_CERTPROBLEM,
        CURLE_FILE_COULDNT_READ_FILE, CURLE_SSL_CIPHER, CURLE_FILESIZE_EXCEEDED
    };

    /**
     * Determines whether the code returned from cURL is one this class supports.
     *
     * @param $code - The code
     * @return - whether this class should be used
     */
    public static function isUsable(int $code) : bool
    {
        return self::$errors->contains($code);
    }
}
