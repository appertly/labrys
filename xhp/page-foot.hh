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
 * Page Footer
 *
 * ```hack
 * <labrys:pagefoot service={$service} />
 * ```
 */
class :labrys:pagefoot extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;

    category %flow;
    children empty;
    attribute :footer,
        Labrys\View\Service service @required;

    protected function render(): XHPRoot
    {
        $blocks = <labrys:block-region />;
        foreach ($this->:service->getBlocks('foot') as $block) {
            $blocks->appendChild(
                <labrys:block block={$block} />
            );
        }
        return <footer class="page-footer" role="contentinfo">
            {$blocks}
        </footer>;
    }
}
