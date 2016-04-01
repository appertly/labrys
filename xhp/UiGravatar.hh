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
 * Displays a Gravatar user image
 */
class :ui:gravatar extends :x:element implements HasXHPHelpers
{
    use XHPHelpers, XHPAsync;

    const URL = "https://secure.gravatar.com/avatar/";

    category %flow, %phrase;
    children empty;
    attribute
        Stringish email @required,
        Stringish url = self::URL,
        Stringish default = 'identicon',
        enum {'g', 'pg', 'r', 'x'} rating = 'g',
        int size = 0,
        :xhp:html-element;

    protected async function asyncRender(): Awaitable<XHPRoot>
    {
        $hash = md5(strtolower(trim((string) $this->:email)));
        $qs = Map{
            'd' => $this->:default,
            'r' => $this->:rating
        };
        if ($this->:size > 0) {
            $qs['s'] = $this->:size;
        }
        $url = $this->:url . $hash . '?' . http_build_query($qs);
        $img = <img src={$url} alt="User avatar" />;
        if ($this->:size > 0) {
            $img->setAttribute('width', $this->:size);
            $img->setAttribute('height', $this->:size);
        }
        return <span class="gravatar" role="presentation">
            {$img}
        </span>;
    }
}
