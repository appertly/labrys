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

/**
 * Phone Number Link
 */
class :text:phone-number extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;

    category %flow, %phrase;
    children (pcdata | %flow)*;
    attribute ?Stringish phone = '',
        :xhp:html-element;

    protected function render(): XHPRoot
    {
        $text = trim((string) $this->:phone);
        $kids = $this->getChildren();
        if ($kids->isEmpty()) {
            $kids = $text;
        }
        if (strlen($text) > 0) {
            return <a href={"tel:$text"}>{$kids}</a>;
        } else {
            return <x:frag>{$kids}</x:frag>;
        }
    }
}
