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
 * A region for several blocks.
 *
 * ```xml
 * <labrys:block-region>
 *     <labrys:block />
 *     <labrys:block />
 * </labrys:block-region>
 * ```
 */
class :labrys:block-region extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;

    category %flow;
    children (:labrys:block)*;
    attribute :xhp:html-element;

    protected function render(): XHPRoot
    {
        return <div class="region clearfix">
            {$this->getChildren()}
        </div>;
    }
}
