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
 * Turns strings into XML
 */
class XmlMapper
{
    /**
     * Converts a string to a SimpleXMLElement.
     *
     * @param $xml - The string to convert
     * @throws \Labrys\Web\Exception\Illegible If the string is not XML
     */
    public function toXml(?string $xml) : \SimpleXMLElement
    {
        try {
            libxml_use_internal_errors(true);
            $xml = simplexml_load_string($xml);
            if ($xml === false) {
                $errors = libxml_get_errors();
                throw new Exception\Illegible($xml, "Invalid XML. " . implode(". ", $errors));
            }
            return $xml;
        } finally {
            libxml_clear_errors();
            libxml_use_internal_errors(true);
        }
    }
}
