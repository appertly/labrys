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
 * An item to appear in a navigation list
 */
class :labrys:nav-item extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;

    children (pcdata | %phrase)*;
    attribute :li,
        Stringish href @required,
        ?Stringish icon;

    protected function render(): XHPRoot
    {
        return <li class="nav-item">
            <a href={$this->:href} class={$this->getAttribute('class')} title={$this->getAttribute('title')}>
                <labrys:icon icon={$this->:icon} />
                <span class="nav-item-label">
                    {$this->getChildren()}
                </span>
            </a>
        </li>;
    }
}
