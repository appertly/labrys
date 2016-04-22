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
namespace Labrys\Net;

/**
 * A class that converts JSON data to equivalent Hack collections.
 */
class JsonMapper
{
    /**
     * Converts a JSON string to a Vector.
     *
     * @param $json - The JSON to convert
     * @throws \Labrys\Net\Exception\Illegible if the JSON is invalid
     */
    public function toVector(?string $json) : Vector<mixed>
    {
        $a = $json === null ? null : json_decode($json, true);
        if (!is_array($a)) {
            throw new Exception\Illegible($json, "Invalid JSON array");
        }
        return new Vector($a);
    }

    /**
     * Converts a JSON string to a Map.
     *
     * @param $json - The JSON to convert
     * @throws \Labrys\Net\Exception\Illegible if the JSON is invalid
     */
    public function toMap(?string $json) : Map<string,mixed>
    {
        $a = $json === null ? null : json_decode($json, true);
        if (!is_array($a)) {
            throw new Exception\Illegible($json, "Invalid JSON map");
        }
        return new Map($a);
    }
}
