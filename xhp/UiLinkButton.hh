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
 * A link styled as a button.
 */
class :ui:link-button extends :x:element implements HasXHPHelpers
{
    use XHPHelpers, XHPAsync;

    category %flow, %phrase, %interactive;
    children (pcdata | %flow)*;
    attribute :a,
        ?Stringish icon;

    protected async function asyncRender(): Awaitable<XHPRoot>
    {
        return <a class="btn">
            <ui:icon icon={$this->:icon}/>
            <span class="button-text">{$this->getChildren()}</span>
        </a>;
    }
}
