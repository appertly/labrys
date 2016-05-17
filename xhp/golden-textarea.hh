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
 * A textarea that should have golden ratio width to height.
 */
class :labrys:golden-textarea extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;

    category %flow, %phrase, %interactive;
    children (pcdata)*;
    attribute :textarea;

    protected function render(): XHPRoot
    {
        $ta = <textarea>{$this->getChildren()}</textarea>;
        $this->transferAllAttributes($ta);
        return <div class="golden-textarea">{$ta}</div>;
    }
}
