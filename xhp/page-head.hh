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
 * Page Header
 *
 * ```hack
 * <labrys:pagehead service={$service} />
 * ```
 */
class :labrys:pagehead extends :x:element implements HasXHPHelpers
{
    use XHPHelpers;

    category %flow, %heading;
    children empty;
    attribute :header,
        Labrys\View\Service service @required;

    protected function render(): XHPRoot
    {
        $blocks = <labrys:block-region />;
        $blocks->setContext('region', 'head');
        foreach ($this->:service->getBlocks('head') as $block) {
            $blocks->appendChild(
                <labrys:block block={$block} />
            );
        }
        return <header class="page-header" role="banner">
            {$blocks}
        </header>;
    }
}
