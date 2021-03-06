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

/**
 * A Button with an icon and a label.
 *
 * ```xml
 * <labrys:button name="foo" id="foo" icon="group">
 *     Hello World
 * </labrys:button>
 * ```
 */
class :labrys:button extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;

    category %flow, %phrase, %interactive;
    children (pcdata | %phrase)*;
    attribute :button,
        ?Stringish icon;

    protected function render(): XHPRoot
    {
        return <button class="btn">
            <labrys:icon icon={$this->:icon}/>
            <span class="button-text">{$this->getChildren()}</span>
        </button>;
    }
}
